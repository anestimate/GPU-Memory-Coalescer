
#pragma once
#include "Config.h"
#include "Cache.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "OPT.h"

void output_json(const config& data, const Cache& cacheInstance, const Stats& statsService) {

    std::cout << "{";

    std::cout << "\"config\":{";
    std::cout << "\"ways\":" << data.ways << ","; 
    std::cout << "\"line_bytes\":" << data.line_bytes << ",";
    std::cout << "\"cache_bytes\":" << data.cache_bytes;
    std::cout << "}," ;


    std::cout << "\"counters\":{";
    std::cout << "\"requests\":" << cacheInstance.requests <<",";
    std::cout << "\"hits\": "<< cacheInstance.hits << ",";
    std::cout << "\"misses\":" << cacheInstance.misses <<",";
    std::cout<< "\"evictions\":" << cacheInstance.evictions << ",";
    std::cout << "\"mem_reqs\":" << cacheInstance.mem_reqs <<",";
    std::cout << "\"bundles\":" << statsService.bundles <<",";
    std::cout << "\"lanes\":" << statsService.lanes <<",";
    std::cout << "\"transactions\":" << statsService.transactions <<",";
    std::cout << "\"transactions_per_bundle\":[";
    for (int i= 0 ; i< 33; i++) {
        std::cout << statsService.transactions_per_bundle[i];

        if (i<32) {
            std::cout << ",";
        }
    }
    std::cout <<"],";

    std::cout << "\"cycles\":" << statsService.current_cycle <<",";
    std::cout << "\"mem_bytes\":"<< cacheInstance.total_bytes << ",";
    std::cout << "\"secondary_misses\":"<< statsService.secondary_miss << ",";
    OPT optInstance(data);
    std::cout << "\"opt_misses\":" << optInstance.run(data, statsService.transaction_log);
    std::cout << "}," ;

    std::cout << "\"fill_logs\":[";

    int n_fills = std::min(cacheInstance.index_counter,2048);
    for (int i = 0; i < n_fills; i++) {
        std::cout << "{ ";
        std::cout <<"\"set\":" <<  cacheInstance.fill_set[i] << ",";
        std::cout <<"\"way\":" <<  cacheInstance.fill_way[i] << ",";
        std::cout <<"\"tag\":" <<  cacheInstance.fill_tag[i];
        std::cout << "}";

        if (i <  n_fills - 1) {
            std::cout << ",";
        }
    }

    std::cout << "]," ;

    std::cout << "\"derived\":{";
    std::cout << "\"hit_rate\":" << 1.0 * cacheInstance.hits/cacheInstance.requests << ","; 
    std::cout << "\"mean_transactions_per_bundle\":" << 1.0 *  statsService.transactions/statsService.bundles << ",";
    std::cout << "\"coalescing_efficiency\":" << 1.0 *  statsService.lanes/statsService.transactions << ",";
    std::cout << "\"mean_mshr_occupancy\":" << 1.0 *  statsService.mshr_occupancy_sum/statsService.mshr_occupancy_samples << ",";
    std::cout << "\"bytes_per_lane\":" << 1.0 *  cacheInstance.total_bytes/statsService.lanes;
    std::cout << "}" ;

    std::cout << "}" ;
}

void output_stats(const config& data, const Cache& cacheInstance, const Stats& statsService) {

    (void)data;

    std::cerr << "requests:" << cacheInstance.requests <<"\n";
    std::cerr << "hits: "<< cacheInstance.hits << "\n";
    std::cerr << "misses:" << cacheInstance.misses <<"\n";
    std::cerr << "evictions:" << cacheInstance.evictions << "\n";
    std::cerr << "mem_reqs:" << cacheInstance.mem_reqs <<"\n";
    std::cerr << "bundles:" << statsService.bundles <<"\n";
    std::cerr << "lanes:" << statsService.lanes <<"\n";
    std::cerr << "transactions:" << statsService.transactions <<"\n";
    std::cerr << "transactions_per_bundle:";
    for (int i= 0 ; i< 33; i++) {
        std::cerr << statsService.transactions_per_bundle[i];

        if (i<32) {
             std::cerr << ",";
        }
    }
    std::cerr <<"\n";
    std::cerr << "cycles:" << statsService.current_cycle <<"\n";
    std::cerr << "mem_bytes:"<< cacheInstance.total_bytes << "\n";
    std::cerr << "secondary_misses:"<< statsService.secondary_miss << "\n";



    std::cerr << "hit_rate:" << 1.0 * cacheInstance.hits/cacheInstance.requests << "\n"; 
    std::cerr<< "mean_transactions_per_bundle:" << 1.0 *  statsService.transactions/statsService.bundles << "\n";
    std::cerr << "coalescing_efficiency:" << 1.0 *  statsService.lanes/statsService.transactions << "\n";
    std::cerr << "mean_mshr_occupancy:" << 1.0 *  statsService.mshr_occupancy_sum/statsService.mshr_occupancy_samples << "\n";
    std::cerr << "bytes_per_lane:" << 1.0 *  cacheInstance.total_bytes/statsService.lanes << "\n";

}