#pragma once
#include "tb_util.h"
#include "WorkLoadGenerator.h"
#include <iostream>

bool test_compulsory(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i=0;i<32768;i=i+64) {
        address.push_back(i);
    }

    for (int i=0;i<512;i++) {
        access(dut,tfp,address[i]);
    }

    if ((dut->n_requests == 512) && (dut->n_hits == 0) && (dut->n_misses == 512) && (dut->n_evictions == 384) && (dut->n_mem_reqs == 512) && (dut->total_bytes == 32768)) {
        std::cerr << "COMPULSORY TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
    } else {
        std::cerr << "COMPULSORY TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

} 


bool test_temporal(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 
    
    address.push_back(1);
    
    for (int i=0;i<100;i++) {
        access(dut,tfp,address[0]);
    }

    if ((dut->n_requests == 100) && (dut->n_hits == 99) && (dut->n_misses == 1) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 1) && (dut->total_bytes == 64)) {
        std::cerr << "TEMPORAL TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
    } else {
        std::cerr << "TEMPORAL TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

} 


bool test_spatial(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i=0;i< 1024;i=i+4) {
        address.push_back(i);
    }

    for (int i=0;i<256;i++) {
        access(dut,tfp,address[i]);
    }

    if ((dut->n_requests == 256) && (dut->n_hits == 240) && (dut->n_misses == 16) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 16) && (dut->total_bytes == 1024)) {
        std::cerr << "SPATIAL TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
    } else {
        std::cerr << "SPATIAL TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

} 

bool test_capacity(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i=0;i<16384;i=i+64) {
        address.push_back(i);
    }

    for (int i=0;i<256;i++) {
        access(dut,tfp,address[i]);
    }

    for (int i=0;i<256;i++) {
        access(dut,tfp,address[i]);
    }

    if ((dut->n_requests == 512) && (dut->n_hits == 0) && (dut->n_misses == 512) && (dut->n_evictions == 384) && (dut->n_mem_reqs == 512) && (dut->total_bytes == 32768)) {
        std::cerr << "CAPACITY TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
        
    } else {
        std::cerr << "CAPACITY TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

} 


bool test_conflict(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    address.push_back(0);
    address.push_back(2048);
  

    for (int i=0;i<100;i++) {

        if (i%2 == 0 ) {
            access(dut,tfp,address[0]);
        } else {
            access(dut,tfp,address[1]);
        }
            
    }

    if ((dut->n_requests == 100) && (dut->n_hits == 98 ) && (dut->n_misses == 2) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 2) && (dut->total_bytes == 128)) {
        std::cerr << "CONFLICT TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
    } else {
        std::cerr << "CONFLICT TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

}


bool test_boundary(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    address.push_back(0);
    address.push_back(2047);
    address.push_back(2048);
    address.push_back(4294967232);
    address.push_back(4294967295);

    for (int i=0;i<5;i++) {
        access(dut,tfp,address[i]);
    }

    if ((dut->n_requests == 5) && (dut->n_hits == 1) && (dut->n_misses == 4) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 4) && (dut->total_bytes ==256)) {
        std::cerr << "BOUNDARY TEST: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);
        return true;
    } else {
        std::cerr << "BOUNDARY TEST: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
        return false;
    }

}

void test_fourway_replacement(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i=0;i<8193;i=i+2048) {
        address.push_back(i);
    }

    for (int i=0;i<5;i++) {
        access(dut,tfp,address[i]);
    }

    output_stats(dut);
    rst(dut,tfp);

}



void test_fourways_access(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i=0;i<8192;i=i+2048) {
        address.push_back(i);
    }

    for (int i=0;i<4;i++) {
        access(dut,tfp,address[i]);
    }

    for (int i=0;i<4;i++) {
        access(dut,tfp,address[i]);
    }


    output_stats(dut);
    rst(dut,tfp);

}


void test_evict_precise(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    access(dut,tfp,448);
    access(dut,tfp,2496);
    access(dut,tfp,4544);
    access(dut,tfp,6592);
    access(dut,tfp,448);
    access(dut,tfp,8640);


    output_stats(dut);
    rst(dut,tfp);

}


void test_evict_all_sets(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 

    for (int i = 0; i<32; i++) {
        for (int j = 0; j<5; j++) {
            access(dut,tfp, 64*i +2048*j);
        }
    }


    output_stats(dut);
    rst(dut,tfp);

}




void test_mixed_locality(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    for (int i = 0; i<32; i++) {
        access(dut,tfp,0);
        access(dut,tfp, 2048*(i+1));
    }

    output_stats(dut);
    rst(dut,tfp);

}


