module cache_ctrl import gpu_mem_pkg::*; (
    
    input clk,
    input rst_n,
    input hit,
    input [WAYS-1:0] hit_way,
    input [WAYS-1:0] valid_bits,
    input [TAG_BITS-1:0] victim_tag,
    input req_valid,
    input [ADDR_BITS-1:0] req_address,
    input mem_ack,
    input mem_req_ready,
    input logic [WAY_BITS-1:0] wr_way,

    output logic req_ready,
    output logic mem_req,
    output logic wr_en,
    output logic [INDEX_BITS-1:0] access_index,
    output logic [WAY_BITS-1:0] access_way,
    output logic access_en,
    output logic [INDEX_BITS-1:0] wr_index,
    output logic [TAG_BITS-1:0] wr_tag,
    output logic [TAG_BITS-1:0]  rd_tag,
    output logic [INDEX_BITS-1:0] rd_index,
    output logic [31:0] n_requests, 
    output logic [31:0] n_hits, 
    output logic [31:0] n_misses, 
    output logic [31:0] n_evictions,
    output logic [31:0] n_mem_reqs,
    output logic [ADDR_BITS-1:0] stored_req_address,
    output logic [31:0] index_counter,
    output logic [INDEX_BITS- 1:0] fill_set [0:2047],
    output logic [WAY_BITS- 1:0] fill_way [0:2047],
    output logic [TAG_BITS- 1:0] fill_tag [0:2047]


);

    typedef enum logic [1:0] {IDLE, LOOKUP, REFILL, FILL}  state_t;
    state_t state, next_state;
    integer i;
    logic [31:0] n_requestsTEMP, n_hitsTEMP, n_missesTEMP, n_evictionsTEMP, n_mem_reqsTEMP; 
    

    always_ff @(posedge clk) begin

        if (!rst_n) begin 
            state <= IDLE; 
            n_missesTEMP <= 0;
            n_requestsTEMP <= 0;
            n_hitsTEMP <= 0;
            n_evictionsTEMP <= 0;
            n_mem_reqsTEMP <= 0;
            stored_req_address <= 0;
            index_counter <=0;
        end else begin
        
            state <= next_state;
            if ((next_state == REFILL) && (state==LOOKUP)) begin
                n_missesTEMP <= n_missesTEMP +1;
            end        

            if ((req_valid) && (req_ready)) begin
                n_requestsTEMP <= n_requestsTEMP +1;
            end  

            if ((next_state == IDLE) && (state==LOOKUP)) begin
                n_hitsTEMP <= n_hitsTEMP+1;
            end    

            if (((next_state == IDLE) && (state==FILL)) && valid_bits[wr_way]) begin
                n_evictionsTEMP <= n_evictionsTEMP+1;
            end  

            if ((next_state == REFILL) && (state==LOOKUP)) begin
                n_mem_reqsTEMP <= n_mem_reqsTEMP+1;
            end   

            if (req_valid && (state == IDLE)) begin
                stored_req_address <= req_address;
            end

            if (wr_en) begin
                if (index_counter<2048) begin
                    fill_set[index_counter] <= wr_index;
                    fill_way[index_counter] <= wr_way;
                    fill_tag[index_counter] <= wr_tag;
                end
                index_counter <= index_counter+1;
            end

        end 
    end


    
    assign rd_tag = get_tag (stored_req_address);
    assign rd_index = get_index (stored_req_address);

    always_comb begin
        next_state = state;
        wr_en=0; 
        wr_tag=0;
        wr_index=0;

        access_en=0; 
        access_index=0;
        access_way = 0;

        mem_req=0;
        req_ready = 0;

        case (state)
            IDLE:   begin
                        req_ready = 1;
                        if (req_valid) begin
                            next_state = LOOKUP;
                        end
                    end
            LOOKUP: if (hit) begin
                        access_en = 1;
                        access_index = rd_index;
                        for (i=0;i< WAYS; i=i+1) begin
                            if (hit_way[i]) begin
                                access_way = i[WAY_BITS-1:0];
                            end
                        end
            
                        req_ready=0;
                        next_state = IDLE;
                        mem_req=0;
                    end else begin
                        access_en = 0;
                        req_ready=0;
                        next_state = REFILL;
                        mem_req = 1;
                        if (mem_req_ready) begin
                            next_state = REFILL; 
                        end else begin
                            next_state = LOOKUP;
                        end
                    end
            REFILL: if (mem_ack) begin
                        req_ready=0;
                        next_state = FILL;
                    end
            FILL:   begin
                        next_state = IDLE;
                        wr_en=1;
                        wr_tag=rd_tag;
                        wr_index=rd_index;

                        access_index = wr_index;
                        access_en = 1;
                        access_way = wr_way;
                    end
           
            default: begin next_state = IDLE; wr_en=0; end
        endcase
    end


    assign n_misses = n_missesTEMP;
    assign n_requests = n_requestsTEMP;
    assign n_hits = n_hitsTEMP;
    assign n_evictions = n_evictionsTEMP;
    assign n_mem_reqs = n_mem_reqsTEMP;

    `include "assertions.svh"

endmodule



