#ifndef CREDENTIALSMANAGER_H
#define CREDENTIALSMANAGER_H
#include "Credentials.h"

class CredentialsManager {
    private:
        static Credentials instance;

    public:
        static Credentials& getInstance() {
            return instance;
        }
};
#endif