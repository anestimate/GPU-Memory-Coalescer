#include <iostream>
#include "Config.h"
#include "Stats.h"
#include "Bundle.h"
#include "Coalescer.h"
#include "tb_coalescer.h"
#include "Cache.h"
void multiple_requests(const config& data, Bundle& bundleData,Cache& cacheInstance );
#include "tb_cache.h"
#include "tb_MSHR.h"
#include "tb_OPT.h"
#include "tb_threeC.h"
#include "tb_workload.h"
#include "tb_workloadmodel.h"
#include <string>



void multiple_requests(const config& data, Bundle& bundleData, Cache& cacheInstance) {

    statsService.lanes +=bundleData.active_mask.count();
    std::vector<uint32_t> transactions = coalescer(data, bundleData);
    statsService.transaction_log.insert(statsService.transaction_log.end(), transactions.begin(), transactions.end());
    int transaction_number = transactions.size();

    for (int i=0;i<transaction_number;i++) {
        cacheInstance.access(data, transactions[i]);
    }
    
    statsService.current_cycle = statsService.current_cycle + transaction_number;
    statsService.transactions += transaction_number;
    statsService.bundles++;
    statsService.transactions_per_bundle[transaction_number]++;

}

int main(int argc, char** argv) {

    config data;
    Bundle dataBundle;
    
    data.num_lanes = 32;
    data.addr_bits = 32;
    data.line_bytes = 64;
    data.cache_bytes = 8192;
    data.ways = 4;
    data.num_mshr = 1;
    data.mem_latency = 100;
    data.access_bytes = 4;
  
    if (argc<2) {
        std::cerr << "Usage: ./sim <workload>\n";
        return 1;
    }

    int offset = 4;
    double locality = 0.66;

    for (int i=2;i<argc;i++) {
        std::string arg = argv[i];
        std::string key = arg.substr(0, arg.find('='));
        std::string value = arg.substr(arg.find('=') +1);

        if (key == "ways") {
            data.ways = std::stoi(value);
        } else if (key == "line_bytes") {
            data.line_bytes=std::stoi(value);
        } else if (key == "cache_bytes") {
            data.cache_bytes=std::stoi(value);
        } else if (key == "num_mshr") {
            data.num_mshr=std::stoi(value);
        } else if (key == "offset") {
            offset= std::stoi(value);
        } else if (key == "locality") {
            locality = std::stod(value);
        } else {
            std::cerr << "Unkown Option: " << key << "\n";
            return 1;
        }
    }

    data.finalise();

    std::string workload = argv[1];


    // test_sameLine(data);
    // test_consecutive(data);
    // test_misaligned(data);
    // test_misaligned60(data);
    // test_stride64(data);
    // test_stride128(data);
    // test_stride_reverse(data);
    // test_halfmask(data);
    // test_zeromask(data);
    // test_cluster(data);

    // test_compulsory(data);
    // test_temporal(data);
    // test_spatial(data);
    // test_capacity(data);
    // test_conflict(data);
    // test_boundary(data);
    
    // test_fourway_replacement(data);
    // test_fourways_access(data);
    // test_evict_precise(data);
    // test_evict_all(data);
    // test_mixed_locality(data);

    // bundle_test_ONE(data, dataBundle);
    // bundle_test_TWO(data, dataBundle);
    // bundle_test_THREE(data, dataBundle);
    // bundle_test_FOUR(data, dataBundle);
    // bundle_test_throughput(data, dataBundle);

    //test_secondary_miss(data);
    //test_stall(data); 
    //test_saturation(data);

    //test_OPT_misses(data);
    //test_OPT_next_use(data);

    //test_conflictThree(data);
    //test_capacityThree(data);
    //test_capacityOPTvLRU(data);

    // test_broadcast(data,10000);
    // test_coalesced_aligned(data,10000);
    // test_coalesced_misaligned(data,10000,4);
    // test_reverse(data,10000);
    // test_strided_k(data,10000,8);
    // test_tiledvtranspose(data,256,256);
    // test_random_uniform(data,10000,1245,16345);
    // test_gather(data, 10000,313,1354,100, 0.6);
    // test_hotspot(data, 10000,313, 106);


    if (workload == "broadcast") {
        workload_broadcast(data,100);
    } else if (workload == "coalesced_aligned") {
        workload_coalesced_aligned(data,100);
    } else if (workload == "coalesced_misaligned") {
        workload_coalesced_misaligned(data,100,offset);
    } else if (workload == "strided_k") {
        workload_strided_k(data,100,4);
    } else if (workload == "reverse") {
        workload_reverse(data,100);
    } else if (workload == "transpose") {
        workload_transpose(data,256,256);
    } else if (workload == "tiled") {
        workload_tiled(data,256,256);
    } else if (workload == "random_uniform") {
        workload_random_uniform(data, 100, 3241,343);
    } else if (workload == "gather") {
        workload_gather(data,100,3241,565,100,locality);
    } else if (workload == "hotspot") {
        workload_hotspot(data,100,3241,565);
    } else {
        std::cerr << "Unknown Workload: " << workload <<"\n";
        return 1;
    }

    return 0;
}