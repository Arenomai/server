#include "SimpleConfig.hpp"

SimpleConfig::SimpleConfig(const std::string &text) {
    parseText(text);
}

void SimpleConfig::parseText(std::istream &stream) {
    std::string line;
    while (getline(stream, line)) {
        if (line[0] == '#') {
          continue;
        }
        std::string::size_type eq_pos = line.find_first_of('=');
        if (eq_pos == std::string::npos) {
          continue;
        }
        entries.emplace(std::piecewise_construct,
            std::forward_as_tuple(line.substr(0, eq_pos)),
            std::forward_as_tuple(line.substr(eq_pos + 1)));
    }
}

std::string SimpleConfig::asText() const {
    std::ostringstream oss;
    for (const std::pair<std::string, std::string> &entry : entries) {
      oss << entry.first << '=' << entry.second << std::endl;
    }
    return oss.str();
}
