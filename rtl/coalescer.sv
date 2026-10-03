module coalescer import gpu_mem_pkg::*; (

    input clk,
    input rst_n,

    input in_valid,
    input [NUM_LANES-1:0] active_mask,
    input [ADDR_BITS-1:0] lane_addr [NUM_LANES],
    input out_ready,

    output logic in_ready,
    output logic out_valid,
    output logic out_last,
    output logic [NUM_LANES-1:0] out_lane_mask,
    output logic [ADDR_BITS-OFFSET_BITS-1:0] out_lane_addr


); 

    integer i;
    integer j;
    integer k;
    integer counter;
    integer counterTWO;

    typedef enum logic [1:0] {IDLE, UPDATE}  state_t;    
    state_t state, next_state;

    logic [NUM_LANES-1:0] pending;
    logic [NUM_LANES-1:0] match_mask;
    logic [ADDR_BITS-OFFSET_BITS-1:0] target [NUM_LANES];
    logic [ADDR_BITS-OFFSET_BITS-1:0] master_target;
    logic [NUM_LANES-1:0] pending_after_match;
    logic [ADDR_BITS-1:0] captured_addr [NUM_LANES];
    

    always_ff @(posedge clk) begin

        if (!rst_n) begin
            state <= IDLE;
            pending <= 0;

            for (i=0; i<NUM_LANES;i=i+1) begin
                captured_addr[i] <= '0;
            end

        end else begin
            state <= next_state;

            if (state== IDLE && in_valid && in_ready) begin
                pending <= active_mask;
                for (k=0; k<NUM_LANES;k=k+1) begin
                    captured_addr[k] <= lane_addr[k];
                end
            end else if (state == UPDATE && out_valid && out_ready) begin
                pending <= pending_after_match;
            end
        end

    end

    always_comb begin

        next_state = state;
        in_ready =0;
        out_valid = 0;
        out_last = 0;
        out_lane_mask ='0;
        out_lane_addr = '0;
        match_mask='0;
        master_target = 0;
        pending_after_match = pending;


        for (counterTWO=0;counterTWO<NUM_LANES;counterTWO=counterTWO+1) begin
            target[counterTWO] = captured_addr[counterTWO][ADDR_BITS-1:OFFSET_BITS];
        end

        case (state)
            IDLE: begin

                in_ready = 1;
            
                if (in_valid && in_ready) begin
                    if (active_mask == 0) begin
                        next_state = IDLE;
                    end else begin
                        next_state = UPDATE;
                    end
                end

                end
            UPDATE: begin 

                    in_ready = 0;
                    out_valid = 0;
                    for (counter=NUM_LANES-1;counter>=0;counter=counter-1) begin
                        if (pending[counter] == 1) begin
                            master_target = target[counter];
                        end
                    end
                            
                    for (j=0;j<NUM_LANES;j=j+1) begin
                        if (master_target == target[j] && pending[j] == 1) begin
                            match_mask[j] = 1;
                        end
                    end

                    pending_after_match = pending & ~match_mask;

                    if (pending != 0) begin
                        out_valid = 1;
                        out_lane_addr = master_target;
                        out_last = (pending_after_match == 0);

                    end

                    if (out_valid && out_ready) begin
                        if ((pending & ~match_mask) ==0)  begin
                            next_state = IDLE;
                            out_last = 1;
                        end else begin
                            next_state = UPDATE;
                        end
                    end
                    
                    end
            default: begin next_state = IDLE; end 
            
        endcase

        out_lane_mask = match_mask;
    end


endmodule





