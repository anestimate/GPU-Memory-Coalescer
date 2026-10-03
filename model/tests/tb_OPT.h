#pragma once

#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "Cache.h"
#include "tb_util.h"
#include <iostream>
#include <set>
#include <cmath>
#include <vector>
#include "OPT.h"
#include <stdexcept>
#include "ThreeC.h"


void test_OPT_misses(const config& data) {

    std::vector<uint32_t> address; 
    config d = data;
    d.ways = 4;
    d.cache_bytes = 256;
    d.line_bytes = 64;
    d.finalise();

    OPT OPTinstance(d);

    for (int i=0;i<4*64;i=i+64) {
        address.push_back(i);
    }

    address.push_back(0);
    address.push_back(5*64);
    address.push_back(64);
    address.push_back(128);
    address.push_back(192);
    
    int misses = OPTinstance.run(d,address);


    if (misses == 5) {
        std::cerr << "OPT Miss Test: Passed!" << "\n";
    } else {
        std::cerr << "Misses: "<< misses << "\n";
        throw std::runtime_error("ERROR: Miss Test Failed!");
    }

} 

void test_OPT_next_use(const config& data) {

    std::vector<uint32_t> address; 
    std::vector<size_t> next_use;
    config d = data;
    d.ways = 4;
    d.cache_bytes = 256;
    d.line_bytes = 64;
    d.finalise();

    OPT OPTinstance(d);

    for (int i=0;i<4*64;i=i+64) {
        address.push_back(i);
    }

    address.push_back(0);
    address.push_back(5*64);
    address.push_back(64);
    address.push_back(128);
    address.push_back(192);
    
    next_use = OPTinstance.get_next_use(d,address);


    if (next_use[0]==4 &&  next_use[1]==6 && next_use[2]==7 && next_use[3]==8 && next_use[4]==std::numeric_limits<size_t>::max() && next_use[5]==std::numeric_limits<size_t>::max() && next_use[6]==std::numeric_limits<size_t>::max()&& next_use[7]==std::numeric_limits<size_t>::max() && next_use[8]==std::numeric_limits<size_t>::max()) {
        std::cerr << "OPT Next Use Test: Passed!" << "\n";
    } else {
        throw std::runtime_error("ERROR: OPT Next Use Test Failed!");
    } 

} 



