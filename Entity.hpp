#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Connection.hpp"

class Entity {
public:
    // Constructors
    Entity();
    Entity(std::string type, std::string command);

    // Methods
    std::string getInfos();
    bool createInDatabase();
    bool deleteFromDatabase();
};

#endif // ENTITY_HPP
