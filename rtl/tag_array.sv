//Remembers every tag and valid bits, looks up whether tags exist and returns hit and miss.
//Updates when new line arrives.



module tag_array import gpu_mem_pkg::*; (

    input clk,
    input rst_n,
    input [INDEX_BITS-1:0] rd_index,
    input [TAG_BITS-1:0] rd_tag,

    input wr_en,
    input [INDEX_BITS-1:0] wr_index,
    input [WAY_BITS-1:0] wr_way,
    input [TAG_BITS-1:0] wr_tag,
    input inv_en,
    input [INDEX_BITS-1:0] inv_index,
    input [WAY_BITS-1:0] inv_way,


    output [WAYS-1:0] valid_bits,
    output [TAG_BITS-1:0] victim_tag,
    output [WAYS-1:0] hit_way,
    output hit

);

    logic [TAG_BITS-1:0] tags[NUM_SETS][WAYS];
    logic [0:0] valid[NUM_SETS][WAYS];
    logic [TAG_BITS-1:0] victim_tagTEMP;
    logic [WAYS-1:0] hit_wayTEMP;

    genvar i,p;
    integer j,k;


    generate


        always_ff @(posedge clk) begin
            if (!rst_n) begin
                    for (j=0; j<NUM_SETS; j=j+1) begin
                        for (k = 0; k<WAYS; k =k+1) begin
                            tags[j][k] <= 0;
                            valid[j][k] <= 0;
                        end
                    end
            end
        end

    endgenerate

    always_ff @(posedge clk) begin
        if (inv_en) begin
            valid[inv_index][inv_way] <= 0;
        end 
    end

    generate

        for (i=0; i< WAYS; i = i+1) begin

            always_comb begin
                if ((valid[rd_index][i]) && (tags[rd_index][i] == rd_tag)) begin
                    hit_wayTEMP[i] = 1;
                end else begin
                    hit_wayTEMP[i] = 0;
                end
            end

        end

    endgenerate

    assign hit_way = hit_wayTEMP;
    assign hit = |hit_way;

    always_ff @(posedge clk) begin
        if (wr_en) begin
            if (valid[wr_index][wr_way]) begin
                victim_tagTEMP <= tags[wr_index][wr_way];
            end
            tags[wr_index][wr_way] <= wr_tag;
            valid[wr_index][wr_way] <= 1;
        end 
    end

    assign victim_tag = victim_tagTEMP;

    generate

        for (p=0; p< WAYS; p= p+1) begin

            always_comb begin
                if (valid[rd_index][p]) begin
                    valid_bits[p] = 1;
                end else begin
                    valid_bits[p] = 0;
                end
            end

        end

    endgenerate

endmodule


