#pragma once

#include <logging/call_stack.hpp>

#include "../../call_stack_backend.hpp"

namespace logging {
/**
 * MinGW-based implmentation of the call-stack generation.
 */
class CallStackBackend final : public CallStack::Backend {
  public:
    // ICallStackImpl
    void refresh(uint32_t discardCount) override final;
};
}   // namespace logging
