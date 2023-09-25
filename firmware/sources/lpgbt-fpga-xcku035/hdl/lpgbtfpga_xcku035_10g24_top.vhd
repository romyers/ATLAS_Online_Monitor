-------------------------------------------------------
--! @file
--! @author Julian Mendez <julian.mendez@cern.ch> (CERN - EP-ESE-BE)
--! @version 2.0
--! @brief KCU105 Example design top - Includes VIOs and pattern gen/check.
-------------------------------------------------------

-- IEEE VHDL standard library:
library ieee;
use ieee.std_logic_1164.all;

package bus_multiplexer_pkg is
    type conf2b_array is array(natural range <>) of std_logic_vector(1 downto 0);
end package;
    
--! Xilinx devices library:
library unisim;
use unisim.vcomponents.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.bus_multiplexer_pkg.all;
use work.lpgbtfpga_package.all;
use work.SCA_PKG.all;

--=================================================================================================--
--#######################################   Entity   ##############################################--
--=================================================================================================--
entity lpgbtfpga_xcku035_10g24_top is
    port (  
      
      --===============--
      -- Clocks scheme --
      --===============--       
      -- MGT(GTX) reference clock:
      ----------------------------
      
      -- Comment: * The MGT reference clock MUST be provided by an external clock generator.
      --
      --          * The MGT reference clock frequency must be 320MHz.
      SMA_MGT_REFCLK_P                               : in  std_logic;
      SMA_MGT_REFCLK_N                               : in  std_logic; 
      
--      LOCK_TOP                                       : out std_logic;          
      -- Fabric clock: 200MHz
      ----------------     

      USER_CLOCK_P                                   : in  std_logic;
      USER_CLOCK_N                                   : in  std_logic; 
      
      TRIGGER0_P                                     : in  std_logic;
      TRIGGER0_N                                     : in  std_logic;

      --hit to mezz
      ----------------
      hit                :OUT STD_LOGIC_VECTOR(23 DOWNTO 0);
      mezz_TRST          :OUT std_logic;
      
      --==========--
      -- MGT(GTX) --
      --==========--                   
      
      -- Serial lanes:
      ----------------
      
      --master
      SFP3_TX_P                                      : out std_logic;
      SFP3_TX_N                                      : out std_logic;
      SFP3_RX_P                                      : in  std_logic;
      SFP3_RX_N                                      : in  std_logic; 
      --slave

      SFP2_RX_P                                      : in  std_logic;
      SFP2_RX_N                                      : in  std_logic; 
           
      -- SFP control:
      ---------------
      
      SFP3_TX_DISABLE                                : out std_logic;  
      SFP2_TX_DISABLE                                : out std_logic;   
    
      --====================--
      -- Signals forwarding --
      --====================--
      
      -- SMA output:
      --------------
      USER_SMA_GPIO_P                                : out std_logic;    
      USER_SMA_GPIO_N                                : out std_logic;

      
        -- 125 MHz clock from MMCM
--      gtx_clk_bufg_out:       OUT STD_LOGIC;
      phy_resetn:               OUT STD_LOGIC;
    
      -- RGMII Interface
      ------------------
      rgmii_txd:                OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
      rgmii_tx_ctl:             OUT STD_LOGIC;
      rgmii_txc:                OUT STD_LOGIC;
      rgmii_rxd:                IN  STD_LOGIC_VECTOR(3 DOWNTO 0);
      rgmii_rx_ctl:             IN  STD_LOGIC;
      rgmii_rxc:                IN  STD_LOGIC;
    
    
    -- MDIO Interface
            -----------------
      mdio:                   INOUT STD_LOGIC;
      mdc :                     OUT STD_LOGIC;

      -- UART Interface
      uart_rxd               : in std_logic;
      uart_txd               : out std_logic
 
); 
end lpgbtfpga_xcku035_10g24_top;

--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture behavioral of lpgbtfpga_xcku035_10g24_top is

    -- Components declaration
    component lpgbtFpga_10g24 is 
       GENERIC (
            FEC                             : integer range 0 to 2                   --! FEC selection can be: FEC5 or FEC12
       );
       PORT (
            -- Clocks
            downlinkClk_i                    : in  std_logic;                       --! 40MHz user clock
            uplinkClk_i                      : in  std_logic;                       --! 40MHz user clock
    
            downlinkRst_i                    : in  std_logic;                       --! Reset the downlink path
            uplinkRst_i                      : in  std_logic;                       --! Reset the uplink path
            
            -- Down link
            downlinkUserData_i               : in  std_logic_vector(31 downto 0);   --! Downlink data (user)
            downlinkEcData_i                 : in  std_logic_vector(1 downto 0);    --! Downlink EC field
            downlinkIcData_i                 : in  std_logic_vector(1 downto 0);    --! Downlink IC field
                    
            downLinkBypassInterleaver_i      : in  std_logic;                       --! Bypass downlink interleaver (test purpose only)
            downLinkBypassFECEncoder_i       : in  std_logic;                       --! Bypass downlink FEC (test purpose only)
            downLinkBypassScrambler_i        : in  std_logic;                       --! Bypass downlink scrambler (test purpose only)
            downlinkReady_o                  : out std_logic;                       --! Downlink ready status
    
            -- Fixed-phase downlink CDC operation
            downlinkPhase_o                  : out  std_logic_vector(9 downto 0);   --! Phase to check fixed-phase
            downlinkPhaseCalib_i             : in   std_logic_vector(9 downto 0);   --! Phase measured in first reset
            downlinkPhaseForce_i             : in   std_logic                   ;   --! Force phase after first reset to ensure fixed-phase operation
    
            -- Up link
            uplinkUserData_o                 : out std_logic_vector(229 downto 0);  --! Uplink data (user)
            uplinkEcData_o                   : out std_logic_vector(1 downto 0);    --! Uplink EC field
            uplinkIcData_o                   : out std_logic_vector(1 downto 0);    --! Uplink IC field
                    
            uplinkBypassInterleaver_i        : in  std_logic;                       --! Bypass uplink interleaver (test purpose only)
            uplinkBypassFECEncoder_i         : in  std_logic;                       --! Bypass uplink FEC (test purpose only)
            uplinkBypassScrambler_i          : in  std_logic;                       --! Bypass uplink scrambler (test purpose only)
    
            uplinkFECCorrectedClear_i        : in  std_logic;                       --! Uplink FEC corrected error clear (debugging)
            uplinkFECCorrectedLatched_o      : out std_logic;                       --! Uplink FEC corrected error latched (debugging)
    
            uplinkReady_o                    : out std_logic;                       --! Uplink ready status
            
            -- Fixed-phase uplink CDC operation
            uplinkPhase_o                    : out  std_logic_vector(2 downto 0);   --! Phase to check fixed-phase
            uplinkPhaseCalib_i               : in   std_logic_vector(2 downto 0);   --! Phase measured in first reset
            uplinkPhaseForce_i               : in   std_logic;                      --! Force the phase to be the calibrated one
    
            -- MGT
            clk_mgtrefclk_i                  : in  std_logic;                       --! Transceiver serial clock
            clk_mgtfreedrpclk_i              : in  std_logic;                       --! 125MHz Free-running clock
            
            clk_mgtTxClk_o                   : out std_logic;
            clk_mgtRxClk_o                   : out std_logic;
                        
            mgt_rxn_i                        : in  std_logic;
            mgt_rxp_i                        : in  std_logic;
            mgt_txn_o                        : out std_logic;
            mgt_txp_o                        : out std_logic;
                   
            mgt_txpolarity_i                 : in  std_logic;      
            mgt_rxpolarity_i                 : in  std_logic;      
           
            mgt_txcaliben_i                  : in  std_logic;
            mgt_txcalib_i                    : in  std_logic_vector(6 downto 0);                      
            mgt_txaligned_o                  : out std_logic;
            mgt_txphase_o                    : out std_logic_vector(6 downto 0)        
       ); 
    end component lpgbtFpga_10g24;


