#include <iostream>
#include "Vgpu_mem_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include "tb_util.h"
#include "regression_suite.h"


#ifndef CFG_WAYS
#define CFG_WAYS 4
#endif
#ifndef CFG_LINE_BYTES
#define CFG_LINE_BYTES 64
#endif
#ifndef CFG_CACHE_BYTES
#define CFG_CACHE_BYTES 8192
#endif


int main(int argc, char** argv) {

    Verilated::traceEverOn(true);

    Vgpu_mem_top* dut = new Vgpu_mem_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;

    config data;
    Bundle dataBundle;
    
    data.num_lanes = 32;
    data.addr_bits = 32;
    data.line_bytes = CFG_LINE_BYTES;
    data.cache_bytes = CFG_CACHE_BYTES;
    data.ways = CFG_WAYS;
    data.num_mshr = 1;
    data.mem_latency = 100;
    data.access_bytes = 4;

    data.finalise();
  

    dut->trace(tfp,1000);
    tfp->open("wave.vcd");

    rst(dut,tfp);

    if (argc<2) {
        std::cerr << "Usage: ./sim <workload>\n";
        return 1;
    }

    std::string workload = argv[1];

    //test_compulsory(dut,tfp);
    //test_temporal(dut,tfp);
    //test_spatial(dut,tfp);
    //test_capacity(dut,tfp);
    //test_conflict(dut,tfp);
    //test_boundary(dut,tfp);

    //Criteria for Pass is matching with files stored in results/rtl-baseline:
    //test_fourway_replacement(dut,tfp);
    //test_fourways_access(dut,tfp);
    //test_evict_precise(dut,tfp);
    //test_evict_all_sets(dut,tfp);
    //test_mixed_locality(dut,tfp);

    //bundle_test_ONE(dut,tfp);
    //bundle_test_TWO(dut,tfp);
    //bundle_test_THREE(dut,tfp);
    //bundle_test_FOUR(dut,tfp);

    if (workload == "broadcast") {
        workload_broadcast(data,dut,tfp,100);
    } else if (workload == "coalesced_aligned") {
        workload_coalesced_aligned(data,dut,tfp,100);
    } else if (workload == "coalesced_misaligned") {
        workload_coalesced_misaligned(data,dut,tfp,100,4);
    } else if (workload == "strided_k") {
        workload_strided_k(data,dut,tfp,100,4);
    } else if (workload == "reverse") {
        workload_reverse(data,dut,tfp,100);
    } else if (workload == "transpose") {
        workload_transpose(data,dut,tfp,256,256);
    } else if (workload == "tiled") {
        workload_tiled(data,dut,tfp,256,256);
    } else if (workload == "random_uniform") {
        workload_random_uniform(data, dut,tfp, 100, 3241,343);
    } else if (workload == "gather") {
        workload_gather(data,dut,tfp,100,3241,565,100,0.66);
    } else if (workload == "hotspot") {
        workload_hotspot(data,dut,tfp,100,3241,565);
    } else {
        std::cerr << "Unknown Workload: " << workload <<"\n";

        tfp->close();
        delete tfp;
        delete dut;
        return 1;
    }

    
    tfp->close();
    delete tfp;
    delete dut;
}

