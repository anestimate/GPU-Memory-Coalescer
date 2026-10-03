#pragma once

#include <array>
#include <cstdint>
#include <bitset>
#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "Cache.h"
#include "ReplacementPolicy.h"
#include <iostream>
#include <set>
#include <cmath>
#include "Stats.h"
#include <vector>
#include <optional>



struct MSHREntry {
    uint32_t line_addr;
    bool valid;
    uint64_t ready_cycle;
    int merged_count;
};


class MSHR_file{

    public:
    std::vector<MSHREntry> entries;

    MSHR_file(const config& data)
        : entries(data.num_mshr) {

    }

    int find(uint32_t line_addr) {

        for (size_t i=0; i< entries.size();i++) {
            if (entries[i].line_addr == line_addr && entries[i].valid) {
                return i;
            }
        }
        
        return -1;
    }

    int find_free() {

        for (size_t i=0; i<entries.size();i++) {
            if (entries[i].valid == 0) {
                return i;
            }
        }

        return -1;
    }

    int allocate(uint32_t line_addr, Stats& statsService, const config& data) {

        int position_allocation = find_free();
        int in_process = find(line_addr);
        uint64_t temp_occupancy = 0;

        if (in_process != -1 || position_allocation == -1) {
            return -1;
        }
        
        entries[position_allocation].valid = true;

        for (size_t i =0;i<entries.size();i++) {
            if (entries[i].valid){
                temp_occupancy++;
            }
        }

        if (temp_occupancy>statsService.max_mshr_occupancy) {
            statsService.max_mshr_occupancy = temp_occupancy;
        }

        entries[position_allocation].line_addr = line_addr;
        entries[position_allocation].ready_cycle = statsService.current_cycle + data.mem_latency;
        entries[position_allocation].merged_count = 0;
        statsService.mem_reqs++;
        statsService.mem_bytes += data.line_bytes;
        
        return position_allocation;
    }

    int find_ready_cycle() {

        int smallest_index = -1;
        uint64_t smallest_entry =0;

        for (size_t i=0; i< entries.size();i++) {
            if (entries[i].valid) {
                smallest_entry = entries[i].ready_cycle;
                smallest_index = i;
                break;
            } 
            
        }

        for (size_t i=0; i< entries.size();i++) {
            if (entries[i].ready_cycle < smallest_entry && entries[i].valid == true ) {
                smallest_entry = entries[i].ready_cycle;
                smallest_index = i;
            }
            
        }

        return smallest_index;

    }

    void free_entry(int position_allocation) {

        entries[position_allocation].valid = false;
        entries[position_allocation].line_addr = 0;
        entries[position_allocation].ready_cycle = 0;
        entries[position_allocation].merged_count = 0;

    }

    int retire_earliest(Stats& statsService) {

        int free_index = find_ready_cycle();
        uint32_t completed_address = entries[free_index].line_addr;

        statsService.mshr_stall_cycles += entries[free_index].ready_cycle - statsService.current_cycle;
        statsService.merges_per_mshr.push_back(entries[free_index].merged_count);
        statsService.current_cycle = entries[free_index].ready_cycle;
        free_entry(free_index);

        return completed_address;
    }

    void merge(int index_increment) {

        entries[index_increment].merged_count++;

    }

    std::optional<uint32_t> handle_miss(uint32_t line_addr, Stats& statsService, const config& data) {

        int exist = find(line_addr);
        int attempt_allocate = 0;

        if (!(exist==-1)){
            merge(exist);
            statsService.secondary_miss++;
        } else {
            attempt_allocate = allocate(line_addr, statsService, data);
        }

        if (attempt_allocate == -1) {
            uint32_t completed_address = retire_earliest(statsService);
            allocate(line_addr, statsService, data);
            statsService.current_cycle++;
            return completed_address;
        }

        statsService.current_cycle++;
        return std::nullopt;

    }

    int find_completed() {

        for (size_t i=0; i< entries.size();i++) {
            if (entries[i].ready_cycle<= statsService.current_cycle && entries[i].valid) {
                return i;
            }
        }

        return -1;
    
    }

    std::optional<uint32_t> operate_find_completed() {

        int index =  find_completed();

        if (index == -1) {
            return std::nullopt;
        }

        uint32_t completed_address = entries[index].line_addr;
        statsService.merges_per_mshr.push_back(entries[index].merged_count);
        free_entry(index);

        return completed_address;
    }

    int occupancy() {
        uint64_t temp_occupancy = 0;

        for (size_t i =0;i<entries.size();i++) {
            if (entries[i].valid){
                temp_occupancy++;
            }
        }

        return temp_occupancy;

    }

};
