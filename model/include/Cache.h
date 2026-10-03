#pragma once

#include <array>
#include <cstdint>
#include <bitset>
#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "ReplacementPolicy.h"
#include <iostream>
#include <set>
#include <cmath>
#include "Stats.h"
#include "MSHR.h"
#include <vector>


class Cache {

    public:
    std::vector<uint32_t> line;
    std::vector<uint32_t> valid;
    std::array<uint32_t, 2048> fill_set{};
    std::array<uint32_t, 2048> fill_way{};
    std::array<uint32_t, 2048> fill_tag{};
    int index_counter = 0;
    int victim_tag = 0;
    int evictions = 0;
    int requests = 0;
    int hits = 0;
    int misses = 0;
    int mem_reqs = 0;
    int total_bytes = 0;
    int way = 0;
    
    LRU LRUinstance;
    MSHR_file MSHRInstance;

    Cache(const config& data) : line(data.num_sets*data.ways,0), valid(data.num_sets*data.ways,0), LRUinstance(data), MSHRInstance(data) {}

    void access(const config& data, int address) {

        while (true) {

            auto completed = MSHRInstance.operate_find_completed();

            if (!completed.has_value()) {
                break;
            }

            fill(data, completed.value());
        }
    
        if (lookup(data, address) == 0) {
            auto reqs_before = statsService.mem_reqs;
            uint32_t line_addr = address & ~(uint32_t)(data.line_bytes-1);
            auto completed = MSHRInstance.handle_miss(line_addr, statsService, data);
            int new_reqs = statsService.mem_reqs - reqs_before;
            mem_reqs += new_reqs;
            total_bytes += new_reqs*data.line_bytes;
            if (completed.has_value()) {
                fill(data, completed.value());
            }
            if (data.num_mshr == 1) {
                fill(data, MSHRInstance.retire_earliest(statsService));
            }
            statsService.current_cycle += 5;
        } else {
            statsService.current_cycle++;
        }

        requests++;
        statsService.requests++;
        statsService.mshr_occupancy_sum +=MSHRInstance.occupancy();
        statsService.mshr_occupancy_samples++;
    }

    int lookup(const config& data, int address) {
        
        int set = get_index(address,data);
        uint32_t tag = get_tag(address,data);

        for (int i=0;i<data.ways;i++) {

            if ((line[set*data.ways+i] == tag) && valid[set*data.ways+i] ==1) {
                hits++;
                statsService.hits++;
                LRUinstance.update(data, set,i);
                return 1;
            }
            
        }


        misses++;
        statsService.misses++;
        return 0;
        
    }

    void fill(const config& data, int address) {

        int set = get_index(address,data);
        int tag = get_tag(address,data);
        bool filled = false;


        for (int i = 0;i<data.ways; i++) {
            if (valid[set*data.ways+i] == 0) {
                  line[set*data.ways+i] = tag;
                  filled = true;
                  way = i;
                  LRUinstance.update(data, set,i);
                  valid[set*data.ways+i] = 1;
                  break;
            } 
        }
        
        if (filled == false) {
            int replace = LRUinstance.replace(data, set);
            way = replace;
            victim_tag = line[set*data.ways + replace];
            line[set*data.ways + replace] = tag;
            evictions++;
            statsService.evictions++;
        }

        if (index_counter < 2048) {
            fill_set[index_counter] = set;
            fill_way[index_counter] = way;
            fill_tag[index_counter] = tag;
        }

        index_counter++;
        
    }

    void drain(const config& data) {
        while (MSHRInstance.find_ready_cycle()!=-1) {
            fill(data, MSHRInstance.retire_earliest(statsService));
        }
    }

};

