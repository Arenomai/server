#ifndef ARN_NOTIFIABLE_HPP
#define ARN_NOTIFIABLE_HPP

#include <mutex>
#include <queue>
#include <sys/eventfd.h>
#include <unistd.h>

#include "platform/Types.hpp"

namespace arn {

template<typename Event>
class Notifiable {
protected:
    std::queue<Event> evt_queue;
    std::mutex evt_mtx;
    int evt_fd;

public:
    Notifiable() {
        evt_fd = ::eventfd(0, 0);
    }

    ~Notifiable() {
        ::close(evt_fd);
    }

    template<typename... Args>
    void notify(Args&&... args) {
        {
            std::lock_guard<std::mutex> evt_mtx_guard(evt_mtx);
            evt_queue.emplace(std::forward<Args>(args)...);
        }
        const uint64 one = 1;
        ::write(evt_fd, &one, sizeof(one));
    }

    int fd() const {
        return evt_fd;
    }
};

}

#endif /* ARN_NOTIFIABLE_THREAD_HPP */