COMPONENT debug_jtag_master
  PORT (
    aclk : IN STD_LOGIC;
    aresetn : IN STD_LOGIC;
    m_axi_awaddr : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_awprot : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_awvalid : OUT STD_LOGIC;
    m_axi_awready : IN STD_LOGIC;
    m_axi_wdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_wstrb : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    m_axi_wvalid : OUT STD_LOGIC;
    m_axi_wready : IN STD_LOGIC;
    m_axi_bresp : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_bvalid : IN STD_LOGIC;
    m_axi_bready : OUT STD_LOGIC;
    m_axi_araddr : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_arprot : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
    m_axi_arvalid : OUT STD_LOGIC;
    m_axi_arready : IN STD_LOGIC;
    m_axi_rdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    m_axi_rresp : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
    m_axi_rvalid : IN STD_LOGIC;
    m_axi_rready : OUT STD_LOGIC
  );
END COMPONENT;

component xlx_k7v7_tx_pll
port
 (-- Clock in ports
    clk_in1           : in     std_logic;
    -- Clock out ports
    txFrameClk        : out    std_logic;
    sys_clk_160       : out    std_logic;
    -- Status and control signals
    reset             : in     std_logic;
    locked            : out    std_logic  
 );
end component;   


component gbtsc_controller is
    generic(
        -- Width of S_AXI data bus.
        C_S_AXI_DATA_WIDTH      : integer                       := 32;
        -- Width of S_AXI address bus.
        C_S_AXI_ADDR_WIDTH      : integer                       := 5
    );
    Port ( 
        -- AXI4LITE Interface
        tx_clock            : in  std_logic;
        meas_clock          : in  std_logic;
        
        -- AXI4LITE Interface
        S_AXI_ARESETN       : in  std_logic;
        S_AXI_AWADDR        : in  std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
        S_AXI_AWVALID       : in  std_logic;
        S_AXI_AWREADY       : out std_logic;
        S_AXI_WDATA         : in  std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
        S_AXI_WSTRB         : in  std_logic_vector((C_S_AXI_DATA_WIDTH / 8) - 1 downto 0);
        S_AXI_WVALID        : in  std_logic;
        S_AXI_WREADY        : out std_logic;
        S_AXI_BRESP         : out std_logic_vector(1 downto 0);
        S_AXI_BVALID        : out std_logic;
        S_AXI_BREADY        : in  std_logic;
        S_AXI_ARADDR        : in  std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
        S_AXI_ARVALID       : in  std_logic;
        S_AXI_ARREADY       : out std_logic;
        S_AXI_RDATA         : out std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
        S_AXI_RRESP         : out std_logic_vector(1 downto 0);
        S_AXI_RVALID        : out std_logic;
        S_AXI_RREADY        : in  std_logic;
        
        -- To GBT-SC
        reset_gbtsc         : out std_logic;
        start_reset         : out std_logic;
        start_connect       : out std_logic;
        start_command       : out std_logic;
        
        tx_address          : out std_logic_vector(7 downto 0);
        tx_transID          : out std_logic_vector(7 downto 0);
        tx_channel          : out std_logic_vector(7 downto 0);
        tx_len              : out std_logic_vector(7 downto 0);
        tx_command          : out std_logic_vector(7 downto 0);
        tx_data             : out std_logic_vector(31 downto 0);
        
        rx_reply_received_i : in std_logic;
        rx_address          : in  std_logic_vector(7 downto 0);
        rx_transID          : in  std_logic_vector(7 downto 0);
        rx_channel          : in  std_logic_vector(7 downto 0);
        rx_len              : in  std_logic_vector(7 downto 0);
        rx_error            : in  std_logic_vector(7 downto 0);
        rx_data             : in  std_logic_vector(31 downto 0);
                
        -- To GBT-IC
        ic_ready            : in  std_logic;
        ic_empty                : in  std_logic;
        
        -- Configuration
        GBTx_address        : out std_logic_vector(7 downto 0);
        Register_addr        : out std_logic_vector(15 downto 0);
        nb_to_be_read        : out std_logic_vector(15 downto 0);
        
        -- Control        
        start_write            : out std_logic;
        start_read            : out std_logic;
        
        -- WRITE register(s)
        data_o                : out std_logic_vector(7 downto 0);
        wr                    : out std_logic;
        
        -- READ register(s)        
        data_i                 : in  std_logic_vector(7 downto 0);
        rd                    : out std_logic;
                
        rd_gbtx_addr        : in  std_logic_vector(7 downto 0);
        rd_mem_ptr            : in  std_logic_vector(15 downto 0);
        rd_nb_of_words        : in  std_logic_vector(15 downto 0);
                
        -- Status
        delay_cnter_o       : out std_logic_vector(31 downto 0);
        wdt_error_o         : out std_logic;
        ready_o             : out std_logic
    );
end component;



    
    COMPONENT vio_0
      PORT (
        clk : IN STD_LOGIC;
        probe_in0 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_in1 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_in2 : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
        probe_in3 : IN STD_LOGIC_VECTOR(27 DOWNTO 0);
        probe_in4 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_in5 : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
        probe_in6 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_in7 : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
        probe_in8 : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
        
        probe_out0 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out1 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out2 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out3 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out4 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out5 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out6 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out7 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out8 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out9 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out10 : OUT STD_LOGIC_VECTOR(6 DOWNTO 0);
        probe_out11 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out12 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out13 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out14 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out15 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out16 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out17 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out18 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out19 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out20 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out21 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out22 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out23 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out24 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out25 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out26 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out27 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out28 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out29 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out30 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out31 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out32 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out33 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out34 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out35 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out36 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out37 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out38 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out39 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out40 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out41 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out42 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out43 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out44 : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        probe_out45 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out46 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out47 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out48 : OUT STD_LOGIC_VECTOR(9 DOWNTO 0);
        probe_out49 : OUT STD_LOGIC_VECTOR(2 DOWNTO 0);
        probe_out50 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        probe_out51 : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
      );
    END COMPONENT;
          
    COMPONENT lpgbtfpga_patterngen is
      port(
          clk320DnLink_i            : in  std_logic;
          clkEnDnLink_i             : in  std_logic;
    
          generator_rst_i           : in  std_logic;
    
          config_group0_i           : in  std_logic_vector(1 downto 0);
          config_group1_i           : in  std_logic_vector(1 downto 0);
          config_group2_i           : in  std_logic_vector(1 downto 0);
          config_group3_i           : in  std_logic_vector(1 downto 0);
    
          fixed_pattern_i           : in  std_logic_vector(31 downto 0);
    
          downlink_o                : out std_logic_vector(31 downto 0);
    
          eport_gen_rdy_o           : out std_logic_vector(15 downto 0)
      );
    END COMPONENT;
        
--    COMPONENT lpgbtfpga_patternchecker
--        port (
--            reset_checker_i  : in  std_logic;
--            ser320_clk_i     : in  std_logic;
--            ser320_clkEn_i   : in  std_logic;
    
--            data_rate_i      : in  std_logic;
    
--            elink_config_i   : in  conf2b_array(27 downto 0);
    
--            error_detected_o : out std_logic_vector(27 downto 0);
    
--            userDataUpLink_i : in  std_logic_vector(229 downto 0)
--        );
--    END COMPONENT;

COMPONENT tdc_decoder_top 
  Port (
        rst_in  : in  std_logic;
        clk_40     : in  std_logic;

        trigger_in       : in  std_logic;
        --error_detected_o : out std_logic_vector(27 downto 0);
        encode_ttc       : out std_logic_vector(1 downto 0);
        hit              : out STD_LOGIC_VECTOR(23 DOWNTO 0);
        mezz_TRST        : out std_logic;
        SCA0_PRI       : out std_logic_vector(7 downto 0);
        SCA0_AUX       : out std_logic_vector(7 downto 0);
        SCA1_PRI       : out std_logic_vector(7 downto 0);
        SCA1_AUX       : out std_logic_vector(7 downto 0);
        SCA2_PRI       : out std_logic_vector(7 downto 0);
        SCA2_AUX       : out std_logic_vector(7 downto 0);

        userDataUpLink_m : in  std_logic_vector(229 downto 0);
        userDataUpLink_s : in  std_logic_vector(229 downto 0);
        
        sys_clk_160            : in  std_logic;           
        -- ethernet interface
        tx_axis_clk        : in std_logic;
        tx_axis_fifo_tready: in  std_logic;
        tx_axis_fifo_tdata : out std_logic_vector(7 downto 0);
        tx_axis_fifo_tvalid: out std_logic;
        tx_axis_fifo_tlast : out std_logic
        
        
         );
