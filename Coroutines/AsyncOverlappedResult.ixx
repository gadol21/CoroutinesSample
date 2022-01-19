module;
#include <Windows.h>
#include <memory>

export module AsyncOverlappedResult;
import Event;

export class AsyncOverlappedResult {
public:
    AsyncOverlappedResult() 
        : m_overlapped(std::make_unique<OVERLAPPED>()),
          m_event(Event::Type::ManualReset) {
        m_overlapped->hEvent = m_event.get();
    }

    OVERLAPPED& getOverlapped() { return *m_overlapped; }
    const OVERLAPPED& getOverlapped() const { return *m_overlapped; }

    // Syncronously wait for result and return it
    unsigned int get() const {
        WaitForSingleObject(m_event.get(), INFINITE);
        return (unsigned int)m_overlapped->InternalHigh;
    }

    bool hasResult() const {
        return WaitForSingleObject(m_event.get(), 0) == WAIT_OBJECT_0;
    }

private:
    std::unique_ptr<OVERLAPPED> m_overlapped;
    Event m_event;
};
