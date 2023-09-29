`timescale 1ns / 1ps
`include "definitions.sv"

module lpgbtcsm_fpga_top (
    input                   sys_clk_p,        // 160MHz from LpGBT elink 
    input                   sys_clk_n,        // 

    input elink_TTC_in_p,   //TTC from elink
    input elink_TTC_in_n,

    input     [`NUM_DEV:0]  tdo,              // JTAG Test Data Out.
    output    [`NUM_DEV:0]  tdi,              // JTAG Test Data In.
    output    [`NUM_DEV:0]  tck,              // JTAG Test Clock.
    output    [`NUM_DEV:0]  tms,              // JTAG Test Mode Select.

    output   rst_AMT_MB, // AMT_RESET TO AMT PIN 1 RESETB
    output   lock,
    output   config_done,

    // output    [`NUM_DEV:0]  calibre_p,        // Calibration P.
    // output    [`NUM_DEV:0]  calibre_n,         // Calibration N.
    output    [`NUM_DEV:0]  mezz_enc_out_p,        // Calibration P.
    output    [`NUM_DEV:0]  mezz_enc_out_n         // Calibration N.
);
  
wire enable_loop_TRST;
wire loop_TRST;

// Clocks.
wire  clk_40MHz;    // Smart Assignment Logic Clock.
// TTC
wire elink_ttc_org;
wire [`NUM_DEV:0] mezz_enc;
wire [`NUM_DEV:0] tck_inner;


// JTAG. 

wire jtag_master_tck;
wire jtag_master_tms;
wire jtag_master_tdi;
wire jtag_master_tdo;
wire start_action_tdc, start_action_asd;

wire [4:0] JTAG_instr_mezz;
(* dont_touch = "true" *)wire [255:0] JTAG_bits_mezz;   //longest single jtag data chain: ASD 167
wire [4095:0] JTAG_readback;  //longest jtag data chain: ASD 167*18=3006
(* dont_touch = "true" *) wire [7:0] JTAG_data_length_mezz;  //length maximum 255
wire JTAG_busy;


// Smart Assignment.
(* dont_touch = "true" *) wire  [4:0] num_active_ports;   // Unsigned integer value of how many nodes there are on the JTAG chain.
(* dont_touch = "true" *) wire  sa_done;                 // Smart assignment done = high, not done = low.
(* dont_touch = "true" *) wire  [`NUM_DEV:0]  active_chains;
(* dont_touch = "true" *) wire  tdi_tdo_equal;
(* dont_touch = "true" *) wire [5:0] chain_success;

//vio output
wire  rst_global;   // Resets every logic element.
wire  [11:0] clk_div;  //tck frequency = input_clock/2/(1+clk_div), maximum 20MHz
wire  enable_vio_config;
wire  TDC_config;
wire  ASD_config;
wire  read_status0;
wire  read_status1;
wire  [`NUM_DEV:0] mezz_enable_vio;
wire  [`NUM_DEV:0] tdo_masked;

wire tdc_config_done;
wire asd_config_done;
wire TDC_loop_start;
wire ASD_loop_start;

wire config_twice;





  
  // Assignments.

//assign rst_AMT_MB = 1'b1; 

assign tdo_masked = mezz_enable_vio & tdo;
assign config_done = sa_done&(|active_chains)&(&chain_success);
assign tck = (~sa_done)|(JTAG_busy)?tck_inner:'b0;
assign rst_AMT_MB = enable_loop_TRST?loop_TRST:rst_vio;
 
clk_gen clk_gen_inst(
    // Clock out ports
    .clk_out1(clk_40MHz),     // output clk_out1
    // Status and control signals
    .locked(lock),       // output locked
    // Clock in ports
    .clk_in1_p(sys_clk_p),    // input clk_in1_p
    .clk_in1_n(sys_clk_n));    // input clk_in1_n

 
  // Smart Assignment.
smart_assignment sa (
    .rst(rst_global),                     // Asynchronous reset.
    .clk(clk_40MHz),                      // Logic Clock (40MHz).
    .jtag_tck(jtag_master_tck),                  // JTAG Clock.
    .jtag_tms(jtag_master_tms),                  // JTAG TMS.
    .jtag_tdi(jtag_master_tdi),                  // JTAG Chain single pin input.
    .tdo_chain(tdo_masked),                      // JTAG TDO fan-in.
    .tdi_chain(tdi),                      // JTAG TDI fan-out.
    .jtag_tdo(jtag_master_tdo),                  // JTAG Chain single pin output.
    .tms_chain(tms),                      // JTAG TMS fan-out.
    .tck_chain(tck_inner),                      // JTAG TCK fan-out.
    .num_active_ports(num_active_ports),  // Unsigned integer value of how many nodes there are on the JTAG chain.
    .active_chains   (active_chains),
    .done(sa_done)                        // Smart assignment has completed and the chain is now usable.
);
  
//   // JTAG Controller.
// jtag_control #(
//     .inst_len(`IR_LEN),
//     .num_data_len_bits(`DATA_LEN_BITS),
//     .num_chain_len_bits(17_BITS + 1)
//     ) jc (
//     .clk(twice_jtag_tck),
//     .en(sa_done),
//     .busy(master_busy),
//     .en_master(enb_master),
//     .inst_reg(JTAG_instr_mezz),
//     .data_len(jtag_control_data_len),
//     .data_reg(jtag_control_data_reg)
// );




