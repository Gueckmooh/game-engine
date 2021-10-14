#include "file_watcher_impl.hpp"

#include <fcntl.h>
#include <file_watcher/file_watcher_event.hpp>
#include <fstream>
#include <sys/inotify.h>
#include <unistd.h>

#include <logging/logger.hpp>

using namespace logging;

namespace {
void waitEvents(int fileDescriptor, const std::atomic<bool>* watching,
                const std::unordered_map<int, fs::Path>* watchDescriptors,
                std::queue<file_watcher::FileWatchEvent>* eventsQueue) {
    constexpr const auto eventBufferSize = sizeof(inotify_event) + 512u + 1u;
    static char buffer[eventBufferSize];

    while (*watching) {
        const auto length = read(fileDescriptor, buffer, eventBufferSize);
        if (!length || !*watching) break;
        auto& iEvent = *reinterpret_cast<inotify_event*>(buffer);

        file_watcher::FileWatchEvent event;
        event.path    = watchDescriptors->at(iEvent.wd);
        event.watchId = iEvent.wd;

        if (iEvent.len) event.path /= iEvent.name;

        switch (iEvent.mask) {
        case IN_ACCESS: continue;
        case IN_ATTRIB: continue;
        case IN_CLOSE_WRITE: continue;
        case IN_CLOSE_NOWRITE: continue;
        case IN_CREATE: event.type = file_watcher::FileWatchEvent::Type::Created; break;
        case IN_DELETE: event.type = file_watcher::FileWatchEvent::Type::Deleted; break;
        case IN_DELETE_SELF:
            event.type = file_watcher::FileWatchEvent::Type::Deleted;
            break;
        case IN_MODIFY: event.type = file_watcher::FileWatchEvent::Type::Modified; break;
        case IN_MOVE_SELF:
            event.type = file_watcher::FileWatchEvent::Type::Created;
            break;
        case IN_MOVED_FROM:
            event.type = file_watcher::FileWatchEvent::Type::Deleted;
            break;
        case IN_MOVED_TO: event.type = file_watcher::FileWatchEvent::Type::Created; break;
        case IN_OPEN: continue;
        default: continue;
        }

        eventsQueue->emplace(event);
    }
}
}   // namespace

namespace file_watcher {

FileWatcher::Impl::Impl()
    // @todo Let the user configure the mask
    : m_mask(IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVED_FROM
             | IN_MOVED_TO)
    , m_fileDescriptor(inotify_init())
    , m_watchThread(waitEvents, m_fileDescriptor, &m_watching, &m_watchDescriptors,
                    &m_eventsQueue) {}

FileWatcher::Impl::~Impl() {
    m_watching = false;

    for (const auto& watchDescriptor : m_watchDescriptors) {
        inotify_rm_watch(m_fileDescriptor, watchDescriptor.first);
    }

    // @note To break the blocking read from waitEvents(),
    // we access /dev/null right here - generating an event.
    auto watchDescriptor =
        inotify_add_watch(m_fileDescriptor, "/dev/null", IN_ALL_EVENTS);
    std::ifstream file("/dev/null");
    file.close();
    m_watchThread.join();
    inotify_rm_watch(m_fileDescriptor, watchDescriptor);

    close(m_fileDescriptor);
}

uint32_t FileWatcher::Impl::watch(const fs::Path& path) {
    auto effectivePath = fs::canonical(path);

    // @note Can't make that descriptor non-blocking using fcntl for some reason.
    // This led us to us a thread.
    auto watchDescriptor =
        inotify_add_watch(m_fileDescriptor, effectivePath.c_str(), m_mask);

    if (watchDescriptor == -1) {
        logger.warning("file_watcher.file_watcher")
            << "Unable to watch " << path << "." << std::endl;
        return -1u;
    }

    m_watchDescriptors[watchDescriptor] = effectivePath;
    return watchDescriptor;
}

std::optional<FileWatchEvent> FileWatcher::Impl::popEvent() {
    if (m_eventsQueue.empty()) return std::nullopt;

    auto event = m_eventsQueue.front();
    m_eventsQueue.pop();
    return event;
}

}   // namespace file_watcher
