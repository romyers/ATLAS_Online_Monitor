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
// File   : lpgbtFpga_10g24_multiuplink.v
// Create : 2023-07-10 13:38:46
// Revise : 2023-07-14 17:25:23
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
module lpgbtFpga_10g24_multiuplink

    #(parameter FEC = 1,
    parameter UPLINKCOUNT = 2,
    parameter DOWNLINKCOUNT = 1
    )
(
    //clocks
    input                           downlinkClk_i,                //40MHz user clock
    input                           uplinkClk_i,                  //40MHz user clock
    input                           downlinkRst_i,                //Reset the downlink path
    input                           uplinkRst_i,                  //Reset the uplink path

    //downlinks
    input   [DOWNLINKCOUNT*32-1:0]  downlinkUserData_i,    //Downlink data (user)
    input   [DOWNLINKCOUNT*2-1:0]   downlinkEcData_i,       //Downlink EC field
    input   [DOWNLINKCOUNT*2-1:0]   downlinkIcData_i,       //Downlink IC field
    input   [DOWNLINKCOUNT-1:0]     downLinkBypassInterleaver_i,  //Bypass downlink interleaver (test purpose only)
    input   [DOWNLINKCOUNT-1:0]     downLinkBypassFECEncoder_i,   //Bypass downlink FEC (test purpose only)
    input   [DOWNLINKCOUNT-1:0]     downLinkBypassScrambler_i,    //Bypass downlink scrambler (test purpose only)
    output  [DOWNLINKCOUNT-1:0]     downlinkReady_o,             //Downlink ready status
    output  [DOWNLINKCOUNT*10-1:0]  downlinkPhase_o,       //Phase to check fixed-phase
    input   [DOWNLINKCOUNT*10-1:0]  downlinkPhaseCalib_i,   //Phase measured in first reset
    input   [DOWNLINKCOUNT-1:0]     downlinkPhaseForce_i,          //Force phase after first reset to ensure fixed-phase operation

    //uplinks
    output  [UPLINKCOUNT*230-1:0]   uplinkUserData_o,   //Uplink data (user)
    output  [UPLINKCOUNT*2-1:0]     uplinkEcData_o,       //Uplink EC field
    output  [UPLINKCOUNT*2-1:0]     uplinkIcData_o,       //Uplink IC field
    input   [UPLINKCOUNT-1:0]       uplinkBypassInterleaver_i,  //Bypass uplink interleaver (test purpose only)
    input   [UPLINKCOUNT-1:0]       uplinkBypassFECEncoder_i,   //Bypass uplink FEC (test purpose only)
    input   [UPLINKCOUNT-1:0]       uplinkBypassScrambler_i,    //Bypass uplink scrambler (test purpose only)
    input   [UPLINKCOUNT-1:0]       uplinkFECCorrectedClear_i,  //Uplink FEC corrected error clear (debugging)
    output reg [UPLINKCOUNT-1:0]    uplinkFECCorrectedLatched_o, //Uplink FEC corrected error latched (debugging)
    output  [UPLINKCOUNT-1:0]       uplinkReady_o,             //Uplink ready status

    //Fixed-phase uplink CDC operation - master
    output  [UPLINKCOUNT*3-1:0]     uplinkPhase_o,     //Phase to check fixed-phase
    input   [UPLINKCOUNT*3-1:0]     uplinkPhaseCalib_i,     //Phase measured in first reset
    input   [UPLINKCOUNT-1:0]       uplinkPhaseForce_i,       //Force the phase to be the calibrated one

    //MGT
    input   [UPLINKCOUNT-1:0]       mgt_rxn_i,
    input   [UPLINKCOUNT-1:0]       mgt_rxp_i,
    output  [DOWNLINKCOUNT-1:0]     mgt_txn_o,
    output  [DOWNLINKCOUNT-1:0]     mgt_txp_o,

    input                           clk_mgtrefclk_i,      //Transceiver serial clock
    input                           clk_mgtfreedrpclk_i,      //125MHz Free-running clock

    output  [UPLINKCOUNT-1:0]       clk_mgtRxClk_o,
    output  [DOWNLINKCOUNT-1:0]     clk_mgtTxClk_o,

    input   [DOWNLINKCOUNT-1:0]     mgt_txpolarity_i,
    input   [UPLINKCOUNT-1:0]       mgt_rxpolarity_i,

    input   [DOWNLINKCOUNT-1:0]     mgt_txcaliben_i,
    input   [DOWNLINKCOUNT*7-1:0]   mgt_txcalib_i,
    output  [DOWNLINKCOUNT-1:0]     mgt_txaligned_o,
    output  [DOWNLINKCOUNT*7-1:0]   mgt_txphase_o
);



    // User CDC Tx    
    wire    [DOWNLINKCOUNT*36-1:0]  downlinkData40;
    wire    [DOWNLINKCOUNT*36-1:0]  downlinkData320;
    wire    [DOWNLINKCOUNT-1:0]     downlinkStrobe320;
    wire    [DOWNLINKCOUNT-1:0]     mgt_txrdy_sync40;
    wire    [DOWNLINKCOUNT-1:0]     cdc_tx_reset;
    wire    [DOWNLINKCOUNT-1:0]     cdc_tx_ready;
    
    // User CDC Rx
    wire    [UPLINKCOUNT*234-1:0]   uplinkData320;
    wire    [UPLINKCOUNT-1:0]       uplinkStrobe320;
    wire    [UPLINKCOUNT*234-1:0]   uplinkData40;
    wire    [UPLINKCOUNT-1:0]       cdc_rx_reset;
    wire    [UPLINKCOUNT-1:0]       cdc_rx_ready;

    //MGT
    wire    [UPLINKCOUNT-1:0]       uplinkReady;
    wire    [DOWNLINKCOUNT*32-1:0]  downlink_mgtword;

    wire    [UPLINKCOUNT*32-1:0]    uplink_mgtword;
    wire    [UPLINKCOUNT-1:0]       mgt_rxslide;

    wire    [DOWNLINKCOUNT-1:0]     mgt_txrdy;
    wire    [UPLINKCOUNT-1:0]       mgt_rxrdy;
    wire    [DOWNLINKCOUNT-1:0]     clk_mgtTxClk;
    wire    [UPLINKCOUNT-1:0]       clk_mgtRxClk;

    // FEC latch flag
    wire    [UPLINKCOUNT*230-1:0]   uplinkDataCorrected;
    wire    [UPLINKCOUNT*2-1:0]     uplinkIcCorrected;
    wire    [UPLINKCOUNT*2-1:0]     uplinkEcCorrected;

    wire    [DOWNLINKCOUNT-1:0]     mgt_txaligned;
    wire    [DOWNLINKCOUNT*7-1:0]   mgt_txphase;

    
    // Reset scheme for downlink:
    // -- downlinkRst_i     => Resets MGT Tx
    // -- MGT Tx ready      => Resets User CDC Tx (responsible for generating the stable strobe aligned to 40MHz clock)
    // -- User CDC Tx ready => Resets downlink datapath

    assign cdc_rx_reset = ~ uplinkReady;
    assign cdc_tx_reset = ~mgt_txrdy_sync40;




