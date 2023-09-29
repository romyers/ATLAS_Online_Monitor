
// JTAG Chain Settings
`define   NUM_DEV         19   // Indexing starting from 0
`define   NUM_DEV_BITS    4   // Number of bits, minus one, to store NUM_DEV + 3 (don't ask why)

// Mezzanine Definitions
`define	  IR_LEN          5
`define   WORD_LEN        32
`define   ID_CODE         32'hFADEC001
`define   DATA_LEN_BITS   8

// // Instructions
// `define   EXTEST      5'b00000
// `define   IDCODE      5'b10001
// `define   SAMPLE      5'b10010
// `define   INTEST      5'b00011
// `define   CONTROL     5'b11000
// `define   ASD         5'b01001
// `define   STATUS      5'b01010
// `define   CORETEST    5'b11011
// `define   BIST        5'b01100
// `define   GEN_OUTPUT  5'b11101
// `define   BYPASS      5'b01111

// Instructions

`define   IDCODE      5'b10001
`define   BYPASS      5'b01111
`define   SETUP0      5'b10010
`define   SETUP1      5'b00011
`define   SETUP2      5'b10100
`define   CONTROL0    5'b00101
`define   CONTROL1    5'b00110
`define   STATUS0     5'b10111
`define   STATUS1     5'b11000
`define   ASDWRITE    5'b01001
`define   ASDREAD     5'b01010

`define   SETUP0_LENGTH      9'd115
`define   SETUP1_LENGTH      9'd94
`define   SETUP2_LENGTH      9'd36
`define   CONTROL0_LENGTH    9'd8
`define   CONTROL1_LENGTH    9'd47
`define   STATUS0_LENGTH      9'd33
`define   STATUS1_LENGTH      9'd25
`define   ASDWRITE_LENGTH    9'd166
`define   ASDREAD_LENGTH     9'd166
`define   IDCODE_LENGTH      9'd32


// TAP States
`define   STATE_TEST_LOGIC_RESET  4'h0
`define   STATE_RUN_TEST_IDLE     4'h1  
`define   STATE_SELECT_DR_SCAN    4'h2
`define   STATE_SELECT_IR_SCAN    4'h3
`define   STATE_CAPTURE_DR        4'h4
`define   STATE_CAPTURE_IR        4'h5
`define   STATE_SHIFT_DR          4'h6
`define   STATE_SHIFT_IR          4'h7
`define   STATE_EXIT1_DR          4'h8
`define   STATE_EXIT1_IR          4'h9
`define   STATE_PAUSE_DR          4'hA  
`define   STATE_PAUSE_IR          4'hB
`define   STATE_EXIT2_DR          4'hC  
`define   STATE_EXIT2_IR          4'hD
`define   STATE_UPDATE_DR         4'hE
`define   STATE_UPDATE_IR         4'hF



