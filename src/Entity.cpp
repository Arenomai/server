#include "Entity.hpp"

#include <iostream>

#include "net/DatabaseConnection.hpp"

Entity::Entity() {
    m_uuid = UUID::make();
}