end COMPONENT;


COMPONENT sys_clk
    port 
     (-- Clock in ports
      -- Clock out ports
      mgt_freedrpclk    : out    std_logic;
      clk_200           : out    std_logic;
      -- Status and control signals
      locked            : out    std_logic;
      clk_in1_p         : in     std_logic;
      clk_in1_n         : in     std_logic
     );
end COMPONENT;

COMPONENT tri_mode_ethernet_mac_0_example_design
  PORT (
    glbl_rst:               IN  STD_LOGIC;

    -- 200MHz clock input from board
    clk_200:                IN  STD_LOGIC;

    -- 125 MHz clock from MMCM
    gtx_clk_bufg_out:       OUT STD_LOGIC;
    phy_resetn:             OUT STD_LOGIC;

    -- RGMII Interface
    ------------------
    rgmii_txd:              OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
    rgmii_tx_ctl:           OUT STD_LOGIC;
    rgmii_txc:              OUT STD_LOGIC;
    rgmii_rxd:              IN  STD_LOGIC_VECTOR(3 DOWNTO 0);
    rgmii_rx_ctl:           IN  STD_LOGIC;
    rgmii_rxc:              IN  STD_LOGIC;

    -- MDIO Interface
    -----------------
    mdio:                 INOUT STD_LOGIC;
    mdc :                   OUT STD_LOGIC;

--  -- USER side RX AXI-S interface  
--  rx_axis_fifo_tdata:     OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
--  rx_axis_fifo_tvalid:    OUT STD_LOGIC;
--  rx_axis_fifo_tready:    IN  STD_LOGIC;
--  rx_axis_fifo_tlast:     OUT STD_LOGIC;  

    -- USER side TX AXI-S interface       
    tx_axis_fifo_tdata:     IN  STD_LOGIC_VECTOR(7 DOWNTO 0);
    tx_axis_fifo_tvalid:    IN  STD_LOGIC;
    tx_axis_fifo_tready:    OUT STD_LOGIC;
    tx_axis_fifo_tlast:     IN  STD_LOGIC
  );
END COMPONENT;



component gbtic_controller is
    Port ( 
        clk                 : in  std_logic;
        rst                 : in  std_logic;
               
        -- To GBT-IC
        ic_ready            : in  std_logic;
        ic_empty            : in  std_logic;
        
        -- Configuration
        GBTx_address        : out std_logic_vector(7 downto 0);
        Register_addr       : out std_logic_vector(15 downto 0);
        nb_to_be_read       : out std_logic_vector(15 downto 0);
        
        -- Control        
        ic_wr_start         : out std_logic;
        ic_rd_start         : out std_logic;
        
        -- WRITE register(s)
        ic_wfifo_data       : out std_logic_vector(7 downto 0);
        ic_fifo_wr          : out std_logic;
        
        -- READ register(s)        
        ic_rfifo_data       : in  std_logic_vector(7 downto 0);
        ic_fifo_rd          : out std_logic;
                
        -- uart interface
        rxd                 : in  std_logic;
        txd                 : out std_logic;

        -- Data to GBT-SC State machine
        reset_gbtsc         : out std_logic;
        start_reset         : out std_logic;
        start_connect       : out std_logic;
        start_command       : out std_logic;
        sca_enable_array    : out std_logic_vector(2 downto 0);
        
            
        --tx_ready_i          : in std_logic;
        tx_address          : out std_logic_vector(7 downto 0);
        tx_transID          : out std_logic_vector(7 downto 0);
        tx_channel          : out std_logic_vector(7 downto 0);
        tx_len              : out std_logic_vector(7 downto 0);
        tx_command          : out std_logic_vector(7 downto 0);
        tx_data             : out std_logic_vector(31 downto 0);
        
        rx_reply_received_i : in  std_logic_vector(2 downto 0);
        rx_address          : in  std_logic_vector(23 downto 0);
        rx_transID          : in  std_logic_vector(23 downto 0);
        rx_channel          : in  std_logic_vector(23 downto 0);
        rx_len              : in  std_logic_vector(23 downto 0);
        rx_error            : in  std_logic_vector(23 downto 0);
        rx_data             : in  std_logic_vector(95 downto 0)
    );
end component;




   
    -- Signals:
            
        -- Clocks:
        signal mgtRefClk_from_smaMgtRefClkbuf_s   : std_logic;

        signal mgt_freedrpclk_s                   : std_logic;
        
        signal lpgbtfpga_mgttxclk_s               : std_logic;
        signal lpgbtfpga_mgtrxclk_s               : std_logic;
        
        
        -- User CDC for lpGBT-FPGA      
        signal lpgbtfpga_clk40                    : std_logic;      
        signal uplinkPhase_s                      : std_logic_vector(2 downto 0);
        signal uplinkPhaseCalib_s                 : std_logic_vector(2 downto 0);
        signal uplinkPhaseForce_s                 : std_logic;                   
                                                                                 
        signal downlinkPhase_s                    : std_logic_vector(9 downto 0);
        signal downlinkPhaseCalib_s               : std_logic_vector(9 downto 0);
        signal downlinkPhaseForce_s               : std_logic                   ;
        
        -- LpGBT-FPGA                             
        signal lpgbtfpga_downlinkrst_s            : std_logic := '1';
        signal lpgbtfpga_downlinkrdy_s            : std_logic;
        signal lpgbtfpga_uplinkrst_s              : std_logic;
        signal lpgbtfpga_uplinkrdy_s              : std_logic;
        
        signal lpgbtfpga_downlinkUserData_s       : std_logic_vector(31 downto 0);
        signal lpgbtfpga_downlinkUserData_tmp     : std_logic_vector(31 downto 0);
                
        signal lpgbtfpga_downlinkEcData_s         : std_logic_vector(1 downto 0);
        signal lpgbtfpga_downlinkIcData_s         : std_logic_vector(1 downto 0);
        
        signal lpgbtfpga_uplinkUserData_s         : std_logic_vector(229 downto 0);
        signal lpgbtfpga_uplinkEcData_s           : std_logic_vector(1 downto 0);
        signal lpgbtfpga_uplinkIcData_s           : std_logic_vector(1 downto 0);
        signal lpgbtfpga_uplinkclk_s              : std_logic;

        signal lpgbtfpga_mgt_txpolarity_s         : std_logic;
        signal lpgbtfpga_mgt_rxpolarity_s         : std_logic;
        signal lpgbtfpga_mgt_txaligned_s          : std_logic;
        signal lpgbtfpga_mgt_txpiphase_s          : std_logic_vector(6 downto 0);
        signal lpgbtfpga_mgt_txpicalib_s          : std_logic_vector(6 downto 0);
        signal lpgbtfpga_mgt_txcaliben_s          : std_logic;
                                
        signal downLinkBypassInterleaver_s        : std_logic := '0';
        signal downLinkBypassFECEncoder_s         : std_logic := '0';
        signal downLinkBypassScrambler_s          : std_logic := '0';
        
        signal upLinkScramblerBypass_s            : std_logic := '0';
        signal upLinkFecBypass_s                  : std_logic := '0';
        signal upLinkInterleaverBypass_s          : std_logic := '0';
        
        signal upLinkFECCorrectedClear_s          : std_logic := '0';
        signal upLinkFECCorrectedLatched_s        : std_logic       ;
        
        -- Config
        signal uplinkSelectDataRate_s             : std_logic := '0';

        signal generator_rst_s                    : std_logic;
        signal downconfig_g0_s                    : std_logic_vector(1 downto 0);
        signal downconfig_g1_s                    : std_logic_vector(1 downto 0);
        signal downconfig_g2_s                    : std_logic_vector(1 downto 0);
        signal downconfig_g3_s                    : std_logic_vector(1 downto 0);        
        signal downlink_gen_rdy_s                 : std_logic_vector(15 downto 0);
    
        signal upelink_config_s                   : conf2b_array(27 downto 0);
        signal uperror_detected_s                 : std_logic_vector(27 downto 0);
        signal reset_upchecker_s                  : std_logic;
        
        
