#ifndef REGFUNCTIONS_H
#define REGFUNCTIONS_H

#include <Windows.h>
#include <string>
#include <winreg.h>

//Prototypes
std::string RegGetKeyValue(std::string key, std::string subkey);
int RegAddKey(std::string key, std::string subkey, std::string value);
int RegDeleteValue(std::string keypath, std::string value);

#endif // REGFUNCTIONS_H
