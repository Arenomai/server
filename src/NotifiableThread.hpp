#ifndef ARN_NOTIFIABLE_THREAD_HPP
#define ARN_NOTIFIABLE_THREAD_HPP

#include "Notifiable.hpp"

namespace arn {

class NotifiableThread {
public:
    std::map<std::string, std::string> entries;

    SimpleConfig() {}
    SimpleConfig(const std::string &text);

    void parseText(std::istream &stream);
    inline void parseText(const std::string &text) {
        std::istringstream iss(text);
        parseText(iss);
    }
    std::string asText() const;
};

}

#endif /* ARN_NOTIFIABLE_THREAD_HPP */
