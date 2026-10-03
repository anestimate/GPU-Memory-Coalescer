#include <iostream>
#include "Vcoalescer.h"
#include <set>
#include <cmath>


void tick(Vcoalescer* dut) {

    dut->clk = 0;
    dut->eval();

    dut->clk = 1;
    dut->eval();

}


void rst(Vcoalescer* dut) {
    dut->rst_n= 0;
    tick(dut);
    dut->eval();
    
    dut->rst_n= 1;
    tick(dut);
    dut->eval();
}
 

void test_sameLine(Vcoalescer* dut) {

    dut->in_valid = 1;
    dut->out_ready =1;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = 4395;
    }

    dut->eval();

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
    }

    
    std::cout << "T1 Lane Address: " << dut->out_lane_addr << "\n";
    std::cout << "T1 Lane Mask: " << dut->out_lane_mask << "\n";

    rst(dut);


}   

void test_consecutive(Vcoalescer* dut) {

    dut->in_valid = 1;
    dut->out_ready =1;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = i*4;
    }

    dut->eval();

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
    }

    std::cout << "T2 Lane Address: " << dut->out_lane_addr << "\n";
    std::cout << "T2 Lane Mask: " << dut->out_lane_mask << "\n";

    while (!(dut->out_last)) {
        tick(dut);
        std::cout << "T2 Lane Address: " << dut->out_lane_addr << "\n";
        std::cout << "T2 Lane Mask: " << dut->out_lane_mask << "\n";
    }

    rst(dut);


} 

void test_misaligned(Vcoalescer* dut) {

    dut->in_valid = 1;
    dut->out_ready =1;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = (i+1)*4;
    }

    dut->eval();

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
    }

    std::cout << "T3 Lane Address: " << dut->out_lane_addr << "\n";
    std::cout << "T3 Lane Mask: " << dut->out_lane_mask << "\n";

    while (!(dut->out_last)) {
        tick(dut);
        std::cout << "T3 Lane Address: " << dut->out_lane_addr << "\n";
        std::cout << "T3 Lane Mask: " << dut->out_lane_mask << "\n";
    }

    rst(dut);


} 

void test_stride64(Vcoalescer* dut) {

    dut->in_valid = 1;
    dut->out_ready =1;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = (i)*64;
    }

    dut->eval();

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
    }

    std::cout << "T4 Lane Address: " << dut->out_lane_addr << "\n";
    std::cout << "T4 Lane Mask: " << dut->out_lane_mask << "\n";

    while (!(dut->out_last)) {
        tick(dut);
        std::cout << "T4 Lane Address: " << dut->out_lane_addr << "\n";
        std::cout << "T4 Lane Mask: " << dut->out_lane_mask << "\n";
    }

    rst(dut);


} 

void test_ready_check(Vcoalescer* dut) {

    int cycles = 0;
    dut->in_valid = 1;
    dut->out_ready =0;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = 4395;
    }

    dut->eval();

    int containerOne = dut->out_lane_addr;
    int containerTwo = dut->out_lane_mask;

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
        cycles ++;

        if (cycles > 1000){
            std::cout << "YOUR TEST HAS STALLED. AUTO CRASH INITIATED" << "\n";
            break;
        }

        if ((dut->out_valid==1) && (dut->out_ready == 0)) {
            if (containerOne != dut->out_lane_addr  || containerTwo != dut->out_lane_mask) {
                std::cout << "TEST FAILED";
            }

            containerOne = dut->out_lane_addr;
            containerTwo = dut->out_lane_mask;
        } 
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
        cycles ++;

        if (cycles > 1000){
            std::cout << "YOUR TEST HAS STALLED. Expected" << "\n";
            break;
        }
    }


    rst(dut);


    dut->in_valid = 1;
    dut->out_ready =1;
    dut->active_mask = 0xFFFFFFFF;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = 4395;
    }

    dut->eval();

    while (!(dut->in_ready & dut->in_valid)) {
        tick(dut);
    }

    tick(dut); 

    dut->in_valid = 0;

    while (!(dut->out_ready & dut->out_valid)) {
        tick(dut);
    }

    std::cout << "T1 Lane Address: " << dut->out_lane_addr << "\n";
    std::cout << "T1 Lane Mask: " << dut->out_lane_mask << "\n";

    rst(dut);

   

}