------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------

    --SCA debug
    -----------
    signal sca_enable_array : std_logic_vector(2 downto 0);
    signal reset_sca: std_logic;
    signal sca_ready: std_logic;
    signal start_sca: std_logic;
    signal start_reset: std_logic;
    signal start_connect: std_logic;
    
    signal sca_rx_parr: std_logic_vector(159 downto 0);
    --signal sca_rx_done: std_logic;
    
    --signal ec_line_tx  : std_logic_vector(1 downto 0);
    --signal ec_line_rx  : std_logic_vector(1 downto 0);
    
    signal tx_ready: std_logic;
    signal tx_addr: std_logic_vector(7 downto 0);
    signal tx_ctrl: std_logic_vector(7 downto 0);
    signal tx_trid: std_logic_vector(7 downto 0);
    signal tx_ch: std_logic_vector(7 downto 0);
    signal tx_len: std_logic_vector(7 downto 0);
    signal tx_cmd: std_logic_vector(7 downto 0);
    signal tx_data: std_logic_vector(31 downto 0);
    
    --signal rx_addr: std_logic_vector(7 downto 0);
    --signal rx_ctrl: std_logic_vector(7 downto 0);
    --signal rx_trid: std_logic_vector(7 downto 0);
    --signal rx_ch: std_logic_vector(7 downto 0);
    --signal rx_len: std_logic_vector(7 downto 0);
    --signal rx_err: std_logic_vector(7 downto 0);
    --signal rx_data: std_logic_vector(31 downto 0);
    

    signal sca_rx_done : std_logic_vector(2 downto 0);   --! Reply received flag (pulse)
    signal rx_addr     : reg8_arr(2 downto 0);           --! Reply: address field (According to the SCA manual)
    signal rx_ctrl     : reg8_arr(2 downto 0);           --! Reply: control field (According to the SCA manual)
    signal rx_trid     : reg8_arr(2 downto 0);           --! Reply: transaction ID field (According to the SCA manual)
    signal rx_ch       : reg8_arr(2 downto 0);           --! Reply: channel field (According to the SCA manual)
    signal rx_len      : reg8_arr(2 downto 0);           --! Reply: len field (According to the SCA manual)
    signal rx_err      : reg8_arr(2 downto 0);           --! Reply: error field (According to the SCA manual)
    signal rx_data     : reg32_arr(2 downto 0);          --! Reply: data field (According to the SCA manual)

    signal rx_addr_24  : std_logic_vector(23 downto 0);
    signal rx_ctrl_24  : std_logic_vector(23 downto 0);
    signal rx_trid_24  : std_logic_vector(23 downto 0);
    signal rx_ch_24    : std_logic_vector(23 downto 0);
    signal rx_len_24   : std_logic_vector(23 downto 0);
    signal rx_err_24   : std_logic_vector(23 downto 0);
    signal rx_data_96  : std_logic_vector(95 downto 0);


        -- EC line
    signal ec_line_tx   : reg2_arr(2 downto 0);           --! (TX) Array of bits to be mapped to the TX GBT-Frame
    signal ec_line_rx   : reg2_arr(2 downto 0);           --! (RX) Array of bits to be mapped to the RX GBT-Frame

            
    --signal m_axi_awaddr         :  STD_LOGIC_VECTOR(31 DOWNTO 0);
    --signal m_axi_awprot         :  STD_LOGIC_VECTOR(2 DOWNTO 0);
    --signal m_axi_awvalid         :  STD_LOGIC;
    --signal m_axi_awready         :  STD_LOGIC;
    --signal m_axi_wdata         :  STD_LOGIC_VECTOR(31 DOWNTO 0);
    --signal m_axi_wstrb         :  STD_LOGIC_VECTOR(3 DOWNTO 0);
    --signal m_axi_wvalid         :  STD_LOGIC;
    --signal m_axi_wready         :  STD_LOGIC;
    --signal m_axi_bresp         :  STD_LOGIC_VECTOR(1 DOWNTO 0);
    --signal m_axi_bvalid         :  STD_LOGIC;
    --signal m_axi_bready         :  STD_LOGIC;
    --signal m_axi_araddr         :  STD_LOGIC_VECTOR(31 DOWNTO 0);
    --signal m_axi_arprot         :  STD_LOGIC_VECTOR(2 DOWNTO 0);
    --signal m_axi_arvalid         :  STD_LOGIC;
    --signal m_axi_arready         :  STD_LOGIC;
    --signal m_axi_rdata         :  STD_LOGIC_VECTOR(31 DOWNTO 0);
    --signal m_axi_rresp         :  STD_LOGIC_VECTOR(1 DOWNTO 0);
    --signal m_axi_rvalid         :  STD_LOGIC;
    --signal m_axi_rready         :  STD_LOGIC;


    -- IC Debug
    ------------
    signal ic_ready                   : std_logic;
    signal ic_empty                   : std_logic;
    signal GBTx_address_to_gbtic      : std_logic_vector(7 downto 0);
    signal Register_addr_to_gbtic     : std_logic_vector(15 downto 0);
    signal nb_to_be_read_to_gbtic     : std_logic_vector(15 downto 0);
    signal start_write_to_gbtic       : std_logic;
    signal start_read_to_gbtic        : std_logic;
    signal data_to_gbtic              : std_logic_vector(7 downto 0);
    signal wr_to_gbtic                : std_logic;
    signal data_from_gbtic            : std_logic_vector(7 downto 0);
    signal rd_to_gbtic                : std_logic;
    signal ic_rd_gbtx_addr            : std_logic_vector(7 downto 0);
    signal ic_rd_mem_ptr              : std_logic_vector(15 downto 0);
    signal ic_rd_nb_of_words          : std_logic_vector(15 downto 0);
    
    signal GBTx_address         : std_logic_vector(7 downto 0);
    signal Register_ptr         : std_logic_vector(15 downto 0);
    signal ic_start_wr          : std_logic;
    signal ic_wr                : std_logic;
    signal ic_data_wr           : std_logic_vector(7 downto 0);
    signal ic_start_rd          : std_logic;
    signal ic_nb_to_be_read_rd  : std_logic_vector(15 downto 0);
    signal ic_empty_rd          : std_logic;
    signal ic_rd                : std_logic;
    signal ic_data_rd           : std_logic_vector(7 downto 0);
    
    signal GBTx_rd_addr         : std_logic_vector(7 downto 0);
    signal GBTx_rd_mem_ptr      : std_logic_vector(15 downto 0);
    signal GBTx_rd_mem_size     : std_logic_vector(15 downto 0);
             
    signal txFrameClkPllLocked_from_gbtExmplDsgn      : std_logic;
    signal elink_320m_sca0_pri          : std_logic_vector(7 downto 0); 
    signal elink_320m_sca0_aux          : std_logic_vector(7 downto 0); 
    signal elink_320m_sca1_pri          : std_logic_vector(7 downto 0); 
    signal elink_320m_sca1_aux          : std_logic_vector(7 downto 0); 
    signal elink_320m_sca2_pri          : std_logic_vector(7 downto 0); 
    signal elink_320m_sca2_aux          : std_logic_vector(7 downto 0); 
    signal up_elink_80m_sca0_pri        : std_logic_vector(1 downto 0);  
    signal up_elink_80m_sca0_aux        : std_logic_vector(1 downto 0);  
    signal up_elink_80m_sca1_pri        : std_logic_vector(1 downto 0);  
    signal up_elink_80m_sca1_aux        : std_logic_vector(1 downto 0);  
    signal up_elink_80m_sca2_pri        : std_logic_vector(1 downto 0);  
    signal up_elink_80m_sca2_aux        : std_logic_vector(1 downto 0);  
  
    signal down_elink_80m_sca0_pri        : std_logic_vector(1 downto 0);
    signal down_elink_80m_sca0_aux        : std_logic_vector(1 downto 0); 
    signal down_elink_80m_sca1_pri        : std_logic_vector(1 downto 0); 
    signal down_elink_80m_sca1_aux        : std_logic_vector(1 downto 0);
    signal down_elink_80m_sca2_pri        : std_logic_vector(1 downto 0);
    signal down_elink_80m_sca2_aux        : std_logic_vector(1 downto 0); 


    -- Frame clock:
    ---------------
    signal txFrameClk_from_txPll                     : std_logic;
    
    signal fast_clock_for_meas  : std_logic;
    signal cmd_delay            : std_logic_vector(31 downto 0); 

    signal ttc_ctrl             : std_logic_vector(1 downto 0); 
    signal trigger_SMA          : std_logic; 
    
    signal LOCK_TOP             : std_logic;
    
    signal sys_clk_160                    : std_logic; 
    signal clk_200                        : std_logic;   
    signal tx_axis_clk                    : std_logic;    
    signal tx_axis_fifo_tdata             : STD_LOGIC_VECTOR(7 DOWNTO 0); 
    signal tx_axis_fifo_tvalid            : std_logic;  
    signal tx_axis_fifo_tready            : std_logic;  
    signal tx_axis_fifo_tlast             : std_logic;  

