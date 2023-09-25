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
// File   : xlx_ku_mgt_10g24_2uplink.v
// Create : 2023-07-03 22:32:23
// Revise : 2023-07-07 01:12:23
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------



module xlx_ku_mgt_10g24_2uplink
    #(parameter UPLINKCOUNT = 2,
                CLKREFCOUNT = 1
    )

(
    input MGT_REFCLK_i,
    input MGT_FREEDRPCLK_i,
             
    output [1:0] MGT_RXUSRCLK_o,
    output [1:0] MGT_TXUSRCLK_o,
       
    // -- Resets      --
    input   MGT_TXRESET_i,
    input   MGT_RXRESET_i,
       
    // -- Control     --
    input [1:0]  MGT_TXPolarity_i,
    input [1:0]  MGT_RXPolarity_i,
       
    input [1:0]  MGT_RXSlide_i,

    input [1:0]  MGT_ENTXCALIBIN_i,
    input [13:0]  MGT_TXCALIB_i,
                    
    // -- Status      --
    output [1:0]  MGT_TXREADY_o,
    output [1:0]  MGT_RXREADY_o,
    output [1:0]  MGT_TX_ALIGNED_o,
    output [13:0]  MGT_TX_PIPHASE_o,

    // -- Data         --
    input [63:0]  MGT_USRWORD_i,
    output reg [63:0]  MGT_USRWORD_o,

    // -- Serial intf.  --
    input [1:0]  RXn_i,
    input [1:0]  RXp_i,
    output [1:0]  TXn_o,
    output [1:0]  TXp_o 
); 

wire rx_reset_done;
wire [63:0] MGT_USRWORD;
wire gtwiz_buffbypass_rx_reset_in;
wire gtrefclk00_in;
// wire [CLKREFCOUNT-1:0] qpll0outclk_out;
// wire [CLKREFCOUNT-1:0] qpll0outrefclk_out;
wire [1:0] rx_wordclk_sig;
wire [1:0] rxoutclk_sig;

wire [1:0] drpclk_in;

wire [17:0] drpaddr_toip;
wire [31:0] drpdi_toip;
wire [1:0] drpen_toip;
wire [1:0] drpwe_toip;
wire [31:0] drpdo;

wire [17:0] drpaddr_ibert;
wire [31:0] drpdi_ibert;
wire [1:0] drpen_ibert;
wire [1:0] drpwe_ibert;

wire [17:0] drpaddr;
wire [31:0] drpdi;
wire [1:0] drpen;
wire [1:0] drpwe;


wire [1:0] rxprbscntreset;
wire [3:0] rxprbssel;
wire [1:0] rxusrclk_in;
wire [1:0] rxusrclk2_in;

wire [1:0] txpippmen;
wire [1:0] txpippmovrden;
wire [1:0] txpippmpd;
wire [1:0] txpippmsel;
wire [4:0] txpippmstepsize;
wire [1:0] txusrclk_in;
wire [1:0] txusrclk2_in;
wire [1:0] tx_wordclk_sig;


wire [1:0] drprdy;
wire [1:0] rxpmaresetdone;
wire [1:0] rxprbserr;
wire [1:0] rxprbslocked;

wire [3:0] txbufstatus;
wire [1:0] txoutclk_sig;
wire [1:0] txpmaresetdone;

wire rxfsm_reset_done;
wire tx_reset_done;
wire [1:0] MGT_TX_ALIGNED;
wire [1:0] MGT_TX_ALIGNED_sync;
wire [1:0] tx_reset_hptd_ip;
wire [1:0] tx_reset_hptd_ip_sync;
wire [1:0] gtwiz_userclk_tx_reset_int;
wire [1:0] gtwiz_userclk_rx_reset_int;
wire [1:0] inv_gtwiz_userclk_tx_reset_int;
wire [1:0] inv_gtwiz_userclk_rx_reset_int;
reg [1:0] gtwiz_userclk_tx_active_int;
reg [1:0] gtwiz_userclk_rx_active_int;
wire [1:0] rxBuffBypassRst;

 
xlx_ku_mgt_ip_reset_synchronizer resetDoneSynch_rx(
   .clk_in(MGT_FREEDRPCLK_i),
   .rst_in(|rxBuffBypassRst), //reset from any uplink
   .rst_out(gtwiz_buffbypass_rx_reset_in)
);

