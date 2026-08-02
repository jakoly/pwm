#ifndef ENCRYPT_H
#define ENCRYPT_H
#include <string>
#include <argon2.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <random>

class encrypt
{
public:
    encrypt();
};

std::string encryptText(std::string password);

#endif // ENCRYPT_H
