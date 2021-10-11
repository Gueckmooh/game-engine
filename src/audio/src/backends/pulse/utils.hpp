#pragma once

#include <pulse/sample.h>

#include <audio/sample_format.hpp>

namespace audio {
namespace utils {
inline pa_sample_format_t formatToByteCount(SampleFormat format) {
    switch (format) {
    case SampleFormat::Uint8: return PA_SAMPLE_U8;
    case SampleFormat::Int16BE: return PA_SAMPLE_S16BE;
    case SampleFormat::Int16LE: return PA_SAMPLE_S16LE;
    case SampleFormat::Int32BE: return PA_SAMPLE_S32BE;
    case SampleFormat::Int32LE: return PA_SAMPLE_S32LE;
    case SampleFormat::Float32: return PA_SAMPLE_FLOAT32;
    case SampleFormat::Unknown: return PA_SAMPLE_INVALID;
    }
    return PA_SAMPLE_INVALID;
}
}   // namespace utils
}   // namespace audio
