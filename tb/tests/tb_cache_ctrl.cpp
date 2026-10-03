#include <iostream>
#include "Vgpu_mem_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

//TEMPORARY TESTBENCH: TESTS CACHE CONTROL, MEMORY MODEL AND TAG ARRAY BUNDLE.
//DO NOT USE THIS TO TEST MODEL: IT HAS BEEN SUBSTANTIALLY CHANGED OVERTIME TO CATER FOR DIFFERENT TESTING REQUIREMENTS.
vluint64_t main_time = 0;

void tick(Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    dut->clk = 0;
    dut->eval();
    tfp->dump(main_time++);
    dut->clk = 1;
    dut->eval();
    tfp->dump(main_time++);

}


void rst(Vgpu_mem_top* dut, VerilatedVcdC* tfp) {
    dut->rst_n= 0;

    tick(dut, tfp);

    dut->eval();
    
    dut->rst_n= 1;
    tick(dut,tfp);
    dut->eval();

}


int main() {

    Verilated::traceEverOn(true);

    Vgpu_mem_top* dut = new Vgpu_mem_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;

    dut->trace(tfp,1000);
    tfp->open("wave.vcd");

    dut->req_valid = 0;
    dut->req_address = 0;
  
    int i=0;

    rst(dut,tfp);

    dut->req_valid = 1;
    dut->req_address = 123;
    
    dut->eval();

    tick(dut,tfp);

    dut->req_valid = 0;
    

    while (dut->mem_req != 1) {
        tick(dut,tfp);
    
    }
    

    
    tick(dut,tfp);
    for (int c =0;c<105;c++) {
        tick(dut,tfp);
    }


    std::cout << dut->n_requests <<"\n";
    std::cout << dut->n_hits << "\n";
    std::cout << dut->n_misses <<"\n";
    std::cout << dut->n_evictions << "\n";
    std::cout << dut->n_mem_reqs <<"\n";
    std::cout << dut->total_bytes <<"\n";


    dut->req_valid = 0;
    dut->req_address = 0;

    
    dut->eval();

    while (!dut->req_ready) {
        tick(dut, tfp);
    }


    dut->req_valid = 1;
    dut->req_address = 898;
    
    dut->eval();

    tick(dut,tfp);

    dut->req_valid = 0;

    for (int c =0;c<105;c++) {
        tick(dut,tfp);
    }

    std::cout << dut->n_requests <<"\n";
    std::cout << dut->n_hits << "\n";
    std::cout << dut->n_misses <<"\n";
    std::cout << dut->n_evictions << "\n";
    std::cout << dut->n_mem_reqs <<"\n";
    std::cout << dut->total_bytes <<"\n";


    dut->req_valid = 0;
    dut->req_address = 0;

    
    dut->eval();

    while (!dut->req_ready) {
        tick(dut, tfp);
    }


    dut->req_valid = 1;
    dut->req_address = 3;

    dut->eval();
    dut->req_valid = 0;
    for (int c =0;c<105;c++) {
        tick(dut,tfp);
    }

    std::cout << dut->n_requests <<"\n";
    std::cout << dut->n_hits << "\n";
    std::cout << dut->n_misses <<"\n";
    std::cout << dut->n_evictions << "\n";
    std::cout << dut->n_mem_reqs <<"\n";
    std::cout << dut->total_bytes <<"\n";


    dut->req_valid = 0;
    dut->req_address = 0;

    
    dut->eval();

    while (!dut->req_ready) {
        tick(dut, tfp);
    }


    dut->req_valid = 1;
    dut->req_address = 644;
    
    dut->eval();
    
    tick(dut,tfp);

    for (int c = 0;c<105;c++) {
        tick(dut,tfp);
    }

    std::cout << dut->n_requests <<"\n";
    std::cout << dut->n_hits << "\n";
    std::cout << dut->n_misses <<"\n";
    std::cout << dut->n_evictions << "\n";
    std::cout << dut->n_mem_reqs <<"\n";
    std::cout << dut->total_bytes <<"\n";
    
  

    tfp->close();
    delete tfp;
    delete dut;
}