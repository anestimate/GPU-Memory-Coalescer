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


void test_secondary_miss(const config& data) {
    config test_data = data;
    Cache cacheInstance(test_data);
    std::vector<uint32_t> address;  

    for (int i=0;i<16;i++) {
        address.push_back(i);
    }

    for (int i=0;i<16;i++) {
        cacheInstance.access(test_data, address[i]);
    }

    cacheInstance.drain(test_data);

    output_stats(test_data,cacheInstance, statsService);

    if ((cacheInstance.requests == 16) && (cacheInstance.hits == 0) && (cacheInstance.misses == 16) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 1) && (statsService.bundles == 0) && (statsService.lanes == 0)&& (statsService.transactions == 0) && (statsService.current_cycle == 100) && (statsService.secondary_miss == 15) && (statsService.merges_per_mshr.size()==1) && (statsService.merges_per_mshr[0]==15)) {
        std::cerr << "SECONDARY MISS TEST: PASS" << "\n";
    } else {
        std::cerr << "SECONDARY MISS TEST: FAIL" << "\n";
    } 

} 


void test_stall(const config& data) {

    config test_data = data;
    test_data.num_mshr = 2;
    Cache cacheInstance(test_data);
    std::vector<uint32_t> address;  

    
    address.push_back(0);
    address.push_back(64);
    address.push_back(128);
    

    for (int i=0;i<3;i++) {
        cacheInstance.access(test_data, address[i]);
    }

    //output_stats(test_data,cacheInstance, statsService);

    if ((cacheInstance.requests == 3) && (cacheInstance.hits == 0) && (cacheInstance.misses == 3) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 3) && (statsService.bundles == 0) && (statsService.lanes == 0)&& (statsService.transactions == 0) && (statsService.current_cycle == 101) && (statsService.secondary_miss == 0)  && (statsService.mshr_stall_cycles == 98)) {
        std::cerr << "STALL TEST: PASS" << "\n";
    } else {
        std::cerr << "STALL TEST: FAIL" << "\n";
        //std::cerr << statsService.mshr_stall_cycles << "\n";
    } 

} 


bool test_saturationP1(const config& data) {
    statsService = Stats{};
    config test_data = data;
    test_data.num_mshr = 1;
    Cache cacheInstance(test_data);
    std::vector<uint32_t> address;  

    
    for (int i=0;i<16;i++) {
        address.push_back(i*64);
    }

    for (int i=0;i<16;i++) {
        cacheInstance.access(test_data, address[i]);
    }
    cacheInstance.drain(test_data);
    //output_stats(test_data,cacheInstance, statsService);

    if ((statsService.current_cycle == 1600) && (statsService.max_mshr_occupancy ==1)) {
        return true;
    } else {
        return false;
    } 

} 

bool test_saturationP2(const config& data) {
    statsService = Stats{};
    config test_data = data;
    test_data.num_mshr = 8;
    Cache cacheInstance(test_data);
    std::vector<uint32_t> address;  

    
    for (int i=0;i<16;i++) {
        address.push_back(i*64);
    }

    for (int i=0;i<16;i++) {
        cacheInstance.access(test_data, address[i]);
    }
    cacheInstance.drain(test_data);
    //output_stats(test_data,cacheInstance, statsService);

    if ((statsService.current_cycle == 207) && (statsService.max_mshr_occupancy ==8)) {
        return true;
    } else {
        return false;
    } 

} 


bool test_saturationP3(const config& data) {
    statsService = Stats{};
    config test_data = data;
    test_data.num_mshr = 16;
    Cache cacheInstance(test_data);
    std::vector<uint32_t> address;  

    
    for (int i=0;i<16;i++) {
        address.push_back(i*64);
    }

    for (int i=0;i<16;i++) {
        cacheInstance.access(test_data, address[i]);
    }
    cacheInstance.drain(test_data);
    //output_stats(test_data,cacheInstance, statsService);

    if ((statsService.current_cycle == 115) && (statsService.max_mshr_occupancy ==16)) {
        return true;
    } else {
        return false;
    } 

} 

void test_saturation(const config& data) {

    bool out1 = test_saturationP1(data);
    bool out2 = test_saturationP2(data);
    bool out3 = test_saturationP3(data);

    if (out1 &out2&out3){
        std::cerr << "Saturation Test: Pass" << "\n";
    } else {
        std::cerr << "Saturation Test: Fail" << "\n";
    }


} 