genvar i;
for (i = 0; i < 1; i=i+1) begin
    assign drpclk_in[i]                      = MGT_FREEDRPCLK_i;
    assign rxusrclk_in[i]                    = rx_wordclk_sig[0];
    assign rxusrclk2_in[i]                   = rx_wordclk_sig[0];
    assign txusrclk_in[i]                    = tx_wordclk_sig[0];
    assign txusrclk2_in[i]                   = tx_wordclk_sig[0];
    assign drpaddr_toip[(i+1)*9-1:i*9]       = drpen[i]==1 ? drpaddr[(i+1)*9-1:i*9] : drpaddr_ibert[(i+1)*9-1:i*9];
    assign drpen_toip[i]                     = drpen[i]==1 ? drpen[i]               : drpen_ibert[i];
    assign drpdi_toip[(i+1)*16-1:i*16]       = drpen[i]==1 ? drpdi[(i+1)*16-1:i*16] : drpdi_ibert[(i+1)*16-1:i*16];
    assign drpwe_toip[i]                     = drpen[i]==1 ? drpwe[i]               : drpwe_ibert[i];
    assign MGT_TXREADY_o[i]                  = tx_reset_done && MGT_TX_ALIGNED_sync[i];
    assign tx_reset_hptd_ip[i]               = ~tx_reset_done;
    assign MGT_RXREADY_o[i]                  = rx_reset_done && rxfsm_reset_done;
    assign gtwiz_userclk_tx_reset_int[i]     = ~txpmaresetdone[i];
    assign gtwiz_userclk_rx_reset_int[i]     = ~rxpmaresetdone[i]; 
    assign inv_gtwiz_userclk_tx_reset_int[i] = ~gtwiz_userclk_tx_reset_int[i];
    assign inv_gtwiz_userclk_rx_reset_int[i] = ~gtwiz_userclk_rx_reset_int[i];
    assign rxBuffBypassRst[i]= ~(gtwiz_userclk_rx_active_int[i]) || (!tx_reset_done && !(MGT_TX_ALIGNED[i]));

    xlx_ku_mgt_ip_reset_synchronizer txAlignedSynch_tx(
        .clk_in(tx_wordclk_sig[i]),
        .rst_in(MGT_TX_ALIGNED[i]),
        .rst_out(MGT_TX_ALIGNED_sync[i])
    );

    xlx_ku_mgt_ip_reset_synchronizer txPhaseAlignerResetSynch_sys(
       .clk_in(MGT_FREEDRPCLK_i),
       .rst_in(tx_reset_hptd_ip[i]),
       .rst_out(tx_reset_hptd_ip_sync[i])
    );


    BUFG_GT rxWordClkBuf_inst(
        .O(rx_wordclk_sig[i]),
        .I(rxoutclk_sig[i]),
        .CE(inv_gtwiz_userclk_rx_reset_int[i]),
        .DIV('b0),
        .CLR('b0),
        .CLRMASK('b0),
        .CEMASK('b0)
    );
    BUFG_GT txWordClkBuf_inst(
        .O(tx_wordclk_sig[i]),
        .I(txoutclk_sig[i]),
        .CE(inv_gtwiz_userclk_tx_reset_int[i]),
        .DIV('b0),
        .CLR('b0),
        .CLRMASK('b0),
        .CEMASK('b0)
    );

    always @(posedge gtwiz_userclk_tx_reset_int[i] or posedge tx_wordclk_sig[i]) begin : activetxUsrClk_proc
        if (gtwiz_userclk_tx_reset_int[i]) 
            gtwiz_userclk_tx_active_int[i] <= 1'b0;
        else
            gtwiz_userclk_tx_active_int[i] <= 1'b1;
    end

    always @(posedge gtwiz_userclk_rx_reset_int[i] or posedge rx_wordclk_sig[i]) begin : activerxUsrClk_proc
        if (gtwiz_userclk_rx_reset_int[i]) 
            gtwiz_userclk_rx_active_int[i] <= 1'b0;
        else
            gtwiz_userclk_rx_active_int[i] <= 1'b1;
    end

    always @(negedge rx_reset_done or posedge rx_wordclk_sig[i]) begin : rxWordPipeline_proc
        if (~rx_reset_done) 
            MGT_USRWORD_o[(i+1)*32-1:i*32] <= 'b0;
        else
            MGT_USRWORD_o[(i+1)*32-1:i*32] <= MGT_USRWORD[(i+1)*32-1:i*32];
    end
end


assign MGT_TX_ALIGNED_o   = MGT_TX_ALIGNED_sync;

assign rx_reset_sig = MGT_RXRESET_i || !(tx_reset_done && (&MGT_TX_ALIGNED));//MGT_RXRESET_i; -- or not(tx_reset_done and MGT_TX_ALIGNED_m); -- and txfsm_reset_done);
assign tx_reset_sig = MGT_TXRESET_i; 

assign MGT_RXUSRCLK_o = rx_wordclk_sig;   
assign MGT_TXUSRCLK_o = tx_wordclk_sig;


genvar j;
for (j = 0; j < CLKREFCOUNT-1; j=j+1) begin
    assign gtrefclk00_in[j] = MGT_REFCLK_i;
end

xlx_ku_mgt_ip_10g24 xlx_ku_mgt_std_inst (
    .gtwiz_userclk_tx_active_in(&gtwiz_userclk_tx_active_int),                  // input wire [0 : 0] gtwiz_userclk_tx_active_in
    .gtwiz_userclk_rx_active_in(&gtwiz_userclk_rx_active_int),                  // input wire [0 : 0] gtwiz_userclk_rx_active_in
    .gtwiz_buffbypass_rx_reset_in(gtwiz_buffbypass_rx_reset_in),              // input wire [0 : 0] gtwiz_buffbypass_rx_reset_in
    .gtwiz_buffbypass_rx_start_user_in(1'b0),    // input wire [0 : 0] gtwiz_buffbypass_rx_start_user_in
    .gtwiz_buffbypass_rx_done_out(rxfsm_reset_done),              // output wire [0 : 0] gtwiz_buffbypass_rx_done_out
    .gtwiz_buffbypass_rx_error_out(),            // output wire [0 : 0] gtwiz_buffbypass_rx_error_out
    .gtwiz_reset_clk_freerun_in(MGT_FREEDRPCLK_i),                  // input wire [0 : 0] gtwiz_reset_clk_freerun_in
    .gtwiz_reset_all_in(1'b0),                                  // input wire [0 : 0] gtwiz_reset_all_in
    .gtwiz_reset_tx_pll_and_datapath_in(tx_reset_sig),  // input wire [0 : 0] gtwiz_reset_tx_pll_and_datapath_in
    .gtwiz_reset_tx_datapath_in(1'b0),                  // input wire [0 : 0] gtwiz_reset_tx_datapath_in
    .gtwiz_reset_rx_pll_and_datapath_in(1'b0),  // input wire [0 : 0] gtwiz_reset_rx_pll_and_datapath_in
    .gtwiz_reset_rx_datapath_in(rx_reset_sig),                  // input wire [0 : 0] gtwiz_reset_rx_datapath_in
    .gtwiz_reset_rx_cdr_stable_out(),            // output wire [0 : 0] gtwiz_reset_rx_cdr_stable_out
    .gtwiz_reset_tx_done_out(tx_reset_done),                        // output wire [0 : 0] gtwiz_reset_tx_done_out
    .gtwiz_reset_rx_done_out(rx_reset_done),                        // output wire [0 : 0] gtwiz_reset_rx_done_out
    
    .gtwiz_userdata_tx_in(MGT_USRWORD_i),                              // input wire [63 : 0] gtwiz_userdata_tx_in
    .gtwiz_userdata_rx_out(MGT_USRWORD),                            // output wire [63 : 0] gtwiz_userdata_rx_out
    
    .gtrefclk00_in(gtrefclk00_in),                                            // input wire [0 : 0] gtrefclk00_in
    .qpll0outclk_out(),                                        // output wire [0 : 0] qpll0outclk_out
    .qpll0outrefclk_out(),                                  // output wire [0 : 0] qpll0outrefclk_out
    
    .drpaddr_in(drpaddr_toip),                                                  // input wire [17 : 0] drpaddr_in
    .drpclk_in(drpclk_in),                                                    // input wire [1 : 0] drpclk_in
    .drpdi_in(drpdi_toip),                                                      // input wire [31 : 0] drpdi_in
    .drpen_in(drpen_toip),                                                      // input wire [1 : 0] drpen_in
    .drpwe_in(drpwe_toip),                                                      // input wire [1 : 0] drpwe_in
    .gthrxn_in(RXn_i),                                                    // input wire [1 : 0] gthrxn_in
    .gthrxp_in(RXp_i),                                                    // input wire [1 : 0] gthrxp_in
    .rxcommadeten_in({UPLINKCOUNT{1'b1}}),                                        // input wire [1 : 0] rxcommadeten_in
    .rxmcommaalignen_in({UPLINKCOUNT{1'b0}}),                                  // input wire [1 : 0] rxmcommaalignen_in
    .rxpcommaalignen_in({UPLINKCOUNT{1'b0}}),                                  // input wire [1 : 0] rxpcommaalignen_in
    .rxpolarity_in(MGT_RXPolarity_i),                                            // input wire [1 : 0] rxpolarity_in
    .rxprbscntreset_in(rxprbscntreset),                                    // input wire [1 : 0] rxprbscntreset_in
    .rxprbssel_in(rxprbssel),                                              // input wire [7 : 0] rxprbssel_in
    .rxslide_in(MGT_RXSlide_i),                                                  // input wire [1 : 0] rxslide_in
    .rxusrclk_in(rxusrclk_in),                                                // input wire [1 : 0] rxusrclk_in
    .rxusrclk2_in(rxusrclk2_in),                                              // input wire [1 : 0] rxusrclk2_in
    .txpippmen_in(txpippmen),                                              // input wire [1 : 0] txpippmen_in
    .txpippmovrden_in(txpippmovrden),                                      // input wire [1 : 0] txpippmovrden_in
    .txpippmpd_in(txpippmpd),                                              // input wire [1 : 0] txpippmpd_in
    .txpippmsel_in(txpippmsel),                                            // input wire [1 : 0] txpippmsel_in
    .txpippmstepsize_in(txpippmstepsize),                                  // input wire [9 : 0] txpippmstepsize_in
    .txpolarity_in(MGT_TXPolarity_i),                                            // input wire [1 : 0] txpolarity_in
    .txusrclk_in(txusrclk_in),                                                // input wire [1 : 0] txusrclk_in
    .txusrclk2_in(txusrclk2_in),                                              // input wire [1 : 0] txusrclk2_in
    .drpdo_out(drpdo),                                                    // output wire [31 : 0] drpdo_out
    .drprdy_out(drprdy),                                                  // output wire [1 : 0] drprdy_out
    .gthtxn_out(TXn_o),                                                  // output wire [1 : 0] gthtxn_out
    .gthtxp_out(TXp_o),                                                  // output wire [1 : 0] gthtxp_out
    .gtpowergood_out(),                                        // output wire [1 : 0] gtpowergood_out
    .rxbyteisaligned_out(),                                // output wire [1 : 0] rxbyteisaligned_out
    .rxbyterealign_out(),                                    // output wire [1 : 0] rxbyterealign_out
    .rxcommadet_out(),                                          // output wire [1 : 0] rxcommadet_out
    .rxoutclk_out(rxoutclk_sig),                                              // output wire [1 : 0] rxoutclk_out
    .rxpmaresetdone_out(rxpmaresetdone),                                  // output wire [1 : 0] rxpmaresetdone_out
    .rxprbserr_out(rxprbserr),                                            // output wire [1 : 0] rxprbserr_out
    .rxprbslocked_out(rxprbslocked),                                      // output wire [1 : 0] rxprbslocked_out
    .txbufstatus_out(txbufstatus),                                        // output wire [3 : 0] txbufstatus_out
    .txoutclk_out(txoutclk_sig),                                              // output wire [1 : 0] txoutclk_out
    .txpmaresetdone_out(txpmaresetdone)                                  // output wire [1 : 0] txpmaresetdone_out
);


genvar l;
for (l = 0; l < UPLINKCOUNT; l=l+1) begin:tx_phase
    tx_phase_aligner 
    #(  .g_DRP_NPORT_CTRL(1'b1),
        .g_DRP_ADDR_TXPI_PPM_CFG(9'b010011010))
    tx_phase_aligner_inst(
        // --==============================================================================
        // --! User control/monitor ports
        // --==============================================================================    
        // -- Clock / reset  
        .clk_sys_i             (MGT_FREEDRPCLK_i),
        .reset_i               (tx_reset_hptd_ip_sync[l]),
        .tx_aligned_o          (MGT_TX_ALIGNED[l]),
        .tx_pi_phase_calib_i   (MGT_TXCALIB_i[((l+1)*7-1):l*7]),
        .tx_ui_align_calib_i   (MGT_ENTXCALIBIN_i[l]),
        .tx_fifo_fill_pd_max_i (32'h00400000),
        .tx_fine_realign_i     ('b0),
        .ps_strobe_i           ('b0),
        .ps_inc_ndec_i         ('b0),
        .ps_phase_step_i       ('b0),
        .ps_done_o             (),
        .tx_pi_phase_o         (MGT_TX_PIPHASE_o[l]),
        .tx_fifo_fill_pd_o (),

        // --==============================================================================
        // --! MGT ports
        // --==============================================================================
        .clk_txusr_i (tx_wordclk_sig[0]),
        .tx_fifo_fill_level_i (txbufstatus[l*2]),
        .txpippmen_o (txpippmen[l]),
        .txpippmovrden_o (txpippmovrden[l]),
        .txpippmsel_o (txpippmsel[l]),
        .txpippmpd_o (txpippmpd[l]),
        .txpippmstepsize_o (txpippmstepsize[((l+1)*5-1):l*5]),

        .drpaddr_o (drpaddr[((l+1)*9-1):l*9]),
        .drpen_o (drpen[l]),
        .drpdi_o (drpdi[((l+1)*16-1):l*16]),
        .drprdy_i (drprdy[l]),
        .drpdo_i (drpdo[((l+1)*16-1):l*16]),
        .drpwe_o (drpwe[l])
    );
end

genvar k;
for (k = 0; k < UPLINKCOUNT; k=k+1) begin:drp_interface

    drp_interface drp_interface_inst
    (
        .drp_clk          (MGT_FREEDRPCLK_i),
        .drprdy_i         (drprdy[k]),
        .drpdo_i          (drpdo[(k+1)*16-1:k*16]),
        .drpen_o          (drpen_ibert[k]),
        .drpwe_o          (drpwe_ibert[k]),
        .drpaddr_o        (drpaddr_ibert[(k+1)*9-1:k*9]),
        .drpdi_o          (drpdi_ibert[(k+1)*16-1:k*16]),
        .rxprbscntreset_o (rxprbscntreset[k]),
        .rxprbssel_o      (rxprbssel[(k+1)*4-1:k*4]),
        .rxprbserr_i      (rxprbserr[k]),
        .rxprbslocked_i   (rxprbslocked[k])
    );
end

endmodule
