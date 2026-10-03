#include <iostream>
#include "Vtag_array.h"

//TEMPORARY TESTBENCH: TESTS CACHE CONTROL AND TAG ARRAY BUNDLE.

void tick(Vtag_array* dut) {

    dut->clk = 0;
    dut->eval();

    dut->clk = 1;
    dut->eval();

}


void rst(Vtag_array* dut) {
    dut->rst_n= 0;
    tick(dut);
    dut->eval();
    
    dut->rst_n= 1;
    tick(dut);
    dut->eval();

}

void test_rst(Vtag_array* dut) {
    rst(dut);

    tick(dut);

    bool flag = false;

    const int NUM_SETS = 32;


    for (int i=0; i<NUM_SETS; i++) {
        
        dut->rd_tag = i;
        dut->rd_index = i;

        dut->eval();

        if (dut->hit != 0)
            flag = true;
       
        
    }

    if (flag == true)
        std::cout << "REST CHECK FAILED!" << "\n";
    else
        std::cout << "RESET CHECK PASSED!" << "\n";

}   


void test_lookup(Vtag_array* dut) {

    dut->wr_en = 1;
    dut->wr_index = 5;
    dut->wr_way = 0;
    dut->wr_tag = 123;
    dut->eval();

    tick(dut);

    dut->rd_tag = 123;
    dut->rd_index = 5;
    dut->wr_en = 0;

    dut->eval();


    if (dut->hit == 1)
        std::cout << "LOOKUP CHECK PASSED!" << "\n";
    else
        std::cout << "LOOKUP CHECK FAILED!" << "\n";

    
    std::cout << "LOOKUP INVALIDATED: " << "\n";


     
    dut->inv_en = 1;
    dut->inv_index = 5;
    dut->inv_way = 0;
    dut->eval();

    tick(dut);

    dut->rd_tag = 123;
    dut->rd_index = 5;
    dut->inv_en=0;

    dut->eval();

    tick(dut);

    if (dut->hit == 1)
        std::cout << "LOOKUP HIT!" << "\n";
    else
        std::cout << "LOOKUP MISSED!" << "\n";

}   




int main() {

    Vtag_array* dut = new Vtag_array;

    test_rst(dut);
    test_lookup(dut);


}