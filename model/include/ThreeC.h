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
#include "CacheAnalysis.h"
#include "OPT.h"
#include <stdexcept>

struct ThreeC {
    int compulsory=0;
    int capacity=0;
    int conflict=0;
};

int compulsory_misses(const config& data, const std::vector<uint32_t>& transactions) {

    OPT OPTinstance(data);
    int unique = OPTinstance.unique_cache_lines(data, transactions);

    return unique;
} 


int fully_associative_misses(const config& data, const std::vector<uint32_t>& transactions) {

    config d = data;
    d.ways = data.cache_bytes/data.line_bytes;
    d.finalise();

    CacheAnalysis cacheAnalysis(d);
    int misses_fa = cacheAnalysis.run(d,transactions);
 

    return misses_fa;
} 

int real_cache_misses(const config& data, const std::vector<uint32_t>& transactions) {

    CacheAnalysis cacheAnalysis(data);
    int misses = cacheAnalysis.run(data,transactions);

    return misses;
}


ThreeC run_all(const config& data, const std::vector<uint32_t>& transactions) {
    
    ThreeC outcome;
    outcome.compulsory = compulsory_misses(data, transactions);
    int misses_fa = fully_associative_misses(data, transactions);
    int misses_real = real_cache_misses(data, transactions);

    outcome.capacity = misses_fa - outcome.compulsory;
    outcome.conflict = misses_real - misses_fa;

    if (outcome.compulsory+outcome.capacity+outcome.conflict != misses_real || outcome.compulsory<0 || outcome.capacity <0 || outcome.conflict<0) {
        throw std::runtime_error("ERROR: THREE C's");
    }

    return outcome;
}