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


bool test_compulsory(const config& data) {
    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;  

    for (int i=0;i<32768;i=i+64) {
        address.push_back(i);
    }

    for (int i=0;i<512;i++) {
        cacheInstance.access(d, address[i]);
    }

    cacheInstance.drain(d);
    //output_json(cacheInstance);

    if ((cacheInstance.requests == 512) && (cacheInstance.hits == 0) && (cacheInstance.misses == 512) && (cacheInstance.evictions == 384) && (cacheInstance.mem_reqs == 512) && (cacheInstance.total_bytes == 32768)) {
        std::cerr << "COMPULSORY TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "COMPULSORY TEST: FAIL" << "\n";

        return false;
    }

} 


bool test_temporal(const config& data) {

    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;  
    
    address.push_back(1);
    
    for (int i=0;i<100;i++) {
        cacheInstance.access(d,address[0]);
    }

    cacheInstance.drain(d);
    
    if ((cacheInstance.requests == 100) && (cacheInstance.hits == 99) && (cacheInstance.misses == 1) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 1) && (cacheInstance.total_bytes == 64)) {
        std::cerr << "TEMPORAL TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "TEMPORAL TEST: FAIL" << "\n";
        return false;
    }

} 


bool test_spatial(const config& data) {

    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;  

    for (int i=0;i< 1024;i=i+4) {
        address.push_back(i);
    }

    for (int i=0;i<256;i++) {
        cacheInstance.access(d,address[i]);
    }

    cacheInstance.drain(d);

    if ((cacheInstance.requests == 256) && (cacheInstance.hits == 240) && (cacheInstance.misses == 16) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 16) && (cacheInstance.total_bytes == 1024)) {
        std::cerr << "SPATIAL TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "SPATIAL TEST: FAIL" << "\n";
        return false;
    }

} 

bool test_capacity(const config& data) {

    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;   

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);
    }

    for (int i=0;i<256;i++) {
        cacheInstance.access(d,address[i]);
    }
    cacheInstance.drain(d);

    for (int i=0;i<256;i++) {
        cacheInstance.access(d,address[i]);
    }
    cacheInstance.drain(d);

    if ((cacheInstance.requests == 512) && (cacheInstance.hits == 0) && (cacheInstance.misses == 512) && (cacheInstance.evictions == 384) && (cacheInstance.mem_reqs == 512) && (cacheInstance.total_bytes == 32768)) {
        std::cerr << "CAPACITY TEST: PASS" << "\n";
        return true;
        
    } else {
        std::cerr << "CAPACITY TEST: FAIL" << "\n";
        return false;
    }

} 


bool test_conflict(const config& data) {

    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;   

    address.push_back(0);
    address.push_back(2048);
  

    for (int i=0;i<100;i++) {

        if (i%2 == 0 ) {
            cacheInstance.access(d,address[0]);
        } else {
            cacheInstance.access(d,address[1]);
        }
            
    }

    cacheInstance.drain(d);

    if ((cacheInstance.requests == 100) && (cacheInstance.hits == 98 ) && (cacheInstance.misses == 2) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 2) && (cacheInstance.total_bytes == 128)) {
        std::cerr << "CONFLICT TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "CONFLICT TEST: FAIL" << "\n";
        return false;
    }

}


bool test_boundary(const config& data) {

    config d = data;
    d.mem_latency = 0;
    Cache cacheInstance(d);
    Bundle dataBundle;
    std::vector<uint32_t> address;   

    address.push_back(0);
    address.push_back(2047);
    address.push_back(2048);
    address.push_back(4294967232);
    address.push_back(4294967295);

    for (int i=0;i<5;i++) {
        cacheInstance.access(d,address[i]);
    }

    cacheInstance.drain(d);

    if ((cacheInstance.requests == 5) && (cacheInstance.hits == 1) && (cacheInstance.misses == 4) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 4) && (cacheInstance.total_bytes ==256)) {
        std::cerr << "BOUNDARY TEST: PASS" << "\n";
        return true;
    } else {
        std::cerr << "BOUNDARY TEST: FAIL" << "\n";
        return false;
    }

}

void test_fourway_replacement(const config& data) {

    Cache cacheInstance(data);
    Bundle dataBundle;
    std::vector<uint32_t> address; 

    for (int i=0;i<8193;i=i+2048) {
        address.push_back(i);
    }

    for (int i=0;i<5;i++) {
        cacheInstance.access(data,address[i]);
    }

    std::cerr << "Replacement Test" << "\n";
    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";

}



void test_fourways_access(const config& data) {

    Cache cacheInstance(data);
    Bundle dataBundle;
    std::vector<uint32_t> address;  

    for (int i=0;i<8192;i=i+2048) {
        address.push_back(i);
    }

    for (int i=0;i<4;i++) {
        cacheInstance.access(data,address[i]);
    }

    for (int i=0;i<4;i++) {
        cacheInstance.access(data,address[i]);
    }
    std::cerr << "Access Test" << "\n";
    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";

}


void test_evict_precise(const config& data) {

    Cache cacheInstance(data);
    Bundle dataBundle;

    cacheInstance.access(data,448);
    cacheInstance.access(data,2496);
    cacheInstance.access(data,4544);
    cacheInstance.access(data,6592);
    cacheInstance.access(data,448);
    cacheInstance.access(data,8640);


    std::cerr << "Precise Eviction Test" << "\n";
    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";

}


