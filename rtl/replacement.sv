module replacement import gpu_mem_pkg::*;(

    input clk,
    input rst_n,
    input logic [INDEX_BITS-1:0] access_index,
    input logic [WAY_BITS-1:0] access_way,
    input logic access_en,
    input logic [WAYS-1:0] valid_bits,


    output [WAY_BITS-1:0] victim_way


);

    integer counteri;
    integer counterj;
    integer counterk;
    integer counter;
    logic [WAY_BITS-1:0] age[NUM_SETS][WAYS];
    localparam logic [WAY_BITS-1:0] MAX_AGE = WAY_BITS'(WAYS - 1);

    always_ff @(posedge clk) begin

        if (!rst_n) begin

                for (counterj=0; counterj<NUM_SETS; counterj=counterj+1) begin
                    for (counterk = 0; counterk<WAYS; counterk =counterk+1) begin
                        age[counterj][counterk] <= counterk[WAY_BITS-1:0];
                    end
                end

        end

        if (access_en) begin

            for (counteri=0;counteri<WAYS;counteri = counteri+1) begin

                if (age[access_index][counteri] < age[access_index][access_way]) begin
                    age[access_index][counteri] <= age[access_index][counteri] +1;
                end else if (access_way == counteri[WAY_BITS-1:0]) begin
                    age[access_index][counteri] <= 0;
                end

            end
        end
    end 


    always_comb begin

        victim_way = 0;

        for (counter = 0; counter<WAYS; counter = counter+1) begin

            if (valid_bits[counter] == 0) begin
                victim_way = counter[WAY_BITS-1:0];
                break;
            end else if (age[access_index][counter] == MAX_AGE) begin
                victim_way = counter[WAY_BITS-1:0];
            end

        end  
    end

endmodule



