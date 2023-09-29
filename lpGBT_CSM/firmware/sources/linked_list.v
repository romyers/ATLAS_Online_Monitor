`timescale 1ns / 1ps
`include "definitions.sv"

// Fills two vectors with indecies of where this node on the chain gets its TDI from
// and where it sends its TDO to. This allows us to construct a dynamic JTAG chain.
module linked_list (
  input   wire                    rst,            // Asynchronous Reset.
  input   wire                    enb,            // Enable, must be held high.
  input   wire                    clk,            // Logic Clock (40MHz or less).
  input   wire  [`NUM_DEV_BITS:0] index,          // Index value of the node we are considering (0 -> `NUM_DEV).
  input   wire  [`NUM_DEV:0]      active_chains,  // Active chains vector take from initialization phase "Daisy Chain Vector".
  output  reg   [`NUM_DEV_BITS:0] tdi_index,      // What index does this node take its TDI from (see localparam, below, for special cases)?
  output  reg   [`NUM_DEV_BITS:0] tdo_index,      // What index does this node send its TDO to (see localparam, below, for special cases)?
  output  reg                     done            // Is the process done?
);
  
  localparam NULL_INDEX = `NUM_DEV + 3;   // Special index to represent a node that is unassigned.
  localparam LAST_INDEX = `NUM_DEV + 2;   // Special index to represent that this node is the first on the chain.
  localparam FIRST_INDEX = `NUM_DEV + 1;  // Special index to represent that this node is the last on the chain.
  
  integer count_tdi;  // Index counter for TDI, counts down.
  integer count_tdo;  // Index counter for TDO, counts up.
  
  reg tdi_done;       // Is the TDI search process done?
  reg tdo_done;       // Is the TDO search process done?
  
  // Assigns the TDO Index for this node.
  // This works by finding the "closest" active node to the "left" of the node we are considering.
  // If no active node is found this implies that this node is the first on the chain.
  always @ (posedge clk, posedge rst) begin
    if (rst) begin
      tdi_index <= NULL_INDEX;                              // Initially this node is unassigned
      count_tdi <= index;                                   // Initially this node's counter starts at the index it exists as.
      tdi_done <= 0;                                        // TDI search not done.
    end
    else if (enb) begin                                     // If the boot config process is done.
      if (~active_chains[index]) begin
        tdi_index <= NULL_INDEX;                            // If there is no device at this index then it cannot get a TDI from anywhere.
        tdi_done <= 1;                                      // TDO search done.
      end
      else begin
        if (count_tdi - 1 < 0) begin
          tdi_index <= FIRST_INDEX;                         // If the counter falls below zero without finding an active node then this index is the first node.
          tdi_done <= 1;                                    // TDI search done.
        end
        else if (active_chains[count_tdi - 1]) begin
          tdi_index <= count_tdi - 1;                       // If the counter finds an active node then that is where it gets its TDI from.
          tdi_done <= 1;                                    // TDI search done.
        end
        else count_tdi <= count_tdi - 1;                    // Decrement the counter.
      end
    end
  end
  
  // Assigns the TDI Index.
  // This works by finding the "closest" active node to the "right" of the node we are considering.
  // If no active node is found this implies that this node is the last on the chain.
  always @ (posedge clk, posedge rst) begin
    if (rst) begin
      tdo_index <= NULL_INDEX;                              // Initially this node is unassigned
      count_tdo <= index;                                   // Initially this node's counter starts at the index it exists as.
      tdo_done <= 0;                                        // TDO search not done.
    end
    else if (enb) begin                                     // If the boot config process is done.
      if (~active_chains[index]) begin
        tdo_index <= NULL_INDEX;                            // If there is no device at this index then it cannot send a TDO anywhere.
        tdo_done <= 1;                                      // TDO search done.
      end
      else begin
        if (count_tdo + 1 > `NUM_DEV) begin
          tdo_index <= LAST_INDEX;                          // If the counter goes above the maximum number of devices without finding an active node then this index is the last node.
          tdo_done <= 1;                                    // TDO search done.
        end
        else if (active_chains[count_tdo + 1]) begin
          tdo_index <= count_tdo + 1;                       // If the counter finds an active node then that is hwere it sends its TDO to.
          tdo_done <= 1;                                    // TDO search done.
        end
        else count_tdo <= count_tdo + 1;                    // Increment the counter.
      end
    end
  end
  
  // Assigns done signal.
  always @ * begin
    if (tdo_done && tdi_done) done <= 1;
    else done <= 0;
  end
  
endmodule
