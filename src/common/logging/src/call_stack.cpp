#include <logging/call_stack.hpp>
#include <string>

#ifndef __USE_WINDOWS__
#    include "./call_stack_backends/gcc/call_stack_backend.hpp"
#else
#    include "./call_stack_backends/mingw/call_stack_backend.hpp"
#endif

std::ostream& operator<<(std::ostream& stream, logging::CallStack& callStack) {
    stream << callStack.toString();
    return stream;
}

namespace logging {

CallStack::CallStack() { fpBackend = new CallStackBackend(); }
CallStack::~CallStack() { delete fpBackend; }

void CallStack::refresh(uint32_t discardCount) { fpBackend->refresh(discardCount); }
std::string CallStack::toString() const { return fpBackend->toString(); }

}   // namespace logging
