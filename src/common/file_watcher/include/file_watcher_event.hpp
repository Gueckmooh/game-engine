#pragma once

#include <string>

#include <macros/macros.hpp>

#include "filesystem.hpp"

namespace file_watcher {
/**
 * A file or directory event.
 */

struct FileWatchEvent {
    // $enum_class_nonamespace(FileWatchEvent, Type, Modified, Created, Deleted);
    enum class Type { Modified, Created, Deleted };

    Type type;
    fs::Path path;
    uint32_t watchId;
};

constexpr const char* stringify(FileWatchEvent::Type value) {
    switch (value) {
    case FileWatchEvent::Type::Modified: {
        return "Type"
               "::"
               "Modified";
    };
    case FileWatchEvent::Type::Created: {
        return "Type"
               "::"
               "Created";
    };
    case FileWatchEvent::Type::Deleted: {
        return "Type"
               "::"
               "Deleted";
    };
        ;
    default: break;
    }
    return "Type"
           "::<Unknown>";
}

}   // namespace file_watcher

inline std::ostream& operator<<(std::ostream& os,
                                file_watcher::FileWatchEvent::Type value) {
    os << stringify(value);
    return os;
}
