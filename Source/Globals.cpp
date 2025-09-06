#include "Globals.h"

Globals& Globals::GetInstance() {
    static Globals instance;
    return instance;
}