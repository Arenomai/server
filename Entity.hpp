#ifndef ENTITY_H
#define ENTITY_H
#include "Connection.hpp"
class Entity
{
public:
    //Constructors
    Entity();
    Entity(std::string type, std::string command);

    //Methods
    std::string getInfos();
    bool createInDatabase();
    bool deleteFromDatabase();
};

#endif // ENTITY_H
