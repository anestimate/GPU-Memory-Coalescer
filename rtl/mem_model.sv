module mem_model import gpu_mem_pkg::*; (

    input clk,
    input rst_n,
    input [ADDR_BITS-1:0] req_address,
    input req_valid,

    output logic [ADDR_BITS-1:0] response_line_addr,
    output logic response_valid,
    output logic req_ready,
    output [63:0] total_bytes

);

    logic occupied [15:0];
    reg [ADDR_BITS-1:0] holding_register [15:0];
    reg [31:0] countdown [15:0];
    integer i;
    integer j;
    integer k;
    logic [31:0] valid_slot;
    logic [63:0] byte_counter;

  
    localparam MAX_OUTSTANDING = 16;

    always_ff @(posedge clk) begin
        if (!rst_n) begin
            for (i=0;i<MAX_OUTSTANDING;i=i+1) begin
                occupied[i] <= 0;
                countdown[i] <= 0;
                holding_register[i] <= 0;
            end
            byte_counter <=0;
        end

        if (req_ready && req_valid) begin
            holding_register[valid_slot] <= req_address;
            occupied[valid_slot] <= 1;
            countdown[valid_slot]<=0;
        end else begin

            response_valid <= 0;

            for (k=0;k<MAX_OUTSTANDING;k=k+1) begin

                countdown[k] <= countdown[k]+1;
                if ((countdown[k] == MEM_LATENCY+1) && occupied[k]) begin
                    response_line_addr <= holding_register[k];
                    response_valid <= 1;
                    byte_counter <= byte_counter +LINE_BYTES;
                    holding_register[k] <= 0;
                    countdown[k] <= 0;
                    occupied[k] <= 0;
                end
            end
        end
    end

   
    
    always_comb begin
        req_ready = 0;
        valid_slot = 0;
        for (j=0;j<MAX_OUTSTANDING;j=j+1) begin
            if (occupied[j] == 0) begin
                req_ready=1;
                valid_slot = j;
                break;
            end else begin
                req_ready = 0;
                valid_slot=0;
            end
        end
        
    end

    assign total_bytes = byte_counter;

endmodule




