#ifndef ARN_WORLD_HPP
#define ARN_WORLD_HPP

#include "Entity.hpp"
#include <tuple>

class World {
public:
    Entity getEntity(std::string id);
    std::tuple<bool, std::string> createEntity();
    bool deleteEntity(std::string id);
};

#endif // ARN_WORLD_HPP
