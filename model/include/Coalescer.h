#pragma once
#include "Config.h"
#include "Bundle.h"
#include <iostream>
#include <stdexcept>
#include <vector>

std::vector<uint32_t> coalescer(const config& data, Bundle& bundleData) {

    uint32_t master_target = 0;
    std::vector<uint32_t> transactions;
    bool hit = false;
    bool warning = false;


    for (int k=0;k<data.num_lanes;k++) {

        hit = false;

        for (int i=data.num_lanes-1;i>=0;i=i-1) {

            if (bundleData.active_mask[i]==1) {
                master_target = bundleData.addr[i];
            }

        }

        for (int j=0;j<data.num_lanes;j++) {

            if (bundleData.active_mask[j]==1) {
                if ((bundleData.addr[j]>>data.offset_bits) == (master_target>>data.offset_bits )) {
                    hit = true;

                    //std::cout << "Included Lane: " << j << "\n";
                    bundleData.active_mask[j] = 0;
                }
            }
            
        }

        
        if (hit ==true) {
            transactions.push_back(master_target & ~(uint32_t)(data.line_bytes-1));
        }

    }
    

    for (int i=0;i<32;i++) {
        if (bundleData.active_mask[i] == 1) {
            warning =true;
        }
    }

    if (warning ==true) {
        throw std::runtime_error("ACTIVE LANES ERROR, REQUEST NOT FULFILLED");
    }


    
    //std::cout << "Total Transactions: "<< 32-transactions << "\n";
    
    return transactions;

};  