void test_evict_all(const config& data) {

    Cache cacheInstance(data);
    Bundle dataBundle;
    std::vector<uint32_t> address;

    for (int i = 0; i<32; i++) {
        for (int j = 0; j<5; j++) {
            cacheInstance.access(data, 64*i +2048*j);
        }
    }

    std::cerr << "Evict-All Test" << "\n";
    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";
}



void test_mixed_locality(const config& data) {

    Cache cacheInstance(data);
    Bundle dataBundle;

    for (int i = 0; i<32; i++) {
        cacheInstance.access(data,0);
        cacheInstance.access(data, 2048*(i+1));
    }

    std::cerr << "Mixed Locality Test" << "\n";
    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";


}



void bundle_test_ONE(const config& data, Bundle& bundleData) {

    Cache cacheInstance(data);
    Bundle dataBundle;

    bundleData.active_mask = 4294967295;

    
    for (int i = 0; i<32; i++) {
        bundleData.addr[i] = 4395;
    }
    
    multiple_requests(data,bundleData, cacheInstance);


    if ((cacheInstance.requests == 1) && (cacheInstance.hits == 0) && (cacheInstance.misses == 1) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 1) && (cacheInstance.total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 1: PASS" << "\n";
        output_json(data, cacheInstance, statsService);

    } else {
        std::cerr << "BUNDLE TEST 1: FAIL" << "\n";
    } 

} 


void bundle_test_TWO(const config& data, Bundle& bundleData) {

    Cache cacheInstance(data);
    Bundle dataBundle;

    bundleData.active_mask = 4294967295;


    for (int i = 0; i<32; i++) {
        bundleData.addr[i] = i*4;

    }
        
    multiple_requests(data,bundleData, cacheInstance);

    std::cerr << " " << "\n";
    std::cerr << "Number of Requests: " << cacheInstance.requests <<"\n";
    std::cerr << "Number of Hits: "<< cacheInstance.hits << "\n";
    std::cerr << "Number of Misses: " << cacheInstance.misses <<"\n";
    std::cerr<<  "Number of Evictions: " << cacheInstance.evictions << "\n";
    std::cerr << "Number of Memory Requests: " << cacheInstance.mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< cacheInstance.total_bytes <<"\n";
    std::cerr << " " << "\n";

    if ((cacheInstance.requests == 2) && (cacheInstance.hits == 0) && (cacheInstance.misses == 2) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 2) && (cacheInstance.total_bytes == 128)) {
        std::cerr << "BUNDLE TEST 2: PASS" << "\n";
        output_json(data, cacheInstance, statsService);

    } else {
        std::cerr << "BUNDLE TEST 2: FAIL" << "\n";
    } 

} 

void bundle_test_THREE(const config& data, Bundle& bundleData) {

    Cache cacheInstance(data);
    Bundle dataBundle;

    bundleData.active_mask = 4294967295;
    int counter = 0;

    
    for (int i = 0; i<128; i=i+4) {
        bundleData.addr[counter] = i+4;
        counter++;
    }
        
    multiple_requests(data,bundleData, cacheInstance);

    if ((cacheInstance.requests == 3) && (cacheInstance.hits == 0) && (cacheInstance.misses == 3) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 3) && (cacheInstance.total_bytes == 192)) {
        std::cerr << "BUNDLE TEST 3: PASS" << "\n";
        output_json(data, cacheInstance, statsService);
    } else {
        std::cerr << "BUNDLE TEST 3: FAIL" << "\n";
    } 

}    

void bundle_test_FOUR(const config& data, Bundle& bundleData) {
    statsService = Stats{};
    Cache cacheInstance(data);
    Bundle dataBundle;
    bundleData.active_mask = 4294967295;

    for (int i = 0; i<32; i++) {
       bundleData.addr[i] = 4395;
    }
        
    multiple_requests(data,bundleData, cacheInstance);

    if ((cacheInstance.requests == 1) && (cacheInstance.hits == 0) && (cacheInstance.misses == 1) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 1) && (cacheInstance.total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 4, PHASE 1: PASS" << "\n";
        output_json(data, cacheInstance, statsService);
    } else {
        std::cerr << "BUNDLE TEST 4, PHASE 1: FAIL" << "\n";
    } 

    statsService.current_cycle += data.mem_latency;
    bundleData.active_mask = 4294967295;
    multiple_requests(data,bundleData, cacheInstance);

    if ((cacheInstance.requests == 2) && (cacheInstance.hits == 1) && (cacheInstance.misses == 1) && (cacheInstance.evictions == 0) && (cacheInstance.mem_reqs == 1) && (cacheInstance.total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 4, PHASE 2: PASS" << "\n";
        output_json(data, cacheInstance, statsService);

    } else {
        std::cerr << "BUNDLE TEST 4, PHASE 2: FAIL" << "\n";
    } 

} 

void bundle_test_throughput(const config& data, Bundle& bundleData) {
    
    Cache cacheInstance(data);
    bundleData.active_mask = 4294967295;

    for (int i = 0; i<32; i++) {
       bundleData.addr[i] = 4395;
    }
    
    for (int i = 0;i<1000000;i++) {
        multiple_requests(data,bundleData, cacheInstance);
        bundleData.active_mask = 4294967295;
    }
}