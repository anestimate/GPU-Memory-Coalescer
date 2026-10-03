#include <stdexcept>
#include <cstdint>
#pragma once

int log2i(int value) {

    int cycles = 0;

    while (value > 1) {
        value = value >> 1;
        cycles++;
    }

    return cycles;
}


struct config {

    int num_lanes;
    int addr_bits;
    int line_bytes;
    int cache_bytes;
    int ways;
    int num_mshr;
    int mem_latency;
    int access_bytes;

    int num_sets;
    int offset_bits;
    int index_bits;
    int tag_bits;
    int way_bits;

    void finalise() {

        offset_bits = log2i(line_bytes);
        num_sets = cache_bytes / line_bytes /ways;
        index_bits = log2i(num_sets);
        tag_bits = addr_bits - index_bits - offset_bits;

        if (ways>1) {
            way_bits = log2i(ways);
        } else {
            way_bits = 1;
        }

        if (cache_bytes != num_sets*ways*line_bytes) {
            throw std::runtime_error("LOGIC ERROR IN CACHE BYTES"); 
        }
        if (tag_bits+index_bits+offset_bits != addr_bits) {
            throw std::runtime_error("LOGIC ERROR IN ADDR BITS"); 
        }

        if ((line_bytes & (line_bytes-1)) != 0) {
            throw std::runtime_error("LOGIC ERROR IN LINE BYTES"); 
        }

        if ((cache_bytes & (cache_bytes-1)) != 0) {
            throw std::runtime_error("LOGIC ERROR IN CACHE BYTES"); 
        }
    }

};

int get_tag (uint32_t value, const config& data) {
    return value >> (data.offset_bits+data.index_bits);
}

int get_index (uint32_t value, const config& data) { 
    return (value>>data.offset_bits) & ((1u<<data.index_bits)-1);
}

int get_offset (uint32_t value, const config& data) { 
    return value & ((1u<<data.offset_bits)-1);
}

int get_line_addr (uint32_t value, const config& data) {
    return value >> data.offset_bits;
}
