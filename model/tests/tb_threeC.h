
#pragma once

#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "Cache.h"
#include "tb_util.h"
#include "ReplacementPolicy.h"
#include <iostream>
#include <set>
#include <cmath>
#include <vector>


bool test_conflictThree(const config& data) {

    std::vector<uint32_t> address;   

    for (int i=0;i<100;i++) {

        if (i%2 == 0 ) {
            address.push_back(0);;
        } else {
            address.push_back(2048);;
        }
            
    }

    ThreeC result = run_all(data, address);

    if (result.compulsory ==2 && result.capacity == 0 &&result.conflict ==0) {
        std::cerr << "CONFLICT TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "CONFLICT TEST: FAIL" << "\n";
        return false;
    }

}


bool test_capacityThree(const config& data) {

    std::vector<uint32_t> address;   

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);;
    }

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);;
    }

    ThreeC result = run_all(data, address);

    if (result.compulsory ==256 && result.capacity == 256 &&result.conflict ==0) {
        std::cerr << "CAPACITY TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "CAPACITY TEST: FAIL" << "\n";
        return false;
    }

}


void test_capacityOPTvLRU(const config& data) {

    std::vector<uint32_t> address;   
    OPT OPTinstance(data);
    CacheAnalysis cacheAnalysis(data);

    OPT_LRU_data comparison;

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);;
    }

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);;
    }

    int result = OPTinstance.run(data, address);
    int LRUresult = cacheAnalysis.run(data, address);

    comparison.OPT_misses = result;
    comparison.LRU_misses = LRUresult;

    if (comparison.OPT_misses>0) {
        comparison.ratio = 1.0*(comparison.LRU_misses)/comparison.OPT_misses;
    }

    if (result == 384 && LRUresult == 512) {
        std::cerr << "OPT & LRU MISS TEST: PASS" << "\n";
    } else {
        std::cerr << "OPT & LRU TEST: FAIL" << "\n";
    }

}