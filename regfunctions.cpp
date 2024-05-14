#include "regfunctions.h"


std::string RegGetKeyValue(std::string key, std::string subkey)
{
    HKEY hKey;
    DWORD dwType, dwSize;
    char buffer[700]; //Max Value Length: You can change this.

    // Extract root key from key path
    std::string rootKey = key.substr(0, key.find('\\'));

    // Convert root key handle to HKEY
    HKEY hRootKey;
    if (rootKey == "HKEY_CLASSES_ROOT") {
        hRootKey = HKEY_CLASSES_ROOT;
    }
    else if (rootKey == "HKEY_CURRENT_CONFIG") {
        hRootKey = HKEY_CURRENT_CONFIG;
    }
    else if (rootKey == "HKEY_CURRENT_USER") {
        hRootKey = HKEY_CURRENT_USER;
    }
    else if (rootKey == "HKEY_LOCAL_MACHINE") {
        hRootKey = HKEY_LOCAL_MACHINE;
    }
    else if (rootKey == "HKEY_USERS") {
        hRootKey = HKEY_USERS;
    }
    else {
        return "Invalid root key handle.";
    }

    //Remove Root key from the string
    key.erase(0, rootKey.length() + 1);  //The +1 accounts for the null terminator at the end of the rootkey.

    // open the registry key
    LONG result = RegOpenKeyExA(hRootKey, key.c_str(), 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS)
    {
        //qDebug() << "Failed to open registry key.";
        return "Key Not Found";
    }

    // get the value of the registry key
    dwSize = sizeof(buffer);
    result = RegQueryValueExA(hKey, subkey.c_str(), NULL, &dwType, (LPBYTE)&buffer, &dwSize);

    if (result != ERROR_SUCCESS)
    {
        //qDebug() << "Failed to retrieve registry value.";
        return "Key Not Found";
    }

    // close the registry key
    RegCloseKey(hKey);

    return buffer;
}


int RegAddKey(std::string key, std::string subkey, std::string value)
{
    HKEY hKey;
    DWORD dwDisposition;

    // Extract root key from key path
    std::string rootKey = key.substr(0, key.find('\\'));

    // Convert root key handle to HKEY
    HKEY hRootKey;
    if (rootKey == "HKEY_CLASSES_ROOT") {
        hRootKey = HKEY_CLASSES_ROOT;
    }
    else if (rootKey == "HKEY_CURRENT_CONFIG") {
        hRootKey = HKEY_CURRENT_CONFIG;
    }
    else if (rootKey == "HKEY_CURRENT_USER") {
        hRootKey = HKEY_CURRENT_USER;
    }
    else if (rootKey == "HKEY_LOCAL_MACHINE") {
        hRootKey = HKEY_LOCAL_MACHINE;
    }
    else if (rootKey == "HKEY_USERS") {
        hRootKey = HKEY_USERS;
    }
    else {
        //return "Invalid root key handle.";
        return 0;
    }

    //Remove Root key from the string
    key.erase(0, rootKey.length() + 1);

    // create the registry key
    LONG result = RegCreateKeyExA(hRootKey, key.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisposition);

    if (result != ERROR_SUCCESS)
    {
        //qDebug() << "Failed to create registry key.";
    }

    // set a value for the registry key
    const char* valueData = value.c_str();
    result = RegSetValueExA(hKey, subkey.c_str(), 0, REG_SZ, (const BYTE*)valueData, strlen(valueData) + 1);

    if (result != ERROR_SUCCESS)
    {
        //qDebug() << "Failed to set registry key.";
    }

    // close the registry key
    RegCloseKey(hKey);
    return 1;

}

int RegDeleteValue(std::string key, std::string value)
{
    HKEY hKey;
    DWORD dwType, dwSize;

    // Extract root key from key path
    std::string rootKey = key.substr(0, key.find('\\'));

    // Convert root key handle to HKEY
    HKEY hRootKey;
    if (rootKey == "HKEY_CLASSES_ROOT") {
        hRootKey = HKEY_CLASSES_ROOT;
    }
    else if (rootKey == "HKEY_CURRENT_CONFIG") {
        hRootKey = HKEY_CURRENT_CONFIG;
    }
    else if (rootKey == "HKEY_CURRENT_USER") {
        hRootKey = HKEY_CURRENT_USER;
    }
    else if (rootKey == "HKEY_LOCAL_MACHINE") {
        hRootKey = HKEY_LOCAL_MACHINE;
    }
    else if (rootKey == "HKEY_USERS") {
        hRootKey = HKEY_USERS;
    }
    else {
        return 0;
    }

    //Remove Root key from the string
    key.erase(0, rootKey.length() + 1);  //The +1 accounts for the null terminator at the end of the rootkey.

    // open the registry key
    LONG result = RegOpenKeyExA(hRootKey, key.c_str(), 0, KEY_READ | KEY_WRITE, &hKey);
    if (result != ERROR_SUCCESS)
    {
        //qDebug() << "Failed to Open registry key.";
    }


    LONG deleteStatus = RegDeleteValueA(hKey, value.c_str());
    if (deleteStatus != ERROR_SUCCESS) {
        //qDebug() << "Error deleting value: " << deleteStatus << std::endl;
        RegCloseKey(hKey);
        return 0;
    }


    RegCloseKey(hKey);
    return 1; //Success

}
