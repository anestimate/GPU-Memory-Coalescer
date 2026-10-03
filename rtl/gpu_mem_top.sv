module gpu_mem_top import gpu_mem_pkg::*; (

    input logic clk,
    input logic rst_n,
    
    input logic req_valid,
    input [ADDR_BITS-1:0] lane_addr [NUM_LANES],
    input [NUM_LANES-1:0] active_mask,


    output logic req_ready,
    output logic mem_req,
    output logic [31:0] n_requests,
    output logic [31:0] n_hits,
    output logic [31:0] n_misses,
    output logic [31:0] n_evictions,
    output logic [31:0] n_mem_reqs,
    output [63:0] total_bytes,
    output logic wr_en,
    output logic [31:0] index_counter,
    output logic [INDEX_BITS- 1:0] fill_set [0:2047],
    output logic [WAY_BITS- 1:0] fill_way [0:2047],
    output logic [TAG_BITS- 1:0] fill_tag [0:2047],
    output logic in_ready,
    output logic out_valid,
    output logic out_last,
    output logic [NUM_LANES-1:0] out_lane_mask


);

    logic hit;
    logic [WAYS-1:0] hit_way;
    logic [WAYS-1:0] valid_bits;
    logic [TAG_BITS-1:0] victim_tag;
    logic [INDEX_BITS-1:0] wr_index;
    logic [TAG_BITS-1:0] wr_tag;
    logic [WAY_BITS-1:0] wr_way;
    logic [ADDR_BITS-1:0] response_line_addr;
    logic [TAG_BITS-1:0] rd_tag;
    logic [INDEX_BITS-1:0] rd_index;

    logic [INDEX_BITS-1:0] access_index;
    logic [WAY_BITS-1:0] access_way;
    logic access_en;
    logic mem_req_valid;
    logic mem_req_ready;
    logic response_path;
    logic [ADDR_BITS-1:0] mem_addr_address;
    logic [WAY_BITS-1:0] victim_way;


    logic [ADDR_BITS-OFFSET_BITS-1:0] coalesced_line_addr;


    assign mem_req = mem_req_valid;

    
    cache_ctrl ctrl (
        .clk(clk),
        .rst_n(rst_n),

        .hit(hit),
        .hit_way(hit_way),
        .valid_bits(valid_bits),
        .victim_tag(victim_tag),
        .mem_req_ready(mem_req_ready),
        .req_valid(out_valid),
        .req_address({coalesced_line_addr, {OFFSET_BITS{1'b0}}}),
        .stored_req_address(mem_addr_address),
        .req_ready(req_ready),
        .mem_req(mem_req_valid),
        .mem_ack(response_path),
        .wr_en(wr_en),
        .wr_index(wr_index),
        .wr_tag(wr_tag),
        .wr_way(wr_way),

        .rd_tag(rd_tag),
        .rd_index(rd_index),

        .n_requests(n_requests),
        .n_hits(n_hits),
        .n_misses(n_misses),
        .n_evictions(n_evictions),
        .n_mem_reqs(n_mem_reqs),
        .index_counter(index_counter),
        .fill_set(fill_set),
        .fill_way(fill_way),
        .fill_tag(fill_tag),

        
        .access_en(access_en),
        .access_index(access_index),
        .access_way(access_way)

    );

    tag_array u_tag_array (

        .clk(clk),
        .rst_n(rst_n),

        .rd_index(rd_index),
        .rd_tag(rd_tag),

        .hit_way(hit_way),
        .hit(hit),
        .valid_bits(valid_bits),
        .victim_tag(victim_tag),

        .wr_en(wr_en),
        .wr_index(wr_index),
        .wr_way(wr_way),
        .wr_tag(wr_tag),

        .inv_en(1'b0),
        .inv_index('0),
        .inv_way('0)

    );

    mem_model u_mem_model (
        .clk(clk),
        .rst_n(rst_n),
        .req_address(mem_addr_address),
        .req_valid(mem_req_valid),
        .req_ready(mem_req_ready),
        .response_line_addr(response_line_addr),
        .total_bytes(total_bytes),
        .response_valid(response_path)

    );

    replacement u_replacement (

        .clk(clk),
        .rst_n(rst_n),

        .victim_way(wr_way),
        .valid_bits(valid_bits),
        .access_en(access_en),
        .access_index(access_index),
        .access_way(access_way)

    );

    coalescer u_coalescer (

        .clk(clk),
        .rst_n(rst_n),

        .in_valid(req_valid),
        .active_mask(active_mask),
        .lane_addr(lane_addr),
        .out_ready(req_ready),


        .in_ready(in_ready),
        .out_valid(out_valid),
        .out_last(out_last),
        .out_lane_mask(out_lane_mask),
        .out_lane_addr(coalesced_line_addr)




    );

endmodule


