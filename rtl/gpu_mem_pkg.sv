`ifndef CFG_WAYS
`define CFG_WAYS 4
`endif
`ifndef CFG_LINE_BYTES
`define CFG_LINE_BYTES 64
`endif
`ifndef CFG_CACHE_BYTES
`define CFG_CACHE_BYTES 8192
`endif


package gpu_mem_pkg;

    localparam NUM_LANES = 32;
    localparam ADDR_BITS = 32;
    localparam LINE_BYTES = `CFG_LINE_BYTES;
    localparam CACHE_BYTES = `CFG_CACHE_BYTES;
    localparam WAYS = `CFG_WAYS;
    localparam MEM_LATENCY = 100;
    localparam ACCESS_BYTES = 4;


    localparam OFFSET_BITS = $clog2(LINE_BYTES);
    localparam NUM_SETS = CACHE_BYTES / LINE_BYTES /WAYS;
    localparam INDEX_BITS = $clog2(NUM_SETS);
    localparam TAG_BITS = ADDR_BITS - INDEX_BITS - OFFSET_BITS;
    localparam WAY_BITS = (WAYS > 1) ? $clog2(WAYS) : 1;


    function automatic [TAG_BITS-1:0] get_tag ;
        input [ADDR_BITS-1:0] array;

        return array[ADDR_BITS-1:OFFSET_BITS+INDEX_BITS];

    endfunction


    function automatic [INDEX_BITS-1:0] get_index;
        input [ADDR_BITS-1:0] array;

        return array[OFFSET_BITS+INDEX_BITS-1:OFFSET_BITS];

    endfunction 


    function automatic [OFFSET_BITS-1:0]get_offset;
        input [ADDR_BITS-1:0] array;
    
        return array[OFFSET_BITS-1:0];

    endfunction


    function automatic [ADDR_BITS-OFFSET_BITS-1:0]line_addr;
        input [ADDR_BITS-1:0] array;
    
        return array[ADDR_BITS-1:OFFSET_BITS];

    endfunction

endpackage


