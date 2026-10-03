#pragma once
#include <algorithm>
#include "Config.h"

vluint64_t main_time = 0;
vluint64_t total_cycles = 0;

void tick(Vgpu_mem_top* dut, VerilatedVcdC* tfp) {

    dut->clk = 0;
    dut->eval();
    //tfp->dump(main_time++);
    dut->clk = 1;
    dut->eval();
    //tfp->dump(main_time++);

    total_cycles++;

}


void rst(Vgpu_mem_top* dut, VerilatedVcdC* tfp) {
    
    dut->rst_n= 0;
    tick(dut, tfp);
    dut->eval();
    
    dut->rst_n= 1;
    tick(dut,tfp);
    dut->eval();

}

void output_stats(Vgpu_mem_top* dut) {

    std::cerr << "Number of Requests: " << dut->n_requests <<"\n";
    std::cerr << "Number of Hits: "<< dut->n_hits << "\n";
    std::cerr << "Number of Misses: " << dut->n_misses <<"\n";
    std::cerr<<  "Number of Evictions: " << dut->n_evictions << "\n";
    std::cerr << "Number of Memory Requests: " << dut->n_mem_reqs <<"\n";
    std::cerr << "Number of Total Bytes: "<< dut->total_bytes <<"\n";
    std::cerr << "Fills Logged: " << dut->index_counter << "\n";

    int n_fills = std::min<uint32_t>(dut->index_counter,2048);
    for (int i = 0; i < n_fills; i++) {
        std::cerr << "Fill " << i
                << ": Set " << +dut->fill_set[i]
                << ", Way " << +dut->fill_way[i]
                << ", Tag " << dut->fill_tag[i]
                << "\n";
    }
    std::cerr << "" <<"\n";
}


void output_json(Vgpu_mem_top* dut, const config& data) {

    std::cout << "{";

    std::cout << "\"config\":{";
    std::cout << "\"ways\":" << data.ways << ","; 
    std::cout << "\"line_bytes\":" << data.line_bytes << ",";
    std::cout << "\"cache_bytes\":" << data.cache_bytes;
    std::cout << "}," ;


    std::cout << "\"counters\":{";
    std::cout << "\"requests\":" << dut->n_requests <<",";
    std::cout << "\"hits\": "<< dut->n_hits << ",";
    std::cout << "\"misses\":" << dut->n_misses <<",";
    std::cout<< "\"evictions\":" << dut->n_evictions << ",";
    std::cout << "\"mem_reqs\":" << dut->n_mem_reqs <<",";
    std::cout << "\"cycles\":" << total_cycles <<",";
    std::cout << "\"mem_bytes\":"<< dut->total_bytes;
    std::cout << "}," ;

    std::cout << "\"fill_logs\":[";

    int n_fills = std::min<uint32_t>(dut->index_counter,2048);
    for (int i = 0; i < n_fills; i++) {
        std::cout << "{ ";
        std::cout <<"\"set\":" << +dut->fill_set[i] << ",";
        std::cout <<"\"way\":" << +dut->fill_way[i] << ",";
        std::cout <<"\"tag\":" << +dut->fill_tag[i];
        std::cout << "}";

        if (i < n_fills - 1) {
            std::cout << ",";
        }
    }

    std::cout << "]" ;


    std::cout << "}" ;

}

void access(Vgpu_mem_top* dut, VerilatedVcdC* tfp,  uint32_t addressAccessed) {

    int clock = 0;

    for (int i=1;i<32;i++) {
        dut->lane_addr[i] = 0;
    }

    dut->lane_addr[0] = addressAccessed;
    dut->active_mask = 1;

    dut->req_valid=1;

    tick(dut,tfp);

    while (!((dut->in_ready)&& dut->req_valid) && clock<1000) {
        tick(dut,tfp);
        clock++;
    } 

    if (clock >= 1000) {
        std::cerr << "Error in processing request: First Instance";
    }

    clock = 0;
    dut->req_valid =0;

    while (!(dut->out_valid && dut->req_ready)&&clock<1000) {
        tick(dut,tfp);
        clock++;
    }

    clock = 0;

    while (!(dut->req_ready)&&clock<1000) {
        tick(dut,tfp);
        clock++;
    }

    if (clock >= 1000) {
        std::cerr << "Error in processing request: Second instance";
    }

    
}


void access_bundle(Vgpu_mem_top* dut, VerilatedVcdC* tfp, std::vector<uint32_t> addressAccessed, uint32_t active_mask) {

    int clock = 0;

    for (int i=0;i<32;i++) {
        dut->lane_addr[i] = addressAccessed[i];
    }

    dut->active_mask = active_mask;

    dut->req_valid=1;

    tick(dut,tfp);

    while (!((dut->in_ready)&& dut->req_valid) && clock<10000) {
        tick(dut,tfp);
        clock++;
    } 

    if (clock >= 10000) {
        std::cerr << "Error in processing request: First Instance";
    }

    dut->req_valid =0;
    clock = 0;

    while (!(dut->req_ready)&&clock<10000) {
        tick(dut,tfp);
        clock++;
    }

    if (clock >= 10000) {
        std::cerr << "Error in processing request: Second instance";
    }

    
}