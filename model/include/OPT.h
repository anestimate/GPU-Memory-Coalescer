#pragma once

#include <vector>
#include <limits>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include "Config.h"


struct OPT_entry {
    uint32_t line =0;
    size_t next_use=0;
    bool valid=false;
};

struct OPT_LRU_data {
    int LRU_misses =0;
    int OPT_misses=0;
    double ratio=0.0;
};

class OPT {
    public:
        std::vector<OPT_entry> entries;

        OPT(const config& data) 
            : entries(data.cache_bytes/data.line_bytes) {
        }


        std::vector<size_t> get_next_use(const config&data, const std::vector<uint32_t>& transactions) {

            const size_t NONE = std::numeric_limits<size_t>::max();
            std::vector<size_t> next_use(transactions.size(), NONE);
            std::unordered_map<uint32_t, size_t> last_seen;

            for (size_t i = transactions.size();i-->0;) {
                
                uint32_t line = get_line_addr(transactions[i],data);

                auto it = last_seen.find(line);

                if (it != last_seen.end()){
                    next_use[i] = it->second;
                }

                last_seen[line] = i;
            }

            return next_use;

        }

    

    bool lookup(const config& data, uint32_t address, size_t next_use) {
        
        int set = get_index(address,data);
        uint32_t line = get_line_addr(address,data);

        for (int i=0;i<data.ways;i++) {

            if (entries[set*data.ways+i].valid == true && entries[set*data.ways+i].line == line) {
                entries[set*data.ways+i].next_use = next_use;
                return true;
            }
            
        }

        return false;
        
        
    }

    bool fill_empty(const config& data, uint32_t address, size_t next_use) {

        int set = get_index(address,data);
        int line = get_line_addr(address,data);        
        
        for (int i=0;i<data.ways;i++) {

            if (!entries[set*data.ways+i].valid) {
                entries[set*data.ways+i].next_use = next_use;
                entries[set*data.ways+i].line = line;
                entries[set*data.ways+i].valid = true;
                return true;
            }
            
        }

        return false;
    }

    void replace(const config& data, uint32_t address, size_t next_use) {

        int set = get_index(address,data);
        size_t value_way = 0;
        int index_position =0;
        int line = get_line_addr(address,data); 

        for (int i=0;i<data.ways;i++) {
            if (entries[set*data.ways+i].next_use > value_way) {
                value_way = entries[set*data.ways+i].next_use;
                index_position = i;
            }
        }

        entries[set*data.ways+index_position].next_use = next_use;
        entries[set*data.ways+index_position].line = line;
        entries[set*data.ways+index_position].valid = true;

    }

    int run(const config& data, const std::vector<uint32_t>& transactions) {

        std::vector<size_t> next_use = get_next_use(data, transactions);
        int hit = 0;
        int miss = 0;

        for (size_t i=0;i<transactions.size();i++) {

            uint32_t address = transactions[i];
            size_t future = next_use[i];

            if (lookup(data, address, future)) {
                hit++;
            } else {
                miss++;

                if (!fill_empty(data,address,next_use[i])) {
                    replace(data,address,future);
                }

            }

        }
         
        return miss;

    }

    int unique_cache_lines(const config& data, const std::vector<uint32_t>& transactions) {
        
        std::unordered_set<uint32_t> unique;

        for (uint32_t t: transactions) {
            unique.insert(get_line_addr(t,data));
        }

        return unique.size();
    }


};


