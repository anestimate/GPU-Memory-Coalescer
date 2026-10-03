#pragma once
#include <stdexcept>
#include <array>
#include <cstdint>
#include <bitset>
#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include <iostream>
#include <set>
#include <cmath>
#include <vector>


class LRU {

    public:
    std::vector<uint32_t> age;

    LRU(const config& data) : age(data.num_sets*data.ways) {
        for (int i = 0;i<data.num_sets*data.ways; i++) {
            age[i] = i % data.ways;
        }
    }
    
    void update(const config& data,int set, int way){

        for (int i = 0;i<data.ways;i++) {
            if (age[set*data.ways+i] < age[set*data.ways+way]) {
                age[set*data.ways+i]++;
            }
        }

        age[set*data.ways+way]=0;

    }

    int replace (const config& data, int set) {

        for (int i = 0;i<data.ways;i++) {
            if (age[set*data.ways+i] ==(uint32_t)(data.ways-1)) {
                update(data, set, i);
                return i;
            }
        }

        throw std::runtime_error("ERROR: AGE TIMING MISMATCH");
    }


};