genvar j;
for (j = 0; j < DOWNLINKCOUNT; j=j+1) begin
    assign downlinkData40[36*(j+1)-1:36*j] = {downlinkIcData_i[2*(j+1)-1:2*j],
    downlinkEcData_i[2*(j+1)-1:2*j],downlinkUserData_i[32*(j+1)-1:32*j]};

    xlx_ku_mgt_ip_reset_synchronizer txrdy_sync(
        .clk_in  (downlinkClk_i),
        .rst_in  (mgt_txrdy[j]),
        .rst_out (mgt_txrdy_sync40[j])
    );

    lpgbtfpga_downlink
    #(
        //Expert parameters
        .c_multicyleDelay    (3),
        .c_clockRatio        (8),
        .c_outputWidth       (32)
    )
    downlink_inst(
        //Clocks
        .clk_i               (clk_mgtTxClk[j]),
        .clkEn_i             (downlinkStrobe320[j]),
        .rst_n_i             (cdc_tx_ready[j]),
        //Down link
        .userData_i          (downlinkData320[32*(j+1)-1:32*j]),
        .ECData_i            (downlinkData320[32*(j+1)+1:32*j+32]),
        .ICData_i            (downlinkData320[32*(j+1)+3:32*j+34]),
        //Output
        .mgt_word_o          (downlink_mgtword[32*(j+1)-1:32*j]),
        //Configuration
        .interleaverBypass_i (downLinkBypassInterleaver_i[j]),
        .encoderBypass_i     (downLinkBypassFECEncoder_i[j]),
        .scramblerBypass_i   (downLinkBypassScrambler_i[j]),
        //Status
        .rdy_o               (downlinkReady_o[j])
    );

    cdc_tx
    #(
        .g_CLOCK_A_RATIO (1),
        .g_CLOCK_B_RATIO (8),
        .g_ACC_PHASE     (125*8),
        .g_PHASE_SIZE    (10),
        .data_SIZE       (36)
    )
    cdc_tx_inst(
        //Interface A (latch - from where data comes)
        .reset_a_i       (cdc_tx_reset[j]),
        .clk_a_i         (downlinkClk_i),
        .data_a_i        (downlinkData40[36*(j+1)-1:36*j]),
        .strobe_a_i      (1'b1),
        //Interface B (capture - to where data goes)
        .clk_b_i         (clk_mgtTxClk[j]),
        .data_b_o        (downlinkData320[36*(j+1)-1:36*j]),
        .strobe_b_o      (downlinkStrobe320[j]),
        .ready_b_o       (cdc_tx_ready[j]),
        //Only relevant for fixed-phase operation
        .clk_freerun_i   (clk_mgtfreedrpclk_i),
        .phase_o         (downlinkPhase_o[10*(j+1)-1:10*j]),
        .phase_calib_i   (downlinkPhaseCalib_i[10*(j+1)-1:10*j]),
        .phase_force_i   (downlinkPhaseForce_i[j])
    );
end

assign clk_mgtTxClk_o = clk_mgtTxClk;
assign clk_mgtRxClk_o = clk_mgtRxClk;

genvar i;
for (i = 0; i < UPLINKCOUNT; i=i+1) begin

    assign uplinkUserData_o = uplinkData40[234*(i+1)-5:234*i];
    assign EcData_o         = uplinkData40[234*(i+1)-3:234*i+230];
    assign IcData_o         = uplinkData40[234*(i+1)-1:234*i+232];
    assign uplinkReady_o[i] = cdc_rx_ready[i];

    //FEC Corrected Flag for debugging
    always @(posedge clk_mgtRxClk[i]) begin
        if(uplinkFECCorrectedClear_i[i])
            uplinkFECCorrectedLatched_o[i] <= 1'b0;
        else if (((|(uplinkDataCorrected[230*(i+1)-1:230*i]))!=1'b0) 
            || ((|uplinkIcCorrected[2*(i+1)-1:2*i])!=1'b0)
            || ((|uplinkEcCorrected[2*(i+1)-1:2*i])!=1'b0))
            uplinkFECCorrectedLatched_o[i] <= 1'b1;
    end

    // --========####   Uplink datapath   ####========--
    // -- Reset scheme for uplink:
    // -- uplinkRst_i           => Resets MGT Rx
    // -- MGT Rx ready          => Resets uplink datapath
    // -- Uplink datapath ready => Resets User CDC Rx
    lpgbtfpga_uplink 
    #(
        //General configuration
        .DATARATE                  (2), //10G24=2
        .FEC                       (1), //FEC5=1
        //Expert parameters
        .c_multicyleDelay          (3),
        .c_clockRatio              (8),
        .c_mgtWordWidth            (32),
        .c_allowedFalseHeader      (5),
        .c_allowedFalseHeaderOverN (64),
        .c_requiredTrueHeader      (30),
        .c_bitslip_mindly          (1),
        .c_bitslip_waitdly         (40)
    )
    uplink_inst(
        //Clock and reset
        .uplinkClk_i               (clk_mgtRxClk[i]),
        .uplinkClkOutEn_o          (uplinkStrobe320[i]),
        .uplinkRst_n_i             (mgt_rxrdy[i]),
        //Input
        .mgt_word_i                (uplink_mgtword[32*(i+1)-1:32*i]),
        //Data
        .userData_o                (uplinkData320[234*(i+1)-5:234*i]),
        .EcData_o                  (uplinkData320[234*(i+1)-3:234*i+230]),
        .IcData_o                  (uplinkData320[234*(i+1)-1:234*i+232]),
        //Control
        .bypassInterleaver_i       (uplinkBypassInterleaver_i[i]),
        .bypassFECEncoder_i        (uplinkBypassFECEncoder_i[i]),
        .bypassScrambler_i         (uplinkBypassScrambler_i[i]),
        //Transceiver control
        .mgt_bitslipCtrl_o         (mgt_rxslide[i]),
        //Status
        .dataCorrected_o           (uplinkDataCorrected[230*(i+1)-1:230*i]),
        .IcCorrected_o             (uplinkIcCorrected[2*(i+1)-1:2*i]),
        .EcCorrected_o             (uplinkEcCorrected[2*(i+1)-1:2*i]),
        .rdy_o                     (uplinkReady[i]),
        .frameAlignerEven_o        ()
    );

    cdc_rx  
    #(
        .g_CLOCK_A_RATIO (8),    //Frequency ratio between slow and fast frequencies (>4)
        .g_PHASE_SIZE    (3),       //log2(g_CLOCK_A_RATIO)
        .data_SIZE       (234)
    )
    cdc_rx_inst(
        //-- Interface A (latch - from where data comes)
        .reset_a_i       (cdc_rx_reset[i]), 
        .clk_a_i         (clk_mgtRxClk[i]),
        .data_a_i        (uplinkData320[234*(i+1)-1:234*i]),
        .strobe_a_i      (uplinkStrobe320[i]),

        //Interface B (capture_a - to where data goes) 
        .clk_b_i         (uplinkClk_i),
        .data_b_o        (uplinkData40[234*(i+1)-1:234*i]),
        .strobe_b_i      (1'b1),
        .ready_b_o       (cdc_rx_ready[i]),

        //Only relevant for fixed-phase operation
        .phase_o         (uplinkPhase_o[3*(i+1)-1:3*i]),
        .phase_calib_i   (uplinkPhaseCalib_i[3*(i+1)-1:3*i]),
        .phase_force_i   (uplinkPhaseForce_i[i])
    );

end

xlx_ku_mgt_10g24_multilink #(
        .UPLINKCOUNT(UPLINKCOUNT),
        .CLKREFCOUNT(1)
    ) mgt_inst (
        .MGT_REFCLK_i      (clk_mgtrefclk_i),
        .MGT_FREEDRPCLK_i  (clk_mgtfreedrpclk_i),
        .MGT_RXUSRCLK_o    (clk_mgtRxClk),
        .MGT_TXUSRCLK_o    (clk_mgtTxClk),
        .MGT_TXRESET_i     (downlinkRst_i),
        .MGT_RXRESET_i     (uplinkRst_i),
        .MGT_TXPolarity_i  (mgt_txpolarity_i),
        .MGT_RXPolarity_i  (mgt_rxpolarity_i),
        .MGT_RXSlide_i     (mgt_rxslide),
        .MGT_ENTXCALIBIN_i (mgt_txcaliben_i),
        .MGT_TXCALIB_i     (mgt_txcalib_i),
        .MGT_TXREADY_o     (mgt_txrdy),
        .MGT_RXREADY_o     (mgt_rxrdy),
        .MGT_TX_ALIGNED_o  (mgt_txaligned_o),
        .MGT_TX_PIPHASE_o  (mgt_txphase_o),
        .MGT_USRWORD_i     (downlink_mgtword),
        .MGT_USRWORD_o     (uplink_mgtword),
        .RXn_i             (mgt_rxn_i),
        .RXp_i             (mgt_rxp_i),
        .TXn_o             (mgt_txn_o),
        .TXp_o             (mgt_txp_o)
    );
    


    
   
endmodule