begin                 --========####   Architecture Body   ####========-- 

    -- Reset controll    
    SFP3_TX_DISABLE           <= '0';
    SFP2_TX_DISABLE           <= '0';

    rx_addr_24  <= rx_addr(2) & rx_addr(1) & rx_addr(0);
    rx_ctrl_24  <= rx_ctrl(2) & rx_ctrl(1) & rx_ctrl(0);
    rx_trid_24  <= rx_trid(2) & rx_trid(1) & rx_trid(0);
    rx_ch_24    <= rx_ch  (2) & rx_ch  (1) & rx_ch  (0);
    rx_len_24   <= rx_len (2) & rx_len (1) & rx_len (0);
    rx_err_24   <= rx_err (2) & rx_err (1) & rx_err (0);
    rx_data_96  <= rx_data(2) & rx_data(1) & rx_data(0);

    down_elink_80m_sca0_aux <= ec_line_tx(0);
    down_elink_80m_sca1_pri <= ec_line_tx(1);
    down_elink_80m_sca2_pri <= ec_line_tx(2);
    ec_line_rx(2)  <= up_elink_80m_sca2_pri;
    ec_line_rx(1)  <= up_elink_80m_sca1_pri;
    ec_line_rx(0)  <= up_elink_80m_sca0_aux;
    -- Clocks
    
    -- MGT(GTX) reference clock:
    ----------------------------   
    -- Comment: * The MGT reference clock MUST be provided by an external clock generator.
    --          * The MGT reference clock frequency must be 320MHz for the latency-optimized GBT Bank. 
    smaMgtRefClkIbufdsGtxe2: ibufds_gte3
      generic map(
        REFCLK_EN_TX_PATH           => '0',
        REFCLK_HROW_CK_SEL          => (others => '0'),
        REFCLK_ICNTL_RX             => (others => '0')
      )
      port map (
        O                                           => mgtRefClk_from_smaMgtRefClkbuf_s,
        ODIV2                                       => open,
        CEB                                         => '0',
        I                                           => SMA_MGT_REFCLK_P,
        IB                                          => SMA_MGT_REFCLK_N
      );
      
   -- Frame clock
   txPll: xlx_k7v7_tx_pll
      port map (
        --input clock
        clk_in1     => lpgbtfpga_clk40,

        --output clocks
        txFrameClk  => txFrameClk_from_txPll,
        sys_clk_160 => sys_clk_160,
        -----------------------------------------  
        RESET       => '0',
        LOCKED      => txFrameClkPllLocked_from_gbtExmplDsgn
      );
 LOCK_TOP   <=  txFrameClkPllLocked_from_gbtExmplDsgn;          
        
--    userClockIbufgds: ibufgds
--      generic map (
--         IBUF_LOW_PWR                                => FALSE,      
--         IOSTANDARD                                  => "LVDS")
--      port map (     
--         O                                           => mgt_freedrpclk_s,   
--         I                                           => USER_CLOCK_P,  
--         IB                                          => USER_CLOCK_N 
--      );
      
IBUFDS_inst: IBUFDS
generic map (
    IBUF_LOW_PWR => TRUE, 
    IOSTANDARD => "LVDS")
port map (
    O  => trigger_SMA, 
    I  => TRIGGER0_P, 
    IB => TRIGGER0_N 
);    

    
    sys_clk_inst : sys_clk
       port map ( 
      -- Clock out ports  
       mgt_freedrpclk => mgt_freedrpclk_s,
       clk_200        => clk_200,
      -- Status and control signals                
       -- Clock in ports
       clk_in1_p      => USER_CLOCK_P,
       clk_in1_n      => USER_CLOCK_N
     );
      
            
  -- In this example design, the 40MHz clock used for the user logic is derived from a division of the Tx user clock of the MGT
  -- It should be noted, that in realistic cases, this clock typically comes from an external PLL (sync. to the MGT Tx reference clock)
  lpgbtfpga_clk40_inst : bufgce_div
    generic map (
      BUFGCE_DIVIDE => 8
    )
    port map (
      i   => lpgbtfpga_mgttxclk_s,   --320M clk_mgtTxClk_o
      o   => lpgbtfpga_clk40,        --
      ce  => '1',
      clr => '0'
    );  

    -- LpGBT FPGA
    lpgbtFpga_top_inst: lpgbtFpga_10g24 
       generic map (
            FEC                             => FEC5
       )
       port map (
            -- Clocks
            downlinkClk_i                    => lpgbtfpga_clk40,
            uplinkClk_i                      => lpgbtfpga_clk40,

            downlinkRst_i                    => lpgbtfpga_downlinkrst_s,
            uplinkRst_i                      => lpgbtfpga_uplinkrst_s,

            -- Down link
            downlinkUserData_i               => lpgbtfpga_downlinkUserData_tmp,  --lpgbtfpga_downlinkUserData_s,
            downlinkEcData_i                 => lpgbtfpga_downlinkEcData_s,
            downlinkIcData_i                 => lpgbtfpga_downlinkIcData_s,

            downLinkBypassInterleaver_i      => downLinkBypassInterleaver_s,
            downLinkBypassFECEncoder_i       => downLinkBypassFECEncoder_s,
            downLinkBypassScrambler_i        => downLinkBypassScrambler_s,

            downlinkReady_o                  => lpgbtfpga_downlinkrdy_s,

            -- Fixed-phase downlink CDC operation
            downlinkPhase_o                  => downlinkPhase_s     ,
            downlinkPhaseCalib_i             => downlinkPhaseCalib_s,
            downlinkPhaseForce_i             => downlinkPhaseForce_s,

            -- Up link
            uplinkUserData_o                 => lpgbtfpga_uplinkUserData_s,
            uplinkEcData_o                   => lpgbtfpga_uplinkEcData_s,
            uplinkIcData_o                   => lpgbtfpga_uplinkIcData_s,

            uplinkBypassInterleaver_i        => upLinkInterleaverBypass_s,
            uplinkBypassFECEncoder_i         => upLinkFecBypass_s,
            uplinkBypassScrambler_i          => upLinkScramblerBypass_s,
            
            uplinkFECCorrectedClear_i        => upLinkFECCorrectedClear_s,
            uplinkFECCorrectedLatched_o      => upLinkFECCorrectedLatched_s,
        
            uplinkReady_o                    => lpgbtfpga_uplinkrdy_s,

            -- Fixed-phase uplink CDC operation
            uplinkPhase_o                    => uplinkPhase_s     ,
            uplinkPhaseCalib_i               => uplinkPhaseCalib_s,
            uplinkPhaseForce_i               => uplinkPhaseForce_s,

            -- MGT
            clk_mgtrefclk_i                  => mgtRefClk_from_smaMgtRefClkbuf_s,
            clk_mgtfreedrpclk_i              => mgt_freedrpclk_s,
            
            clk_mgtTxClk_o                   => lpgbtfpga_mgttxclk_s,
            clk_mgtRxClk_o                   => lpgbtfpga_mgtrxclk_s,
            
            mgt_rxn_i                        => SFP3_RX_N,
            mgt_rxp_i                        => SFP3_RX_P,
            mgt_txn_o                        => SFP3_TX_N,
            mgt_txp_o                        => SFP3_TX_P,
            
            mgt_txpolarity_i                 => lpgbtfpga_mgt_txpolarity_s,
            mgt_rxpolarity_i                 => lpgbtfpga_mgt_rxpolarity_s,
            -- HPTD IP
            mgt_txcaliben_i                  => lpgbtfpga_mgt_txcaliben_s,
            mgt_txcalib_i                    => lpgbtfpga_mgt_txpicalib_s,                    
            mgt_txaligned_o                  => lpgbtfpga_mgt_txaligned_s,
            mgt_txphase_o                    => lpgbtfpga_mgt_txpiphase_s
       );
       
    -- Data stimulis
    lpgbtfpga_downlinkEcData_s     <= (others => '1');
    --lpgbtfpga_downlinkIcData_s     <= (others => '1');
    
    -- Data pattern generator / checker (PRBS7)
    lpgbtfpga_patterngen_inst: lpgbtfpga_patterngen
        port map(
            --clk40Mhz_Tx_i      : in  std_logic;
            clk320DnLink_i            => lpgbtfpga_clk40,
            clkEnDnLink_i             => '1',

            generator_rst_i           => generator_rst_s,

            -- Group configurations:
            --    "11": 320Mbps
            --    "10": 160Mbps
            --    "01": 80Mbps
            --    "00": Fixed pattern
            config_group0_i           => downconfig_g0_s,
            config_group1_i           => downconfig_g1_s,
            config_group2_i           => downconfig_g2_s,
            config_group3_i           => downconfig_g3_s,

            downlink_o                => lpgbtfpga_downlinkUserData_s,

            fixed_pattern_i           => x"12345678",

            eport_gen_rdy_o           => downlink_gen_rdy_s
        );

