#include <file_watcher/file_watcher.hpp>

#include <macros/macros.hpp>

#ifdef __FILE_WATCHER_USE_INOTIFY__
#    include "backends/inotify/file_watcher_impl.hpp"
#else
#    error "[file_watcher.file_watcher] Unsupported platform for file watching"
#endif

namespace file_watcher {

$pimpl_class(FileWatcher);
$pimpl_class_delete(FileWatcher);

$pimpl_method(FileWatcher, uint32_t, watch, const fs::Path&, path);

std::optional<FileWatchEvent> FileWatcher::pollEvent() { return fpImpl->popEvent(); }

}   // namespace file_watcher
