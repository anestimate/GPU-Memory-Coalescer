#pragma once

#include "Config.h"
#include <iostream>
#include <vector>
#include "WorkLoadGenerator.h"


void test_broadcast(const config& data, int bundle_count) {

    std::vector<Bundle> bundle = generate_broadcast(data, bundle_count);
    bool flag = false;

    for (int i=0;i<bundle_count;i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);

        if (transactions.size() != 1) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Broadcast Test: Passed!" << "\n";
    } else {
        std::cerr << "Broadcast Test: Failed!" << "\n";
    }

}


void test_coalesced_aligned(const config& data, int bundle_count) {

    std::vector<Bundle> bundle = coalesced_aligned(data, bundle_count);
    bool flag = false;

    for (int i=0;i<bundle_count;i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);

        if (transactions.size() != 2) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Coalesce Aligned Test: Passed!" << "\n";
    } else {
        std::cerr << "Coalesce Aligned Test: Failed!" << "\n";
    }

}

void test_coalesced_misaligned(const config& data, int bundle_count, int offset) {

    std::vector<Bundle> bundle = coalesced_misaligned(data, bundle_count, offset);
    bool flag = false;

    for (int i=0;i<bundle_count;i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);

        if (transactions.size() != 3) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Coalesce Misaligned Test: Passed!" << "\n";
    } else {
        std::cerr << "Coalesce Misaligned Test: Failed!" << "\n";
    }

}

void test_reverse(const config& data, int bundle_count) {

    std::vector<Bundle> bundle = reverse(data, bundle_count);
    bool flag = false;

    for (int i=0;i<bundle_count;i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);

        if (transactions.size() !=2) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Reverse Test: Passed!" << "\n";
    } else {
        std::cerr << "Reverse Test: Failed!" << "\n";
    }

}

void test_strided_k(const config& data, int bundle_count, int stride) {

    std::vector<Bundle> bundle = strided_k(data, bundle_count, stride);
    bool flag = false;


    for (int i=0;i<bundle_count;i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);
        
        if (transactions.size() != (size_t)(stride*2)) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Stride Test: Passed!" << "\n";
    } else {
        std::cerr << "Stride Test: Failed!" << "\n";
    }

}

double test_transpose(const config& data, int rows, int coloumns) {

    std::vector<Bundle> bundle = transpose(data, rows, coloumns);
    bool flag = false;
    std::vector<uint32_t> transactions;
    int total_transactions = 0;

    for (size_t i=0;i<bundle.size();i++) {
        transactions = coalescer(data, bundle[i]);
        total_transactions = total_transactions + transactions.size();
        if (transactions.size() != 32) {
            flag = true;
        }
    }

    if (flag == false) {
        std::cerr << "Transpose Test: Passed!" << "\n";
    } else {
        std::cerr << "Transpose Test: Failed!" << "\n";
    }

    double avg = 1.0* total_transactions/bundle.size();

    return avg;

}

double test_tiled(const config& data, int rows, int coloumns) {

    std::vector<Bundle> bundle = tiled(data, rows, coloumns);
    int transac_total = 0;
    double avg = 0;

    for (size_t i=0;i<bundle.size();i++) {
        std::vector<uint32_t> transactions = coalescer(data, bundle[i]);

        transac_total = transac_total + transactions.size();
    }

    avg = 1.0 * transac_total/bundle.size();
    return avg;
}

void test_tiledvtranspose(const config& data, int rows, int coloumns) {

    double avg = test_transpose(data,rows,coloumns);
    double average = test_tiled(data,rows,coloumns);

    std::cerr << "Tiled Average: " << average << "\n";
    std::cerr << "Transpose Average: " << avg << "\n";
    std::cerr << "Transpose to Tiled Ratio: " << avg/average << "\n";

    if (avg/average == 16) {
        std::cerr << "Tiled V Transpose Test: Passed!" << "\n";
    } else {
        std::cerr << "Tiled V Transpose Test: Failed!" << "\n";
    }

}

void test_random_uniform(const config& data, int bundle_count, int seed, int region_element) {

    std::vector<Bundle> bundle = random_uniform(data, bundle_count, seed, region_element);
    std::vector<Bundle> bundleTwo = random_uniform(data, bundle_count, seed, region_element);
    bool flag = false;


    for (int i=0;i<bundle_count;i++) {
        if (bundle[i].addr != bundleTwo[i].addr) {
            flag = true;
            break;
        }
        if (bundle[i].active_mask != bundleTwo[i].active_mask) {
            flag = true;
            break;
        }

    }

    if (flag == false) {
        std::cerr << "Random Uniform Test: Passed!" << "\n";
    } else {
        std::cerr << "Random Uniform Test: Failed!" << "\n";
    }

}


void test_gather(const config& data, int bundle_count, int seed, int region_elements, int local_region_elements, double locality) {

    std::vector<Bundle> bundle = gather(data, bundle_count, seed, region_elements, local_region_elements, locality);
    std::vector<Bundle> bundleTwo = gather(data, bundle_count, seed, region_elements, local_region_elements, locality);
    bool flag = false;


    for (int i=0;i<bundle_count;i++) {
        if (bundle[i].addr != bundleTwo[i].addr) {
            flag = true;
            break;
        }
        if (bundle[i].active_mask != bundleTwo[i].active_mask) {
            flag = true;
            break;
        }

    }

    if (flag == false) {
        std::cerr << "Gather Test: Passed!" << "\n";
    } else {
        std::cerr << "Gather Test: Failed!" << "\n";
    }

}


void test_hotspot(const config& data, int bundle_count, int seed, int region_elements) {

    std::vector<Bundle> bundle = hotspot(data, bundle_count, seed, region_elements);
    std::vector<Bundle> bundleTwo = hotspot(data, bundle_count, seed, region_elements);
    bool flag = false;


    for (int i=0;i<bundle_count;i++) {
        if (bundle[i].addr != bundleTwo[i].addr) {
            flag = true;
            break;
        }
        if (bundle[i].active_mask != bundleTwo[i].active_mask) {
            flag = true;
            break;
        }

    }

    if (flag == false) {
        std::cerr << "Hotspot Test: Passed!" << "\n";
    } else {
        std::cerr << "Hotspot Test: Failed!" << "\n";
    }

}