void bundle_test_ONE(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 
    uint32_t active_mask = 4294967295;

    
    for (int i = 0; i<32; i++) {
        address.push_back(4395);
    }
        
    access_bundle(dut,tfp,address, active_mask);

    if ((dut->n_requests == 1) && (dut->n_hits == 0) && (dut->n_misses == 1) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 1) && (dut->total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 1: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);

    } else {
        std::cerr << "BUNDLE TEST 1: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
    } 

} 


void bundle_test_TWO(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 
    uint32_t active_mask = 4294967295;

    
    for (int i = 0; i<129; i=i+4) {
        address.push_back(i);
    }
        
    access_bundle(dut,tfp,address, active_mask);

    if ((dut->n_requests == 2) && (dut->n_hits == 0) && (dut->n_misses == 2) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 2) && (dut->total_bytes == 128)) {
        std::cerr << "BUNDLE TEST 2: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);

    } else {
        std::cerr << "BUNDLE TEST 2: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
    } 

} 

void bundle_test_THREE(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 
    uint32_t active_mask = 4294967295;

    
    for (int i = 0; i<129; i=i+4) {
        address.push_back(i+4);
    }
        
    access_bundle(dut,tfp,address, active_mask);

    if ((dut->n_requests == 3) && (dut->n_hits == 0) && (dut->n_misses == 3) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 3) && (dut->total_bytes == 192)) {
        std::cerr << "BUNDLE TEST 3: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);

    } else {
        std::cerr << "BUNDLE TEST 3: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
    } 

}    

void bundle_test_FOUR(const config& data,Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    std::vector<uint32_t> address; 
    uint32_t active_mask = 4294967295;

    for (int i = 0; i<32; i++) {
        address.push_back(4395);
    }
        
    access_bundle(dut,tfp,address, active_mask);

    if ((dut->n_requests == 1) && (dut->n_hits == 0) && (dut->n_misses == 1) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 1) && (dut->total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 4, PHASE 1: PASS" << "\n";
        output_json(dut,data);
    } else {
        std::cerr << "BUNDLE TEST 4, PHASE 1: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
    } 

    access_bundle(dut,tfp,address, active_mask);

    if ((dut->n_requests == 2) && (dut->n_hits == 1) && (dut->n_misses == 1) && (dut->n_evictions == 0) && (dut->n_mem_reqs == 1) && (dut->total_bytes == 64)) {
        std::cerr << "BUNDLE TEST 4, PHASE 2: PASS" << "\n";
        output_json(dut,data);
        rst(dut,tfp);

    } else {
        std::cerr << "BUNDLE TEST 4, PHASE 2: FAIL" << "\n";
        output_json(dut,data);
        output_stats(dut);
        rst(dut,tfp);
    } 

} 

void workload_broadcast(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count) {

    std::vector<Bundle> address = generate_broadcast(data, bundle_count);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);

}

void workload_coalesced_aligned(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count) {

    std::vector<Bundle> address = coalesced_aligned(data, bundle_count);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);
    //output_json(dut,data);

}

void workload_coalesced_misaligned(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count, int offset) {

    std::vector<Bundle> address = coalesced_misaligned(data, bundle_count, offset);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);
    //output_json(dut,data);

} 


void workload_strided_k(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count, int stride) {

    std::vector<Bundle> address = strided_k(data, bundle_count, stride);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);
    //output_json(dut,data);

} 

void workload_reverse(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count) {

    std::vector<Bundle> address = reverse(data, bundle_count);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);
    //output_json(dut,data);

}

void workload_transpose(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int rows, int coloums) {
    
    std::vector<Bundle> address = transpose(data, rows, coloums);
        
    for (int i=0;i<address.size();i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);

}


void workload_tiled(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int rows, int coloums) {

    std::vector<Bundle> address = tiled(data, rows, coloums);
        
    for (int i=0;i<address.size();i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);

}

void workload_random_uniform(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count, int seed, int region_elements) {

    std::vector<Bundle> address = random_uniform(data, bundle_count,seed,region_elements);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    rst(dut,tfp);
    //output_json(dut,data);

}

void workload_gather(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count, int seed, int region_elements, int local_region_elements, double locality) {

    std::vector<Bundle> address = gather(data, bundle_count, seed, region_elements, local_region_elements, locality);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    rst(dut,tfp);
    //output_json(dut,data);

}

void workload_hotspot(const config& data, Vgpu_mem_top* dut, VerilatedVcdC* tfp, int bundle_count, int seed, int region_elements) {

    std::vector<Bundle> address = hotspot(data, bundle_count, seed, region_elements);
        
    for (int i=0;i<bundle_count;i++) {
        access_bundle(dut,tfp,{address[i].addr.begin(),address[i].addr.end()}, address[i].active_mask.to_ulong());
    }

    output_json(dut,data);
    output_stats(dut);
    rst(dut,tfp);
    //output_json(dut,data);

}