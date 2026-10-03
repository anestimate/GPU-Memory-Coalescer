#pragma once
#include "Config.h"
#include "Bundle.h"
#include "Coalescer.h"
#include <iostream>
#include <set>
#include <cmath>


int analytic_check(int base, int stride, std::bitset<32> active_mask) {

    int elem_bytes = 4;
    int expected_Transactions = 0;
    std::set<int> mem_lanesTWO;

    int lane_addrTWO [32];

    for (int i = 0; i<32; i++) {
        lane_addrTWO[i] = base+stride*i*elem_bytes;
    }

    for (int i = 0; i<32; i++) {

        if (active_mask[i] == 1) {
            mem_lanesTWO.insert(lane_addrTWO[i]/64);
        }

    }

    expected_Transactions = mem_lanesTWO.size();


    return expected_Transactions;

}

void test_sameLine(const config& data) {
    
    Bundle dataBundle;

    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = 4395;
    }

    auto active_mask = dataBundle.active_mask;
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(dataBundle.addr[0],0,active_mask)) {
        std::cout << "Test 1: PASS"<< "\n";
    } else {
        std::cout << "Test 1: FAIL"<< "\n";
    }

}   

void test_consecutive(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = i*4;
    }

    auto active_mask = dataBundle.active_mask;

    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(dataBundle.addr[0],1, active_mask)) {
        std::cout << "Test 2: PASS"<< "\n";
    } else {
        std::cout << "Test 2: FAIL"<< "\n";
    }


} 

void test_misaligned(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = (i+1)*4;
    }

    auto active_mask = dataBundle.active_mask;

    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(4,1, active_mask)) {
        std::cout << "Test 3: PASS"<< "\n";
    } else {
        std::cout << "Test 3: FAIL"<< "\n";
    }


}

void test_misaligned60(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = (i+60)*4;
    }

    auto active_mask = dataBundle.active_mask;
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(60,1, active_mask)) {
        std::cout << "Test 4: PASS"<< "\n";
    } else {
        std::cout << "Test 4: FAIL"<< "\n";
    }

} 

void test_stride64(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = (i)*64;
    }

    auto active_mask = dataBundle.active_mask;
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(0,64, active_mask)) {
        std::cout << "Test 5: PASS"<< "\n";
    } else {
        std::cout << "Test 5: FAIL"<< "\n";
    }

} 

void test_stride128(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = (i)*128;
    }

    auto active_mask = dataBundle.active_mask;
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(0,128, active_mask)) {
        std::cout << "Test 6: PASS"<< "\n";
    } else {
        std::cout << "Test 6: FAIL"<< "\n";
    }

} 


void test_stride_reverse(const config& data) {

    Bundle dataBundle;
    dataBundle.active_mask = 0xFFFFFFFF;

    for (int i=32;i>=1;i=i-1) {
        dataBundle.addr[i-1] = 128-4*i;
    }

    auto active_mask = dataBundle.active_mask;
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == analytic_check(124,-1, active_mask)) {
        std::cout << "Test 7: PASS"<< "\n";
    } else {
        std::cout << "Test 7: FAIL"<< "\n";
    }

} 


void test_halfmask(const config& data) {
    
    Bundle dataBundle;

    dataBundle.active_mask = 65535;

    for (int i=0;i<16;i++) {
        dataBundle.addr[i] = 4395;
    }

    for (int i=17;i<32;i++) {
        dataBundle.addr[i] = 10;
    }
    
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == 1) {
        std::cout << "Test 8: PASS"<< "\n";
    } else {
        std::cout << "Test 8: FAIL"<< "\n";
    }

}  

void test_zeromask(const config& data) {
    
    Bundle dataBundle;

    dataBundle.active_mask = 0;

    for (int i=0;i<32;i++) {
        dataBundle.addr[i] = 4395;
    }
    
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == 0) {
        std::cout << "Test 9: PASS"<< "\n";
    } else {
        std::cout << "Test 9: FAIL"<< "\n";
    }
} 


void test_cluster(const config& data) {
    
    Bundle dataBundle;

    dataBundle.active_mask =  0xFFFFFFFF;

    for (int i=0;i<17;i++) {
        dataBundle.addr[i] = 10;
    }

    for (int i=17;i<32;i++) {
        dataBundle.addr[i] = 5465;
    }
    std::vector <uint32_t> transaction = coalescer(data, dataBundle);
    int transaction_number = transaction.size();

    if ((transaction_number) == 2) {
        std::cout << "Test 10: PASS"<< "\n";
    } else {
        std::cout << "Test 10: FAIL"<< "\n";
    }

}  

