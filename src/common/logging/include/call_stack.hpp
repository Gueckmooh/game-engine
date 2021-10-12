#pragma once

#include <memory>
#include <ostream>

namespace logging {
/**
 * A class able to extract the current callstack.
 */
class CallStack {
  public:
    CallStack();
    ~CallStack();

    /**
     * Recompute the current callstack.
     *
     * Specify how many fonctions in the stack to ignore.
     */
    void refresh(uint32_t discardCount = 0);

    /// Stringified version of the call stack since last refresh.
    std::string toString() const;

  public:
    /// Internal pointer to implementation.
    class Backend;

  private:
    Backend* fpBackend = nullptr;
};
}   // namespace logging

std::ostream& operator<<(std::ostream& stream, logging::CallStack& callStack);