--    lpgbtfpga_patternchecker_inst: lpgbtfpga_patternchecker
--        port map(
--            reset_checker_i  => reset_upchecker_s,
--            ser320_clk_i     => lpgbtfpga_clk40,
--            ser320_clkEn_i   => '1',
    
--            data_rate_i      => uplinkSelectDataRate_s,
    
--            elink_config_i   => upelink_config_s,
    
--            error_detected_o => uperror_detected_s,
    
--            userDataUpLink_i => lpgbtfpga_uplinkUserData_s
--        );
        
 tdc_decoder_top_inst: tdc_decoder_top 
    port map (
        rst_in              => reset_upchecker_s,
        clk_40              => lpgbtfpga_clk40,
        --ser320_clkEn_i      => '1',

        --data_rate_i         => uplinkSelectDataRate_s,

        --elink_config_i    => upelink_config_s,
        trigger_in          => trigger_SMA,
        --error_detected_o    => uperror_detected_s,
        encode_ttc          => ttc_ctrl,
        hit                 => hit,
        mezz_TRST           => mezz_TRST,



        SCA0_PRI            => elink_320m_sca0_pri,
        SCA0_AUX            => elink_320m_sca0_aux,
        SCA1_PRI            => elink_320m_sca1_pri,
        SCA1_AUX            => elink_320m_sca1_aux,
        SCA2_PRI            => elink_320m_sca2_pri,
        SCA2_AUX            => elink_320m_sca2_aux,

        userDataUpLink_m    => lpgbtfpga_uplinkUserData_s,
        userDataUpLink_s    => lpgbtfpga_uplinkUserData_s,
        
        sys_clk_160         => sys_clk_160,
        
        tx_axis_clk         => tx_axis_clk,         
        tx_axis_fifo_tdata  => tx_axis_fifo_tdata,
        tx_axis_fifo_tvalid => tx_axis_fifo_tvalid,
        tx_axis_fifo_tready => tx_axis_fifo_tready,        
        tx_axis_fifo_tlast  => tx_axis_fifo_tlast

        

        
         );


         
        
    vio_debug_inst : vio_0
      PORT MAP (
        clk => mgt_freedrpclk_s,
        probe_in0(0)  => lpgbtfpga_downlinkrdy_s,
        probe_in1(0)  => lpgbtfpga_uplinkrdy_s,
        probe_in2     => downlink_gen_rdy_s,
        probe_in3     => uperror_detected_s,
        probe_in4(0)  => lpgbtfpga_mgt_txaligned_s,
        probe_in5     => lpgbtfpga_mgt_txpiphase_s,
        probe_in6(0)  => upLinkFECCorrectedLatched_s,  
        probe_in7     => downlinkPhase_s,
        probe_in8     => uplinkPhase_s,
        
        probe_out0(0) => lpgbtfpga_downlinkrst_s,
        probe_out1(0) => lpgbtfpga_uplinkrst_s,
        probe_out2(0) => downLinkBypassInterleaver_s,
        probe_out3(0) => downLinkBypassFECEncoder_s,
        probe_out4(0) => downLinkBypassScrambler_s,
        probe_out5(0) => open,
        probe_out6(0) => open,
        probe_out7(0) => upLinkInterleaverBypass_s,
        probe_out8(0) => upLinkFecBypass_s,
        probe_out9(0) => upLinkScramblerBypass_s,
        probe_out10   => lpgbtfpga_mgt_txpicalib_s,
        probe_out11(0)=> lpgbtfpga_mgt_txcaliben_s,
        probe_out12(0)=> generator_rst_s,
        probe_out13   => downconfig_g0_s,
        probe_out14   => downconfig_g1_s,
        probe_out15   => downconfig_g2_s,
        probe_out16   => downconfig_g3_s,
        probe_out17(0)=> reset_upchecker_s,
        probe_out18   => upelink_config_s(0),
        probe_out19   => upelink_config_s(1),
        probe_out20   => upelink_config_s(2),
        probe_out21   => upelink_config_s(3),
        probe_out22   => upelink_config_s(4),
        probe_out23   => upelink_config_s(5),
        probe_out24   => upelink_config_s(6),
        probe_out25   => upelink_config_s(7),
        probe_out26   => upelink_config_s(8),
        probe_out27   => upelink_config_s(9),
        probe_out28   => upelink_config_s(10),
        probe_out29   => upelink_config_s(11),
        probe_out30   => upelink_config_s(12),
        probe_out31   => upelink_config_s(13),
        probe_out32   => upelink_config_s(14),
        probe_out33   => upelink_config_s(15),
        probe_out34   => upelink_config_s(16),
        probe_out35   => upelink_config_s(17),
        probe_out36   => upelink_config_s(18),
        probe_out37   => upelink_config_s(20),
        probe_out38   => upelink_config_s(21),
        probe_out39   => upelink_config_s(22),
        probe_out40   => upelink_config_s(23),
        probe_out41   => upelink_config_s(24),
        probe_out42   => upelink_config_s(25),
        probe_out43   => upelink_config_s(26),
        probe_out44   => upelink_config_s(27),
        probe_out45(0)=> lpgbtfpga_mgt_txpolarity_s,
        probe_out46(0)=> lpgbtfpga_mgt_rxpolarity_s,
        probe_out47(0)=> upLinkFECCorrectedClear_s,
        probe_out48   => downlinkPhaseCalib_s,
        probe_out49   => uplinkPhaseCalib_s,
        probe_out50(0)=> downlinkPhaseForce_s,
        probe_out51(0)=> uplinkPhaseForce_s
      );
      
