#pragma once

#include "Config.h"
#include "Bundle.h"
#include <iostream>
#include <vector>
#include <random>

struct WorkloadInfo {
    std::string pattern;
    int bundle_count = 0;
    int seed = 0;
};

std::vector<Bundle> generate_broadcast(const config& data, int bundle_count) {

    Bundle dataBundle;
    dataBundle.active_mask = 4294967295;
    std::vector<Bundle> bundles;

    for (int i=0;i<data.num_lanes;i++) {
        dataBundle.addr[i] =0;   
    }

    for (int i=0;i<bundle_count;i++) {
        bundles.push_back(dataBundle);
    }

    return bundles;

} 

std::vector<Bundle> coalesced_aligned(const config& data, int bundle_count) {

    std::vector<Bundle> bundles;

    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int base = i*data.num_lanes*data.access_bytes;

        for (int j=0;j<data.num_lanes;j++) {
            dataBundle.addr[j] =base+j*data.access_bytes;   
        }

        bundles.push_back(dataBundle);
    }

    return bundles;

} 


std::vector<Bundle> coalesced_misaligned(const config& data, int bundle_count, int offset) {

    std::vector<Bundle> bundles;

    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int base = offset+ i*data.num_lanes*data.access_bytes;

        for (int j=0;j<data.num_lanes;j++) {
            dataBundle.addr[j] =base+j*data.access_bytes;   
        }

        bundles.push_back(dataBundle);
    }

    return bundles;

} 


std::vector<Bundle> strided_k(const config& data, int bundle_count, int stride) {

    std::vector<Bundle> bundles;

    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int base = i*data.num_lanes*data.access_bytes*stride;

        for (int j=0;j<data.num_lanes;j++) {
            dataBundle.addr[j] =base+j*stride*data.access_bytes;   
        }

        bundles.push_back(dataBundle);
    }

    return bundles;

} 


std::vector<Bundle> reverse(const config& data, int bundle_count) {

    std::vector<Bundle> bundles;

    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int base = i*data.num_lanes*data.access_bytes;

        for (int j=data.num_lanes-1;j>=0;j--) {
            dataBundle.addr[j] =base+(data.num_lanes-1-j)*data.access_bytes;   
        }

        bundles.push_back(dataBundle);
    }

    return bundles;

} 


std::vector<Bundle> transpose(const config& data, int rows, int coloums) {

    std::vector<Bundle> bundles;

    for (int row_group=0;row_group<rows;row_group+=data.num_lanes) {

        for (int col=0;col<coloums;col++) {

            Bundle dataBundle;
            dataBundle.active_mask = 4294967295;

            for (int lane=0;lane<data.num_lanes;lane++) {
        
                int matrix_row = row_group + lane;
                int matrix_col = col;

                dataBundle.addr[lane] = (matrix_row*coloums + matrix_col)*data.access_bytes;
                
            
            }

            bundles.push_back(dataBundle);
        
        }

    }

    return bundles;

}

std::vector<Bundle> tiled(const config& data, int rows, int coloums) {

    std::vector<Bundle> bundles;

    const int tile_size =16;
    const int elements_per_tile = tile_size* tile_size;
    const int bundles_per_tile = elements_per_tile/data.num_lanes;


    for (int tile_row=0;tile_row<rows;tile_row+=tile_size) {

        for (int tile_col=0;tile_col<coloums;tile_col+=tile_size) {

            for (int b=0;b<bundles_per_tile;b++) {
        
                Bundle dataBundle;
                dataBundle.active_mask = 4294967295;

                for (int lane=0;lane<data.num_lanes;lane++) {
                    int element = b* data.num_lanes+lane;
                    int local_row = element/tile_size;
                    int local_col = element % tile_size;
                    int matrix_row = tile_row + local_row;
                    int matrix_col = tile_col + local_col;

                    dataBundle.addr[lane] = (matrix_row*coloums+matrix_col)*data.access_bytes;
                }

                bundles.push_back(dataBundle);
            
            }
        
        }

    }

    return bundles;

}



std::vector<Bundle> gather(const config& data, int bundle_count, int seed, int region_elements, int local_region_elements, double locality) {

    std::vector<Bundle> bundles;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> global_dist(0, region_elements-1);
    std::uniform_int_distribution<int> local_dist(0, local_region_elements-1);
    std::uniform_real_distribution<double> probability(0,1.0);


    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int randomNum = 0;

        for (int j=0;j<data.num_lanes;j++) {

            if (probability(rng) < locality) {
                randomNum = local_dist(rng);
            } else {
                randomNum = global_dist(rng);

            }

            dataBundle.addr[j] = (randomNum)*data.access_bytes; 
            
        } 

        bundles.push_back(dataBundle);
    }

    return bundles;

}


std::vector<Bundle> random_uniform(const config& data, int bundle_count, int seed, int region_elements) {

    std::vector<Bundle> bundles;
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(0, region_elements-1);

    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        for (int j=0;j<data.num_lanes;j++) {
            int randomNum = dist(rng);
            dataBundle.addr[j] = (randomNum)*data.access_bytes;  
        }


        bundles.push_back(dataBundle);
    }

    return bundles;

} 

std::vector<Bundle> hotspot(const config& data, int bundle_count, int seed, int region_elements) {

    std::vector<Bundle> bundles;
    std::mt19937 rng(seed);
    int hot_region_elements = static_cast<int>(0.2*region_elements);
    std::uniform_int_distribution<int> hot_dist(0, hot_region_elements-1);
    std::uniform_int_distribution<int> cold_dist(hot_region_elements, region_elements-1);
    std::uniform_real_distribution<double> probability(0,1.0);


    for (int i=0;i<bundle_count;i++) {
        
        Bundle dataBundle;
        dataBundle.active_mask = 4294967295;

        int randomNum = 0;

        for (int j=0;j<data.num_lanes;j++) {

            if (probability(rng) < 0.8) {
                randomNum = hot_dist(rng);
            } else {
                randomNum = cold_dist(rng);

            }

            dataBundle.addr[j] = (randomNum)*data.access_bytes; 
            
        } 

        bundles.push_back(dataBundle);
    }

    return bundles;

}