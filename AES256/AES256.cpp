#include <iostream>
#include <string>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>
#include <cstring>

std::string encryptString(const std::string &plaintext, const std::string &key) {
    // Initialize OpenSSL cipher context
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Set up encryption parameters
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                       reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

    int c_len = plaintext.length() + AES_BLOCK_SIZE;
    unsigned char *ciphertext = new unsigned char[c_len];

    // Perform encryption
    int len;
    EVP_EncryptInit_ex(ctx, NULL, NULL, NULL, NULL);
    EVP_EncryptUpdate(ctx, ciphertext, &len,
                      reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
    int c_len1 = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);

    int c_len2 = c_len1 + len;

    // Convert ciphertext to Base64
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, ciphertext, c_len2);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string base64Ciphertext(bufferPtr->data, bufferPtr->length);

    // Clean up
    BIO_free_all(bio);
    EVP_CIPHER_CTX_free(ctx);
    delete[] ciphertext;

    return base64Ciphertext;
}

std::string decryptString(const std::string &base64Ciphertext, const std::string &key) {
    // Decode Base64
    BIO *bio, *b64;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(base64Ciphertext.c_str(), base64Ciphertext.length());
    bio = BIO_push(b64, bio);

    const int maxDecodedLen = base64Ciphertext.length() * 3 / 4 + 1;
    unsigned char *decodedCiphertext = new unsigned char[maxDecodedLen];
    int decodedLen = BIO_read(bio, decodedCiphertext, base64Ciphertext.length());
    BIO_free_all(bio);

    // Initialize OpenSSL cipher context
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    // Set up decryption parameters
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,
                       reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

    // Perform decryption
    int p_len = decodedLen + AES_BLOCK_SIZE;
    unsigned char *plaintext = new unsigned char[p_len];

    int len;
    EVP_DecryptInit_ex(ctx, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(ctx, plaintext, &len, decodedCiphertext, decodedLen);
    int p_len1 = len;
    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    int p_len2 = p_len1 + len;

    std::string result(reinterpret_cast<char*>(plaintext), p_len2);

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    delete[] decodedCiphertext;
    delete[] plaintext;

    return result;
}

/*
Usage:
    // Set your encryption key
    std::string key = "0123456789abcdef0123456789abcdef";

    // Text to encrypt
    std::string plaintext = "Hello, world!";

    // Encrypt
    std::string encrypted = encryptString(plaintext, key);
    std::cout << "Encrypted: " << encrypted << std::endl;

    // Decrypt
    std::string decrypted = decryptString(encrypted, key);
    std::cout << "Decrypted: " << decrypted << std::endl;

*/