--      USER_SMA_GPIO_P <= lpgbtfpga_clk40;
      USER_SMA_GPIO_N <= lpgbtfpga_mgtrxclk_s;
      
      
   --jtag_master_inst : debug_jtag_master
   --  PORT MAP (
   --    aclk          =>    txFrameClk_from_txPll,
   --    aresetn       => txFrameClkPllLocked_from_gbtExmplDsgn,
   --    m_axi_awaddr  => m_axi_awaddr,
   --    m_axi_awprot  => m_axi_awprot,
   --    m_axi_awvalid => m_axi_awvalid,
   --    m_axi_awready => m_axi_awready,
   --    m_axi_wdata   => m_axi_wdata,
   --    m_axi_wstrb   => m_axi_wstrb,
   --    m_axi_wvalid  => m_axi_wvalid,
   --    m_axi_wready  => m_axi_wready,
   --    m_axi_bresp   => m_axi_bresp,
   --    m_axi_bvalid  => m_axi_bvalid,
   --    m_axi_bready  => m_axi_bready,
   --    m_axi_araddr  => m_axi_araddr,
   --    m_axi_arprot  => m_axi_arprot,
   --    m_axi_arvalid => m_axi_arvalid,
   --    m_axi_arready => m_axi_arready,
   --    m_axi_rdata   => m_axi_rdata,
   --    m_axi_rresp   => m_axi_rresp,
   --    m_axi_rvalid  => m_axi_rvalid,
   --    m_axi_rready  => m_axi_rready
   --  );
         
-- lpgbtfpga_uplinkUserData_s 320Mbps
-- LpGBT_CSM V2 : sca0_aux -- elink_320m_s(17)
-- elink_320m_s(17)     <= userDataUpLink_i(143 downto 136)

-- elink_320m_sca0_aux (7 downto 0)           <=  lpgbtfpga_uplinkUserData_s(143 downto 136);    
-- up_elink_80m_sca0_aux (1 downto 0)         <=  elink_320m_sca0_aux (5) & elink_320m_sca0_aux (1);    

-- lpgbtfpga_downlinkUserData_tmp (31 downto 0)  <= x"12" & down_elink_80m_sca0_aux (1 downto 0) & "11" & x"45678";

--  LpGBT_CSM V2 : sca1_pri -- elink_320m_s(12)
--  elink_320m_s(12)     <= userDataUpLink_i(103 downto 96)

 --elink_320m_sca1_pri(7 downto 0)           <=  lpgbtfpga_uplinkUserData_s(103 downto 96);
 --elink_320m_sca0_aux(7 downto 0)           <=  lpgbtfpga_uplinkUserData_s(143 downto 136);
 --up_elink_80m_sca1_pri (1 downto 0)         <=  elink_320m_sca1_pri (4) & elink_320m_sca1_pri (0);
 --up_elink_80m_sca1_pri (1 downto 0)         <=  elink_320m_sca1_pri (4) & elink_320m_sca1_pri (0);

    up_elink_80m_sca0_pri (1 downto 0)         <=  elink_320m_sca0_pri (4) & elink_320m_sca0_pri (0);
    up_elink_80m_sca0_aux (1 downto 0)         <=  elink_320m_sca0_aux (4) & elink_320m_sca0_aux (0);
    up_elink_80m_sca1_pri (1 downto 0)         <=  elink_320m_sca1_pri (4) & elink_320m_sca1_pri (0);
    up_elink_80m_sca1_aux (1 downto 0)         <=  elink_320m_sca1_aux (4) & elink_320m_sca1_aux (0);
    up_elink_80m_sca2_pri (1 downto 0)         <=  elink_320m_sca2_pri (4) & elink_320m_sca2_pri (0);
    up_elink_80m_sca2_aux (1 downto 0)         <=  elink_320m_sca2_aux (4) & elink_320m_sca2_aux (0);


 --lpgbtfpga_downlinkUserData_tmp (31 downto 0)  <= x"12345" & down_elink_80m_sca1_pri (1 downto 0) & "11" & x"78"; 
 --lpgbtfpga_downlinkUserData_tmp (31 downto 0)  <= x"12345" & down_elink_80m_sca1_pri (1 downto 0) & "11" & x"7" & "00" & ttc_ctrl; 
    lpgbtfpga_downlinkUserData_tmp (31 downto 0)  <= x"00" & down_elink_80m_sca0_aux & "00" & down_elink_80m_sca0_pri & down_elink_80m_sca2_pri &
                                                down_elink_80m_sca2_aux & down_elink_80m_sca1_aux & down_elink_80m_sca1_pri &
                                                x"00" & ttc_ctrl;

 

   gbtsc_inst: entity work.gbtsc_top
    generic map (
        -- IC configuration
        g_IC_FIFO_DEPTH     => 453,                                 --! Defines the depth of the FIFO used to handle the Internal control (Max. number of words/bytes can be read/write from/to a GBTx)
        g_ToLpGBT           => 1,                        --! 1 to use LpGBT. Otherwise, it should be 0

        -- EC configuration
        g_SCA_COUNT         => 3                                      --! Defines the maximum number of SCA that can be connected to this module
    )
    port map (
        -- Clock & reset
        tx_clk_i            => txFrameClk_from_txPll,                                   --! Tx clock (Tx_frameclk_o from GBT-FPGA IP): must be a multiple of the LHC frequency
        tx_clk_en           => '1',                            --! Tx clock enable signal must be used in case of multi-cycle path(tx_clk_i > LHC frequency). By default: always enabled

        rx_clk_i            => txFrameClk_from_txPll,                                    --! Rx clock (Rx_frameclk_o from GBT-FPGA IP): must be a multiple of the LHC frequency
        rx_clk_en           => '1',                            --! Rx clock enable signal must be used in case of multi-cycle path(rx_clk_i > LHC frequency). By default: always enabled

        rx_reset_i          => reset_sca,                                    --! Reset RX datapath
        tx_reset_i          => reset_sca,                                     --! Reset TX datapath

        -- IC control
        tx_start_write_i    => start_write_to_gbtic,                                    --! Request a write config. to the GBTx (IC)
        tx_start_read_i     => start_read_to_gbtic,                                   --! Request a read config. to the GBTx (IC)

        -- IC configuration
        tx_GBTx_address_i   => GBTx_address_to_gbtic,                 --! I2C address of the GBTx
        tx_register_addr_i  => Register_addr_to_gbtic,                --! Address of the first register to be accessed
        tx_nb_to_be_read_i  => nb_to_be_read_to_gbtic,               --! Number of words/bytes to be read (only for read transactions)

        -- IC FIFO control
        wr_clk_i            => txFrameClk_from_txPll,   -- new signal ??            --! Fifo's writing clock
        tx_wr_i             => wr_to_gbtic,                                  --! Request a write operation into the internal FIFO (Data to GBTx)
        tx_data_to_gbtx_i   => data_to_gbtic,                 --! Data to be written into the internal FIFO

        rd_clk_i            => txFrameClk_from_txPll,   -- new signal ?? 
        rx_rd_i             => rd_to_gbtic,                                   --! Request a read operation of the internal FIFO (GBTx reply)
        rx_data_from_gbtx_o => data_from_gbtic,               --! Data from the FIFO

        -- IC Status
        tx_ready_o          => ic_ready,                                   --! IC core ready for a transaction
        rx_empty_o          => ic_empty,                                  --! Rx FIFO is empty (no reply from GBTx)

        rx_gbtx_addr_o      => ic_rd_gbtx_addr,                 --! I2C address of the GBTx (read from a reply)
        rx_mem_ptr_o        => ic_rd_mem_ptr,               --! I2C address of the first register read/written
        rx_nb_of_words_o    => ic_rd_nb_of_words,               --! Number of words/bytes read/written

        -- SCA control
        sca_enable_i        => sca_enable_array,   --! Enable flag to select SCAs
        start_reset_cmd_i   => start_reset,                                   --! Send a reset command to the enabled SCAs
        start_connect_cmd_i => start_connect,                                  --! Send a connect command to the enabled SCAs
        start_command_i     => start_sca,                                   --! Send the command set in input to the enabled SCAs
        inject_crc_error    => '0',                                  --! Emulate a CRC error

        -- SCA command
        tx_address_i        => tx_addr,                --! Command: address field (According to the SCA manual)
        tx_transID_i        => tx_trid,               --! Command: transaction ID field (According to the SCA manual)
        tx_channel_i        => tx_ch,                --! Command: channel field (According to the SCA manual)
        tx_command_i        => tx_cmd,                --! Command: command field (According to the SCA manual)
        tx_data_i           => tx_data,               --! Command: data field (According to the SCA manual)

        rx_received_o       => sca_rx_done,   --! Reply received flag (pulse)
        rx_address_o        => rx_addr,          --! Reply: address field (According to the SCA manual)
        rx_control_o        => open,          --! Reply: control field (According to the SCA manual)
        rx_transID_o        => rx_trid,          --! Reply: transaction ID field (According to the SCA manual)
        rx_channel_o        => rx_ch,          --! Reply: channel field (According to the SCA manual)
        rx_len_o            => rx_len,          --! Reply: len field (According to the SCA manual)
        rx_error_o          => rx_err,           --! Reply: error field (According to the SCA manual)
        rx_data_o           => rx_data,          --! Reply: data field (According to the SCA manual)

        -- EC line
        ec_data_o           => ec_line_tx, -- downlink -- txData_to_gbtExmplDsgn(81 downto 80),        --! (TX) Array of bits to be mapped to the TX GBT-Frame
        ec_data_i           => ec_line_rx, -- uplink  -- rxData_from_gbtExmplDsgn(81 downto 80),        --! (RX) Array of bits to be mapped to the RX GBT-Frame




        -- IC lines
        ic_data_o           => lpgbtfpga_downlinkIcData_s,                --! (TX) Array of bits to be mapped to the TX GBT-Frame (bits 83/84)
        ic_data_i           => lpgbtfpga_uplinkIcData_s                 --! (RX) Array of bits to be mapped to the RX GBT-Frame (bits 83/84)

    );


   delaymeasclk_inst: BUFG 
       port map (
          I                                           => mgtRefClk_from_smaMgtRefClkbuf_s,
          O                                           => fast_clock_for_meas
       );
  
   --gbtsc_controller_inst: entity work.gbtsc_controller
   --    Port map( 
   --        -- AXI4LITE Interface
   --        tx_clock              => txFrameClk_from_txPll,
   --        meas_clock            => fast_clock_for_meas,
           
   --        -- AXI4LITE Interface
   --        S_AXI_ARESETN         => txFrameClkPllLocked_from_gbtExmplDsgn,
   --        S_AXI_AWADDR          => m_axi_awaddr(4 downto 0),
   --        S_AXI_AWVALID         => m_axi_awvalid,
   --        S_AXI_AWREADY         => m_axi_awready,
   --        S_AXI_WDATA           => m_axi_wdata,
   --        S_AXI_WSTRB           => m_axi_wstrb,
   --        S_AXI_WVALID          => m_axi_wvalid,
   --        S_AXI_WREADY          => m_axi_wready,
   --        S_AXI_BRESP           => m_axi_bresp,
   --        S_AXI_BVALID          => m_axi_bvalid,
   --        S_AXI_BREADY          => m_axi_bready,
   --        S_AXI_ARADDR          => m_axi_araddr(4 downto 0),
   --        S_AXI_ARVALID         => m_axi_arvalid,
   --        S_AXI_ARREADY         => m_axi_arready,
   --        S_AXI_RDATA           => m_axi_rdata,
   --        S_AXI_RRESP           => m_axi_rresp,
   --        S_AXI_RVALID          => m_axi_rvalid,
   --        S_AXI_RREADY          => m_axi_rready,
           
   --        -- To GBT-SC
   --        reset_gbtsc         => reset_sca,
   --        start_reset         => start_reset,
   --        start_connect       => start_connect,
   --        start_command       => start_sca,           
                   
   --        tx_address          => tx_addr,
   --        tx_transID          => tx_trid,
   --        tx_channel          => tx_ch,
   --        tx_len              => tx_len,
   --        tx_command          => tx_cmd,
   --        tx_data             => tx_data,           
           
   --        rx_reply_received_i => sca_rx_done,
   --        rx_address          => rx_addr,
   --        rx_transID          => rx_trid,
   --        rx_channel          => rx_ch,
   --        rx_len              => rx_len,
   --        rx_error            => rx_err,
   --        rx_data             => rx_data,
           
                          
   --       ---- Data to GBT-IC State machine        
   --       --ic_ready             => ic_ready,
   --       --ic_empty             => ic_empty,
          
   --       ---- Configuration
   --       --GBTx_address         => GBTx_address_to_gbtic,
   --       --Register_addr        => Register_addr_to_gbtic,
   --       --nb_to_be_read        => nb_to_be_read_to_gbtic,
          
   --       ---- Control        
   --       --start_write          => start_write_to_gbtic,
   --       --start_read           => start_read_to_gbtic,
          
   --       ---- WRITE register(s)
   --       --data_o               => data_to_gbtic,
   --       --wr                   => wr_to_gbtic,
          
   --       ---- READ register(s)        
   --       --data_i               => data_from_gbtic,
   --       --rd                   => rd_to_gbtic,
                  
   --       --rd_gbtx_addr         => ic_rd_gbtx_addr,
   --       --rd_mem_ptr           => ic_rd_mem_ptr,
   --       --rd_nb_of_words       => ic_rd_nb_of_words,
          
   --        -- Status
   --        delay_cnter_o       => cmd_delay,
   --        wdt_error_o         => open,
   --        ready_o             => open
   --    );


