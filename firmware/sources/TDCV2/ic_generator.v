/* 
       _________   ________     ________       
      / U OF M  \ | LSA    \   / Physics\
      \__ATLAS__/ |   ___   | |   ______/
         |   |    |  |   \  | |  |
         |   |    |  |___/  | |  \______     
         |   |    |         | |         \
         \___/    |________/   \________/
*/  

// Author : Yuxiang Guo  gyuxiang@umich.edu
// File   : ic_generator.v
// Create : 2021-06-23 22:35:39
// Revise : 2021-06-24 05:03:45
// Editor : sublime text3, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------


module ic_generator
(
    input clk_40,
    input rst,
    input ram_write,
    input [7:0] ram_write_data,
    input [8:0] ram_write_address,

    output [1:0] ic,


    input lpgbt_write,
    input [63:0] lpgbt_write_data

);


wire [7:0] ram_read_data;
reg [8:0] ram_read_address;


lpGBT_ram lpGBT_ram_inst (
  .clka(clk_40),    // input wire clka
  .wea(ram_write),      // input wire [0 : 0] wea
  .addra(ram_write_address),  // input wire [8 : 0] addra
  .dina(ram_write_data),    // input wire [7 : 0] dina
  .clkb(clk_40),    // input wire clkb
  .addrb(ram_read_address),  // input wire [8 : 0] addrb
  .doutb(ram_read_data)  // output wire [7 : 0] doutb
);


reg [5:0]   state;
reg [5:0]   nextstate;



localparam IDLE         =6'b000001;
localparam START_IC     =6'b000010;
localparam WRITE_IC     =6'b000100;


localparam IDLE_BIT     =1;
localparam START_IC_BIT =2;
localparam WRITE_IC_BIT =3;

localparam [7:0] frame_delimiter = 8'b01111110;

// reg [1:0] tick;
reg [79:0] lpgbt_write_data_reg;


// lpgbt_write_data
// [63:56]  lpGBT address (7 bits) + R/W bit = 0
// [55:48]  command
// [47:40]  Number of data words n[7:0]
// [39:32]  Number of data words n[15:8]
// [31:24]  memory address [7:0]
// [23:26]  memory address [15:8]
// [15:8]   data
// [7:0]    Parity_word

reg [15:0]  data_words;
reg [5:0] counter;


always @(posedge clk_40) begin
    if (rst) begin
        // reset
        state <= IDLE;
    end else begin
        state <= nextstate;
    end
end

// always @(posedge clk_40) begin
//     if (rst|state[IDLE_BIT]) begin
//         // reset
//         tick <= 2'b0;
//     end else begin
//         tick <= tick + 1'b1;
//     end
// end

always @(posedge clk_40) begin
    if (rst) begin
        // reset
        lpgbt_write_data_reg <= 'b0;
        counter <= 'b0;
    end else if (nextstate[START_IC_BIT]) begin
        lpgbt_write_data_reg <= {frame_delimiter,lpgbt_write_data,frame_delimiter};
        counter <= 6'd39;
    end else if (state[WRITE_IC_BIT]) begin
        lpgbt_write_data_reg <= {lpgbt_write_data_reg[77:0],lpgbt_write_data_reg[79:78]};
        counter <= counter - 1'b1;
    end
end

assign ic = lpgbt_write_data_reg[79:78];

// always @(posedge clk_40) begin
//     if (rst) begin
//         // reset
//         data_words       <= 'b0;
//         ram_read_address <= 'b0;
//     end else if (nextstate[START_IC_BIT]) begin
//         data_words       <= {lpgbt_write_data_reg[24:16]}-1'b1;
//         ram_read_address <= {lpgbt_write_data_reg[39:32],lpgbt_write_data_reg[47:40]};
//     end else if (state[RAM_READ]&(&tick)) begin
//         data_words       <= data_words -1'b1;
//         ram_read_address <= ram_read_address +1'b1;
//     end
// end


always @(*) begin
    case(state)
        IDLE: 
            nextstate = lpgbt_write ? START_IC : IDLE;
        START_IC:
            nextstate = WRITE_IC;
        WRITE_IC:
            nextstate = (|counter)? WRITE_IC : IDLE;
        default: nextstate = IDLE;
    endcase
end


endmodule