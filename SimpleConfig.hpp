#ifndef SIMPLE_CONFIG_HPP
#define SIMPLE_CONFIG_HPP

#include <map>
#include <sstream>
#include <string>

class SimpleConfig {
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

#endif // SIMPLE_CONFIG_HPP
