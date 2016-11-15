#ifndef __COMPONENTMANAGER_H__
#define __COMPONENTMANAGER_H__

#include <string>



namespace Base {

void RegisterComponent(void *thiz, std::string name);

void UnRegisterComponent(std::string name);

void *FindComponent(string name);

}

#endif