JTAG_master #(
    .DATA_BIT(8),
    .INSTR_LENGTH(5),
    .DEVICE_BIT(5)
) inst_JTAG_master (
    .clk                (clk_40MHz),
    .rst                (rst_global),
    .tck                (jtag_master_tck),
    .tms                (jtag_master_tms),
    .tdi                (jtag_master_tdi),
    .tdo                (jtag_master_tdo),
    .start_action_tdc   (start_action_tdc),
    .start_action_asd   (start_action_asd),
    .clk_div            (clk_div),
    .JTAG_bits          (JTAG_bits_mezz),
    .bit_length         (JTAG_data_length_mezz),
    .JTAG_inst          (JTAG_instr_mezz),
    .device_count       (num_active_ports),
    .JTAG_readback      (JTAG_readback),
    .JTAG_busy          (JTAG_busy),
    .tdi_tdo_equal      (tdi_tdo_equal)
);


Mezz_config #(
    .DATA_BIT(8)
) inst_Mezz_config (
    .clk                   (clk_40MHz),
    .rst                   (rst_global),
    .JTAG_busy             (JTAG_busy),
    .enable_vio_config     (enable_vio_config),
    .TDC_config            (TDC_config|TDC_loop_start),
    .ASD_config            (ASD_config|ASD_loop_start),
    .tdi_tdo_equal         (tdi_tdo_equal),
    .start_action_status0  (read_status0),
    .start_action_status1  (read_status1),
    .config_twice          (config_twice),

    //output ports
    .start_action_tdc      (start_action_tdc),
    .start_action_asd      (start_action_asd),
    .tdc_config_done       (tdc_config_done),
    .asd_config_done       (asd_config_done),

    .chain_success         (chain_success),

    .JTAG_instr_mezz       (JTAG_instr_mezz),
    .JTAG_data_length_mezz (JTAG_data_length_mezz),
    .JTAG_bits_mezz        (JTAG_bits_mezz)
);

config_loop config_loop_inst(
    .clk             (clk_40MHz),
    .rst             (rst_global),
    .tdc_config_done (tdc_config_done),
    .asd_config_done (asd_config_done),
    .chain_success   (chain_success),
    .TDC_loop_start  (TDC_loop_start),
    .ASD_loop_start  (ASD_loop_start),
    .loop_TRST       (loop_TRST),
    .enable_loop_TRST(enable_loop_TRST)
);



vio_top vio_top_inst (
    .clk(clk_40MHz),                // input wire clk
    .probe_in0(num_active_ports),    // input wire [4 : 0] probe_in0
    .probe_in1(sa_done),    // input wire [0 : 0] probe_in1
    .probe_in2(lock),    // input wire [0 : 0] probe_in2
    .probe_in3(active_chains), // input wire [17 : 0] probe_in3
    .probe_in4(JTAG_data_length_mezz), // input wire [7 : 0] probe_in4
    .probe_in5(JTAG_readback[4095:3840]), // input wire [255 : 0] probe_in5
    .probe_in6(JTAG_instr_mezz), // input wire [4 : 0] probe_in6
    .probe_in7(config_done), // input wire [0 : 0] probe_in7
    .probe_in8(JTAG_bits_mezz), // input wire [255 : 0] probe_in8
    .probe_in9(tdi_tdo_equal), // input wire [0 : 0] probe_in9
    .probe_in10(chain_success), // input wire [5 : 0] probe_in9    

    .probe_out0(rst_global),  // output wire [0 : 0] probe_out0
    .probe_out1(clk_div),  // output wire [11 : 0] probe_out1
    .probe_out2(enable_vio_config),  // output wire [0 : 0] probe_out2
    .probe_out3(TDC_config),  // output wire [0 : 0] probe_out3
    .probe_out4(ASD_config),  // output wire [0 : 0] probe_out4
    .probe_out5(mezz_enable_vio),  // output wire [17 : 0] probe_out5
    .probe_out6(rst_vio),  // output wire [0 : 0] probe_out6
    .probe_out7(read_status0),  // output wire [0 : 0] probe_out7
    .probe_out8(read_status1),  // output wire [0 : 0] probe_out8
    .probe_out9(config_twice)  // output wire [0 : 0] probe_out8
);  

ila_top ila_top_inst (
    .clk(clk_40MHz), // input wire clk

    .probe0(num_active_ports), // input wire [4:0]  probe0  
    .probe1(sa_done), // input wire [0:0]  probe1 
    .probe2(lock), // input wire [0:0]  probe2 
    .probe3(active_chains), // input wire [17:0]  probe3 
    .probe4(JTAG_busy), // input wire [0:0]  probe4 
    .probe5(tdi_tdo_equal), // input wire [0:0]  probe5 
    .probe6(chain_success), // input wire [5:0]  probe6 
    .probe7(elink_ttc_org), // input wire [0:0]  probe7
    .probe8(tdo), // input wire [17:0]  probe8
    .probe9(tdi), // input wire [17:0]  probe9
    .probe10(tck), // input wire [17:0]  probe10
    .probe11(tms) // input wire [17:0]  probe11
);

//assign elink_ttc_org = 1'b0;
IBUFDS 
#(.IOSTANDARD("LVDS_25"),
  .DIFF_TERM(1),
  .IBUF_LOW_PWR(1))
    IBUFDS_inst
       (.O  (elink_ttc_org),
        .I  (elink_TTC_in_p),
        .IB (elink_TTC_in_n));

generate
    genvar i;
    for (i = 0; i <= `NUM_DEV; i = i + 1) begin:enc_obuf
        assign mezz_enc[i] = elink_ttc_org;
        OBUFDS 
        #(.IOSTANDARD("DEFAULT"),
          .SLEW("SLOW"))
            OBUFDS_inst
            (.O  (mezz_enc_out_p[i]),
            .OB  (mezz_enc_out_n[i]),
            .I   (mezz_enc[i]));
    end   
endgenerate



endmodule
