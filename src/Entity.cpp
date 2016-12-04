#include "Entity.hpp"

#include <iostream>

#include "DatabaseConnection.hpp"

Entity::Entity() {
    m_uuid = UUID::make();
}