int test_expected_T(int base, int stride, int elem_bytes, int active_mask[],Vcoalescer* dut) {
    rst(dut);
    int expected_Transactions = 0;
    std::set<int> mem_lanes;

    int lane_addr [32];

    for (int i = 0; i<30; i++) {
        lane_addr[i] = 10;
    }

    lane_addr[30] = 3245;
    lane_addr[31] = 45641;

    for (int i = 0; i<32; i++) {

        if (active_mask[i] == 1) {
            mem_lanes.insert(lane_addr[i]/64);
        }

    }

    expected_Transactions = mem_lanes.size();

    //std::cout << "Expected Transactions CASE 1: " << expected_Transactions << "\n"; 

    rst(dut);

    int expected_TransactionsTWO = 0;
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

    expected_TransactionsTWO = mem_lanesTWO.size();

    std::cout << "Expected Transactions CASE 2: " << expected_TransactionsTWO << "\n"; 

    return expected_TransactionsTWO;
}

int actual_T(int base, int stride, int elem_bytes, int active_mask[], Vcoalescer* dut) {

    rst(dut);

    int cycles = 0;
    int transact_count = 0;
    vluint32_t active_calculator = 0;

    dut->in_valid = 1;
    dut->out_ready =1;

    for (int i = 0; i<32; i++) {

        dut->lane_addr[i] = base+stride*i*elem_bytes;

        if (active_mask[i] ==1) {
            active_calculator = active_calculator+ pow(2,i);
        } 
    }

    dut->active_mask = active_calculator;
    dut->eval();

    while ((!(dut->in_ready & dut->in_valid)) && cycles<10000) {
        tick(dut); 
        cycles++;
    }

    tick(dut); 

    for (int j = 0;j<32;j++) {

        dut->in_valid = 0;

        while ((!(dut->out_ready & dut->out_valid)) && cycles<10000) {
            tick(dut);
            cycles++;
        }

        if (dut->out_ready & dut->out_valid) {
            transact_count++;
        }

        if (dut->out_last==1) {
            break;
        } else {
            tick(dut);

            while ((!(dut->out_ready & dut->out_valid)) && cycles<10000) {
                tick(dut);
               
            }
        }

    }

    std::cout << "Actual Transactions: " << transact_count << "\n";
    
    return transact_count;

}

void check(int base, int stride, int elem_bytes, int active_mask[], Vcoalescer* dut) {
    
    int a = test_expected_T(base,stride, elem_bytes,active_mask,dut);

    int b = actual_T(base,stride, elem_bytes,active_mask, dut);

    if (a==b) {
        std::cout << "TRANSACTIONS CHECK PASSED" << "\n";
    } else {
        std::cout << "TRANSACTIONS CHECK FAILED" << "\n";
    }
    
    rst(dut);
}

int main() {

    Vcoalescer* dut = new Vcoalescer;

    rst(dut);

    //test_sameLine(dut);
    //test_consecutive(dut);
    //test_misaligned(dut);
    //test_stride64(dut);
    //test_ready_check(dut);

    int active_mask [32];
    int stride  = 1;
    int base = 60;
    int elem_bytes = 4;


    for (int i = 0; i<32; i++) {
        active_mask[i] = 1;
    }

    check(base,stride, elem_bytes,active_mask,dut);

    int strideTWO = 64;


    for (int i = 0; i<32; i++) {
        active_mask[i] = 1;
    }

    check(base,strideTWO, elem_bytes,active_mask,dut);

    int strideTHREE = 128;


    for (int i = 0; i<32; i++) {
        active_mask[i] = 1;
    }

    check(base,strideTHREE, elem_bytes,active_mask,dut);

    rst(dut);
    
    int strideFOUR  = -4;
    int baseTWO = 124;
    int elem_bytesTWO = 1;


    check(baseTWO,strideFOUR, elem_bytesTWO,active_mask,dut);


    int active_maskFIVE [32];
    int strideFIVE  = 1;
    int baseFIVE = 60;
    int elem_bytesFIVE = 4;


    for (int i = 0; i<16; i++) {
        active_maskFIVE[i] = 1;
    }

    for (int i = 16; i<32; i++) {
        active_maskFIVE[i] = 0;
    }

    check(baseFIVE,strideFIVE, elem_bytesFIVE,active_maskFIVE,dut);

    int active_maskSIX [32];

    for (int i = 0; i<32; i++) {
        active_maskSIX[i] = 0;
    }


    check(baseFIVE,strideFIVE, elem_bytesFIVE,active_maskSIX,dut);

}