gbtic_controller_inst: gbtic_controller
Port map(
    clk                  => txFrameClk_from_txPll,
    rst                  => reset_upchecker_s,

                  
    -- Data to GBT-IC State machine        
    ic_ready             => ic_ready,
    ic_empty             => ic_empty,

    -- Configuration
    GBTx_address         => GBTx_address_to_gbtic,
    Register_addr        => Register_addr_to_gbtic,
    nb_to_be_read        => nb_to_be_read_to_gbtic,

    -- Control        
    ic_wr_start          => start_write_to_gbtic,
    ic_rd_start          => start_read_to_gbtic,

    -- WRITE register(s)
    ic_wfifo_data        => data_to_gbtic,
    ic_fifo_wr           => wr_to_gbtic,

    -- READ register(s)        
    ic_rfifo_data        => data_from_gbtic,
    ic_fifo_rd           => rd_to_gbtic,
          
    -- UART interface
    rxd                  => uart_rxd,
    txd                  => uart_txd,

    -- To GBT-SC
    reset_gbtsc         => reset_sca,
    start_reset         => start_reset,
    start_connect       => start_connect,
    start_command       => start_sca,  
    sca_enable_array    => sca_enable_array,         
           
    tx_address          => tx_addr,
    tx_transID          => tx_trid,
    tx_channel          => tx_ch,
    tx_len              => tx_len,
    tx_command          => tx_cmd,
    tx_data             => tx_data,           

    rx_reply_received_i => sca_rx_done,
    rx_address          => rx_addr_24,
    rx_transID          => rx_trid_24,
    rx_channel          => rx_ch_24,
    rx_len              => rx_len_24,
    rx_error            => rx_err_24,
    rx_data             => rx_data_96
);

   

   ethernet_mac : tri_mode_ethernet_mac_0_example_design
    port map (
    glbl_rst            =>reset_upchecker_s,
    clk_200             =>clk_200,

    -- 125 MHz clock from MMCM
    gtx_clk_bufg_out    =>tx_axis_clk,
    phy_resetn          =>phy_resetn,

    -- RGMII terface
    ------------------
    rgmii_txd           =>rgmii_txd,
    rgmii_tx_ctl        =>rgmii_tx_ctl,
    rgmii_txc           =>rgmii_txc,
    rgmii_rxd           =>rgmii_rxd,
    rgmii_rx_ctl        =>rgmii_rx_ctl,
    rgmii_rxc           =>rgmii_rxc,

    -- MDIO terface
    -----------------
    mdio                =>mdio,
    mdc                 =>mdc,

    -- USER side TX AXI-S terface       
    tx_axis_fifo_tdata  =>tx_axis_fifo_tdata,
    tx_axis_fifo_tvalid =>tx_axis_fifo_tvalid,
    tx_axis_fifo_tready =>tx_axis_fifo_tready,
    tx_axis_fifo_tlast  =>tx_axis_fifo_tlast
    );

             
    
end behavioral;




   
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--
