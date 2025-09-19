#include <iostream>
#include <variant>
using namespace std;

#ifndef BINDATA_H
#define BINDATA_H


struct BinData{
    uint32_t TIMESTAMP;
    uint16_t id;
    variant<int, float, bool, double, long long> data;
};

#endif