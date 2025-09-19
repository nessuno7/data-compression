#include <iostream>
#include <unordered_map>
#include <optional>
#include "BinValueLine.h"
using namespace std;

#ifndef BINMETADATA_H
#define BINMETADATA_H



struct BinMetaData{ //struct of a data line
    string metadata;
    unordered_map<uint16_t, BinValueLine> ValueLines; //list of all value lines

    /*optional<BinValueLine> getValueLine(uint16_t id) const {
        auto it = ValueLines.find(id);
        if (it != ValueLines.end()) {
            return it->second; // copy returned
        }
        return nullopt;   // nothing found
    }*/

    // Overload that returns pointer (no copy, modifiable)
    BinValueLine* getValueLine(uint16_t id) {
        unordered_map<uint16_t, BinValueLine>::iterator it = ValueLines.find(id);
        if (it != ValueLines.end()) {
            return &(it->second);
        }
        return nullptr;
    }
};

#endif