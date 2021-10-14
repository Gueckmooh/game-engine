#pragma once

#include <file_watcher/file_watcher_event.hpp>
#include <optional>
#include <string>

#include <macros/macros.hpp>

namespace file_watcher {
class FileWatcher {
  public:
    FileWatcher();
    ~FileWatcher();

    /// Watch a file or a directory. Returns a watch Id.
    uint32_t watch(const fs::Path& path);

    /// Grab the next event if any.
    std::optional<FileWatchEvent> pollEvent();

  private:
    $pimpl_decl;
    // class Backend;
    // Backend* fpBackend = nullptr;
};
}   // namespace file_watcher
