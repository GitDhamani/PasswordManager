#include <openssl/sha.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

std::string sha1(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(hash[i]);
    }
    return ss.str();
}

/*
Usage:
    std::string input = "Hello, World!";
    std::string hash = sha1(input);

    std::cout << "Input: " << input << std::endl;
    std::cout << "SHA-1 Hash: " << hash << std::endl;

*/