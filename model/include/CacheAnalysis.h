#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <set>
#include <cmath>
#include <array>
#include <bitset>
#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "ReplacementPolicy.h"
#include "Stats.h"
#include "MSHR.h"

struct AnalysisEntry {

    uint64_t line = 0;
    bool valid = false;

};

class CacheAnalysis {

    public:
    std::vector<AnalysisEntry> entries;

    LRU LRUInstance;
    
    CacheAnalysis(const config& data) : entries(data.cache_bytes/data.line_bytes), LRUInstance(data) {}

    bool lookup(const config& data, int address) {
        
        int set = get_index(address,data);
        uint64_t line = get_line_addr(address,data);

        for (int i=0;i<data.ways;i++) {

            if ((entries[set*data.ways+i].valid == true) && entries[set*data.ways+i].line == line) {
                LRUInstance.update(data, set,i);
                return true;
            }
            
        }

        return false;
        
    }


    bool fill(const config& data, int address) {

        int set = get_index(address,data);
        int line = get_line_addr(address,data);

        for (int i = 0;i<data.ways; i++) {
            if (entries[set*data.ways+i].valid == false) {
                  entries[set*data.ways+i].line = line;
                  entries[set*data.ways+i].valid = true;
                  LRUInstance.update(data, set,i);
                  return true;
            } 
        }
        
        int replace = LRUInstance.replace(data, set);
        entries[set*data.ways + replace].line = line;
        entries[set*data.ways + replace].valid = true;

        return false;
    }

    int run(const config& data, const std::vector<uint32_t>& transactions) {
        
        int misses = 0;
        
        for (size_t i=0;i<transactions.size();i++) {
            if (!(lookup(data, transactions[i]))) {
                misses++;
                fill(data, transactions[i]);
            }
        }

        return misses;

    }

};