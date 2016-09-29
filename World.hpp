#ifndef WORLD_HPP
#define WORLD_HPP
#include <Entity.hpp>
#include <tuple>

class World {
public:
    Entity getEntity(std::string id);
    std::tuple<bool, std::string> createEntity();
    bool deleteEntity(std::string id);
};

#endif // WORLD_HPP
