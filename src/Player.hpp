#ifndef ARN_PLAYER_HPP
#define ARN_PLAYER_HPP

#include <string>

#include "Entity.hpp"

class Player : public Entity {
protected:
    std::string m_nickname;

public:
    std::string nickname() {
        return m_nickname;
    }
};

#endif // ARN_PLAYER_HPP
