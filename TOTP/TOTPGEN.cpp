#include <chrono>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <QMessageBox>

//Add these to your CMakeLists
//target_include_directories(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP)
//target_link_libraries(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP/libcrypto.lib)
//target_link_libraries(Password_Manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/TOTP/libssl.lib)

const char* base32_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

//Base32 to Binary
std::vector<uint8_t> base32_decode(const std::string& encoded_string)
{
    std::vector<uint8_t> result;
    uint32_t buffer = 0;
    int bits = 0;
    for (char c : encoded_string) {
        if (c == '=')
            break;
        int value = 0;
        for (int i = 0; i < 32; i++) {
            if (base32_chars[i] == c) {
                value = i;
                break;
            }
        }
        if (value == 0 && c != 'A') {
            throw std::invalid_argument("invalid base32 character");
        }
        buffer = (buffer << 5) | value;
        bits += 5;
        if (bits >= 8) {
            result.push_back(buffer >> (bits - 8));
            bits -= 8;
        }
    }
    return result;
}

//Usage: std::string code = generate_TOTP(B32Seed, "SHA1", 6, 30, 2);
//The last parameter is the tolerance window, so in this case 2 windows previous and forward.
std::string generate_TOTP(const std::string& secret, std::string algorithm, uint8_t digits, uint8_t time_window, int interval)
{
    std::vector<uint8_t> secret1;
    try
    {
        secret1 = base32_decode(secret);
    }
    catch (const std::invalid_argument& e)
    {
        QMessageBox::information(0, "Error", "Invalid Base32 Character. Quitting.");
        exit(1);
    }


    // Calculate the TOTP
    auto time_since_epoch = std::chrono::time_point_cast<std::chrono::seconds>(
    std::chrono::system_clock::now()).time_since_epoch();
    std::chrono::seconds time_in_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
    uint64_t counter = time_in_seconds.count() / time_window; // 30-second intervals
    counter = counter + interval;
	
	//Reversing Byte Order
	uint8_t counter_bytes[8] = {0};
    for (int i = 7; i >= 0; i--) {
        counter_bytes[i] = counter & 0xff;
        counter >>= 8;
    }
	
	//Generate an HMAC-SHA1 hash using the byte array and a secret key. The secret key should be shared between the server and the client and kept secure.

    std::vector<uint8_t> hash(SHA_DIGEST_LENGTH,0);

    if (algorithm == "SHA1")
    {
        HMAC(EVP_sha1(), secret1.data(), secret1.size(),
             counter_bytes, sizeof(counter_bytes),
             &hash[0], nullptr);
    }
    else
    {
        std::fill(hash.begin(), hash.end(), 0);
        hash.clear();
        hash.resize(SHA256_DIGEST_LENGTH);

        HMAC(EVP_sha256(), secret1.data(), secret1.size(),
             counter_bytes, sizeof(counter_bytes),
             &hash[0], nullptr);
    }

    //Take the last 4 bits of the hash and use them to calculate an offset value. The offset value is used to select 4 bytes from the hash.
    int offset;
    if (algorithm == "SHA1")
    offset = hash[SHA_DIGEST_LENGTH-1] & 0x0f;
    else
    offset = hash[SHA256_DIGEST_LENGTH-1] & 0x0f;

    uint32_t truncated_hash =
        (hash[offset] & 0x7f) << 24 |     //7f is 0111 1111
        (hash[offset+1] & 0xff) << 16 |
        (hash[offset+2] & 0xff) << 8 |
        (hash[offset+3] & 0xff);
		
	//Apply a masking operation to the integer to obtain a 6-digit TOTP code. The masking operation involves taking the modulo of the integer with 10^6 and padding the result with leading zeroes if necessary.	
    truncated_hash %= 1000000; // 6-digit code
    std::stringstream ss;
	
	//Create a six digit format field to output the hash into and stream it into the string.
    ss << std::setfill('0') << std::setw(6) << truncated_hash;
    return ss.str();
}


