#pragma once

#include <audio/sample_format.hpp>

namespace audio {
namespace utils {
inline uint16_t formatToBitCount(SampleFormat format) {
    switch (format) {
    case SampleFormat::Uint8: return 8;
    case SampleFormat::Int16BE: return 16;
    case SampleFormat::Int16LE: return 16;
    case SampleFormat::Int32BE: return 32;
    case SampleFormat::Int32LE: return 32;
    case SampleFormat::Float32: return 32;
    case SampleFormat::Unknown: return 0;
    }
    return 0;
}
}   // namespace utils
}   // namespace audio
