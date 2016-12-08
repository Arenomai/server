#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "net/DatabaseConnection.hpp"
#include "UUID.hpp"

class Entity {
private:
    UUID m_uuid;

    // Constructors
    Entity();

    // Methods


public:
    UUID uuid() {
        return m_uuid;
    }
};

#endif // ENTITY_HPP
