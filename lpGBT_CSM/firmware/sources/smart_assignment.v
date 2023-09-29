`timescale 1ns / 1ps
`include "definitions.sv"

module smart_assignment (
  input   wire                rst,                    // Asynchronous reset.
  input   wire                clk,                    // Logic Clock (40Mhz or less)
  input   wire                jtag_tck,               // JTAG Clock.
  input   wire                jtag_tms,               // JTAG TMS.
  input   wire                jtag_tdi,               // JTAG Chain single pin input.
  input   wire  [`NUM_DEV:0]  tdo_chain,              // JTAG TDO fan-in.
  output  reg   [`NUM_DEV:0]  tdi_chain,              // JTAG TDI fan-out.
  output  reg                 jtag_tdo,               // JTAG Chain single pin output.
  output  reg   [`NUM_DEV:0]  tms_chain,              // JTAG TMS fan-out.
  output  reg   [`NUM_DEV:0]  tck_chain,              // JTAG TCK fan-out.
  output  reg   [`NUM_DEV_BITS:0] num_active_ports,   // Unsigned integer value of how many nodes there are on the JTAG chain.
  output  wire  [`NUM_DEV:0]  active_chains,          // What ports are active. "Daisy Chain Vector"
  output  reg                 done                    // Smart assignment has completed and the chain is now usable.
);

  genvar i;                                         // For loop generator varaible.
  integer j;                                        // For loop generator integer.
  localparam NULL_INDEX = `NUM_DEV + 3;             // Special index to represent a node that is unassigned.
  localparam LAST_INDEX = `NUM_DEV + 2;             // Special index to represent that this node is the first on the chain.
  localparam FIRST_INDEX = `NUM_DEV + 1;            // Special index to represent that this node is the last on the chain.
  wire  [`NUM_DEV_BITS:0] tdo_array [`NUM_DEV:0];   // Array to store indecies of TDO pointers.
  wire  [`NUM_DEV_BITS:0] tdi_array [`NUM_DEV:0];   // Array to store indecies of TDI pointers.
  wire  [3:0]             tap_state;                // TAP State for config phase.
  wire                    boot_config_done;         // Has the boot config process finished?
  wire  [`NUM_DEV:0]      linked_list_chain_done;   // What linked list ports have finished their process?
  wire  [`NUM_DEV:0]      bc_tms;                   // Boot config TMS, used in MUX
  wire  [`NUM_DEV:0]      bc_tdi;                   // Boot config TDI, used in MUX
  reg   [`NUM_DEV:0]      sa_tdi;                   // Smart assignment TDI, used in MUX
  
  // Boot config chain determines which devices are connect and working
  // on the jtag network. Returns a vector that stores this data.
  jtag_boot_config_chain jbcc (
    .rst(rst),                      // Asynchronous reset.
    .tck(tck_chain),                // JTAG TCK fan-in.
    .tdo(tdo_chain),                // JTAG TDO fan-in.
    .tap_state(tap_state),          // TAP State.
    .tms(bc_tms),                   // JTAG TMS fan-out.
    .tdi(bc_tdi),                   // JTAG TDI fan-out.
    .done(boot_config_done),        // Has the process finished?
    .active_chains(active_chains)   // What JTAG ports are active? "Daisy Chain Vector"
  );
  
  // TAP State Machine for config.
  jtag_tap_state_machine tsm_config (
    .rst(rst),          // Asychronous reset.
    .tck(jtag_tck),     // Test clock.
    .tms(bc_tms[0]),    // Test mode select.
    .state(tap_state)   // Tap state.
  );
  
  // Linked list generator for every single port.
  for (i = 0; i <= `NUM_DEV; i = i + 1) begin
    linked_list list (
      .rst(rst),                        // Asynchronous Reset.
      .enb(boot_config_done),           // Enable, must be held high.
      .clk(clk),                        // Logic Clock (40Mhz or less).
      .index(i),                        // Index value of the node we are considering (0 -> `NUM_DEV).
      .active_chains(active_chains),    // Active chains vector take from initialization phase "Daisy Chain Vector".
      .tdi_index(tdi_array[i]),         // What index does this node take its TDI from (see localparam for special cases)?
      .tdo_index(tdo_array[i]),         // What index does this node send its TDO to (see localparam for special cases)?
      .done(linked_list_chain_done[i])  // Is the process done?
    );
  end
  
  // Determines if the linked list process has finished.
  always @ * begin
    if (linked_list_chain_done == {(`NUM_DEV + 1){1'b1}}) done <= 1;    // If every port has finished its process then the the chain process is done.
    else done <= 0;                                                     // Else it is not.
  end

  // MUX for TMS and TDI.
  for (i = 0; i <= `NUM_DEV; i = i + 1) begin
    always @ * begin
      tck_chain[i] <= jtag_tck;       // Forward JTAG Clock to every device.
      if (done) begin
        tms_chain[i] <= jtag_tms;     // If Linked List is done configuring send JTAG TMS.
        tdi_chain[i] <= sa_tdi[i];    // If Linked List is done configuring send smart assigned TDIs.
      end
      else begin
        tms_chain[i] <= bc_tms[i];    // Else send config instructions.
        tdi_chain[i] <= bc_tdi[i];    // Else fan-out tdi during configuring stage.
      end
    end
  end

  // Assign Smart Assign TDI.
  for (i = 0; i <= `NUM_DEV; i = i + 1) begin
    always @ * begin
      if (tdi_array[i] == FIRST_INDEX) sa_tdi[i] <= jtag_tdi;                     // If it is the first element feed in JTAG TDI.
      else if (tdi_array[i] != NULL_INDEX) sa_tdi[i] <= tdo_chain[tdi_array[i]];  // Else work from chain logic.
      else sa_tdi[i] <= 0;                                                        // Else it is not part of the chain.
    end
  end
  
  // Assign number of valid JTAG chain nodes.
  always @ * begin
    num_active_ports = {(`NUM_DEV + 1){1'b0}};
    for (j = 0; j <= `NUM_DEV; j = j + 1) begin
      num_active_ports = num_active_ports + active_chains[j];
    end
  end
  
  // Assign the single ended JTAG TDO output pin.
  always @ * begin
    jtag_tdo <= 0;
    if (`NUM_DEV == 0) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
    end
    else if (`NUM_DEV == 1) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
    end
    else if (`NUM_DEV == 2) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
    end
    else if (`NUM_DEV == 3) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
    end
    else if (`NUM_DEV == 4) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
    end
    else if (`NUM_DEV == 5) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
    end
    else if (`NUM_DEV == 6) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
    end
    else if (`NUM_DEV == 7) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
    end
    else if (`NUM_DEV == 8) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
    end
    else if (`NUM_DEV == 9) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
    end
    else if (`NUM_DEV == 10) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
    end
    else if (`NUM_DEV == 11) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
    end
    else if (`NUM_DEV == 12) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
    end
    else if (`NUM_DEV == 13) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
    end
    else if (`NUM_DEV == 14) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
    end
    else if (`NUM_DEV == 15) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
      else if (tdo_array[15] == LAST_INDEX) jtag_tdo <= tdo_chain[15];
    end
    else if (`NUM_DEV == 16) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
      else if (tdo_array[15] == LAST_INDEX) jtag_tdo <= tdo_chain[15];
      else if (tdo_array[16] == LAST_INDEX) jtag_tdo <= tdo_chain[16];
    end
    else if (`NUM_DEV == 17) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
      else if (tdo_array[15] == LAST_INDEX) jtag_tdo <= tdo_chain[15];
      else if (tdo_array[16] == LAST_INDEX) jtag_tdo <= tdo_chain[16];
      else if (tdo_array[17] == LAST_INDEX) jtag_tdo <= tdo_chain[17];
    end
    else if (`NUM_DEV == 18) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
      else if (tdo_array[15] == LAST_INDEX) jtag_tdo <= tdo_chain[15];
      else if (tdo_array[16] == LAST_INDEX) jtag_tdo <= tdo_chain[16];
      else if (tdo_array[17] == LAST_INDEX) jtag_tdo <= tdo_chain[17];
      else if (tdo_array[18] == LAST_INDEX) jtag_tdo <= tdo_chain[18];
    end
    else if (`NUM_DEV == 19) begin
      if (tdo_array[0] == LAST_INDEX) jtag_tdo <= tdo_chain[0];
      else if (tdo_array[1] == LAST_INDEX) jtag_tdo <= tdo_chain[1];
      else if (tdo_array[2] == LAST_INDEX) jtag_tdo <= tdo_chain[2];
      else if (tdo_array[3] == LAST_INDEX) jtag_tdo <= tdo_chain[3];
      else if (tdo_array[4] == LAST_INDEX) jtag_tdo <= tdo_chain[4];
      else if (tdo_array[5] == LAST_INDEX) jtag_tdo <= tdo_chain[5];
      else if (tdo_array[6] == LAST_INDEX) jtag_tdo <= tdo_chain[6];
      else if (tdo_array[7] == LAST_INDEX) jtag_tdo <= tdo_chain[7];
      else if (tdo_array[8] == LAST_INDEX) jtag_tdo <= tdo_chain[8];
      else if (tdo_array[9] == LAST_INDEX) jtag_tdo <= tdo_chain[9];
      else if (tdo_array[10] == LAST_INDEX) jtag_tdo <= tdo_chain[10];
      else if (tdo_array[11] == LAST_INDEX) jtag_tdo <= tdo_chain[11];
      else if (tdo_array[12] == LAST_INDEX) jtag_tdo <= tdo_chain[12];
      else if (tdo_array[13] == LAST_INDEX) jtag_tdo <= tdo_chain[13];
      else if (tdo_array[14] == LAST_INDEX) jtag_tdo <= tdo_chain[14];
      else if (tdo_array[15] == LAST_INDEX) jtag_tdo <= tdo_chain[15];
      else if (tdo_array[16] == LAST_INDEX) jtag_tdo <= tdo_chain[16];
      else if (tdo_array[17] == LAST_INDEX) jtag_tdo <= tdo_chain[17];
      else if (tdo_array[18] == LAST_INDEX) jtag_tdo <= tdo_chain[18];
      else if (tdo_array[19] == LAST_INDEX) jtag_tdo <= tdo_chain[19];
    end
  end

endmodule
