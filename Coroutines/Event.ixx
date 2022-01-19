module;
#include <Windows.h>
#include <stdexcept>
#include <wil\resource.h>

export module Event;
import Exceptions;

export class Event {
public:
    enum class Type {
        AutoReset,
        ManualReset
    };

    Event(Type type) {
        m_event.reset(CreateEvent(nullptr, type == Type::ManualReset, FALSE, nullptr));
        if (!m_event.is_valid()) {
            throw Win32Exception("CreateEvent");
        }
    }

    HANDLE get() const {
        return m_event.get();
    }

private:
    wil::unique_event m_event;
};