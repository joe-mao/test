#include "global.h"



Global * Global::instance = nullptr;

Global * Global::getInstance()
{
    if(instance == nullptr){
        instance = new Global();
    }
    return instance;
}


Global::Global()
{

}

