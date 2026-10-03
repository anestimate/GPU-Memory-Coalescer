#pragma once
#include <cstdint>
#include <vector>

struct Stats {

    uint64_t bundles = 0;
    uint64_t lanes = 0;
    uint64_t transactions= 0 ;
    uint64_t requests = 0;
    uint64_t hits = 0;
    uint64_t misses = 0;
    uint64_t evictions = 0;
    uint64_t mem_reqs= 0;
    uint64_t mem_bytes = 0;
    uint64_t transactions_per_bundle[33] {};
    uint64_t current_cycle = 0;
    uint64_t secondary_miss = 0;
    uint64_t max_mshr_occupancy =0;
    uint64_t mshr_stall_cycles = 0;
    uint64_t mshr_occupancy_sum =0;
    uint64_t mshr_occupancy_samples = 0;
    std::vector <uint64_t> merges_per_mshr;
    std::vector<uint32_t> transaction_log;


};

inline Stats statsService;