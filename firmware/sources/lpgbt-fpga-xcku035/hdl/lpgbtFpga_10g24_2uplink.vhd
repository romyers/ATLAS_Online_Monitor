--    _____    _____   __  __            _      _____   _____ ____ _______ 
--   / ____|  / ____| |  \/  |          | |    |  __ \ / ____|  _ \__   __|
--  | |      | (___   | \  / |  ______  | |    | |__) | |  __| |_) | | |   
--  | |       \___ \  | |\/| | |______| | |    |  ___/| | |_ |  _ <  | |   
--  | |____   ____) | | |  | |          | |____| |    | |__| | |_) | | |   
--   \_____| |_____/  |_|  |_|          |______|_|     \_____|____/  |_|   
--                                                                         
----------------------------------------------------------------------------------
-- Company: U OF M
-- Engineer: Xueye Hu
-- 
-- Create Date: 02/23/2021 9:16:16 AM
-- Additional Comments: Revise to handle both master and slave LpGBT links
-- 
---------------------------------------------------------------------------------- 
                                                                         
-------------------------------------------------------
--! @file
--! @author Julian Mendez <julian.mendez@cern.ch> (CERN - EP-ESE-BE)
--! @version 2.0
--! @brief LpGBT-FPGA Top
-------------------------------------------------------

--! Include the IEEE VHDL standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--! Include the LpGBT-FPGA specific package
use work.lpgbtfpga_package.all;

--! Xilinx devices library:
library unisim;
use unisim.vcomponents.all;

entity lpgbtFpga_10g24_2uplink is 
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

        -- Up link master

        uplinkUserData_o_m                 : out std_logic_vector(229 downto 0);  --! Uplink data (user)
        uplinkEcData_o_m                   : out std_logic_vector(1 downto 0);    --! Uplink EC field
        uplinkIcData_o_m                   : out std_logic_vector(1 downto 0);    --! Uplink IC field
                
        uplinkBypassInterleaver_i_m        : in  std_logic;                       --! Bypass uplink interleaver (test purpose only)
        uplinkBypassFECEncoder_i_m         : in  std_logic;                       --! Bypass uplink FEC (test purpose only)
        uplinkBypassScrambler_i_m          : in  std_logic;                       --! Bypass uplink scrambler (test purpose only)
        
        

        uplinkFECCorrectedClear_i_m        : in  std_logic;                       --! Uplink FEC corrected error clear (debugging)
        uplinkFECCorrectedLatched_o_m      : out std_logic;                       --! Uplink FEC corrected error latched (debugging)

        uplinkReady_o_m                    : out std_logic;                       --! Uplink ready status
        
        -- Up link slave
        
        uplinkUserData_o_s                 : out std_logic_vector(229 downto 0);  --! Uplink data (user)
        uplinkEcData_o_s                   : out std_logic_vector(1 downto 0);    --! Uplink EC field
        uplinkIcData_o_s                   : out std_logic_vector(1 downto 0);    --! Uplink IC field
                
        uplinkBypassInterleaver_i_s        : in  std_logic;                       --! Bypass uplink interleaver (test purpose only)
        uplinkBypassFECEncoder_i_s         : in  std_logic;                       --! Bypass uplink FEC (test purpose only)
        uplinkBypassScrambler_i_s          : in  std_logic;                       --! Bypass uplink scrambler (test purpose only)
        
        

        uplinkFECCorrectedClear_i_s        : in  std_logic;                       --! Uplink FEC corrected error clear (debugging)
        uplinkFECCorrectedLatched_o_s      : out std_logic;                       --! Uplink FEC corrected error latched (debugging)

        uplinkReady_o_s                    : out std_logic;                       --! Uplink ready status

        
        -- Fixed-phase uplink CDC operation - master
        uplinkPhase_o_m                    : out  std_logic_vector(2 downto 0);   --! Phase to check fixed-phase
        uplinkPhaseCalib_i_m               : in   std_logic_vector(2 downto 0);   --! Phase measured in first reset
        uplinkPhaseForce_i_m               : in   std_logic;                      --! Force the phase to be the calibrated one

        -- Fixed-phase uplink CDC operation - slave
        uplinkPhase_o_s                    : out  std_logic_vector(2 downto 0);   --! Phase to check fixed-phase
        uplinkPhaseCalib_i_s               : in   std_logic_vector(2 downto 0);   --! Phase measured in first reset
        uplinkPhaseForce_i_s               : in   std_logic;                      --! Force the phase to be the calibrated one
        
        
        -- MGT
        clk_mgtrefclk_i                  : in  std_logic;                       --! Transceiver serial clock
        clk_mgtfreedrpclk_i              : in  std_logic;                       --! 125MHz Free-running clock
        
        clk_mgtTxClk_o                   : out std_logic;
        clk_mgtRxClk_o                   : out std_logic;
                    
        mgt_rxn_i_m                        : in  std_logic;
        mgt_rxp_i_m                        : in  std_logic;

        mgt_rxn_i_s                        : in  std_logic;
        mgt_rxp_i_s                        : in  std_logic;
                
        mgt_txn_o_m                        : out std_logic;
        mgt_txp_o_m                        : out std_logic;

        mgt_txn_o_s                        : out std_logic;
        mgt_txp_o_s                        : out std_logic;
                       
        mgt_txpolarity_i                   : in  std_logic;	   
        mgt_rxpolarity_i_m                 : in  std_logic;	   
        mgt_rxpolarity_i_s                 : in  std_logic;	
        	   
        mgt_txcaliben_i                  : in  std_logic;
        mgt_txcalib_i                    : in  std_logic_vector(6 downto 0);                      
        mgt_txaligned_o                  : out std_logic;
        mgt_txphase_o                    : out std_logic_vector(6 downto 0)        
   ); 
end lpgbtFpga_10g24_2uplink;



--=================================================================================================--
--####################################   Architecture   ###########################################-- 
--=================================================================================================--

architecture behavioral of lpgbtFpga_10g24_2uplink is

    -- Components declaration
    
    COMPONENT xlx_ku_mgt_10g24_2uplink is    
        generic(
            UPLINKCOUNT : integer := 2;
            CLKREFCOUNT : integer := 1);                                  
        port (
           --=============--
           -- Clocks      --
           --=============--
           MGT_REFCLK_i                 : in  std_logic;
           MGT_FREEDRPCLK_i             : in  std_logic;
                 
           MGT_RXUSRCLK_o               : out std_logic;
           MGT_TXUSRCLK_o               : out std_logic;
           
           --=============--
           -- Resets      --
           --=============--
           MGT_TXRESET_i                : in  std_logic;
           MGT_RXRESET_i                : in  std_logic;
           
           --=============--
           -- Control     --
           --=============--
           MGT_TXPolarity_i_m             : in  std_logic;	   
           MGT_RXPolarity_i_m             : in  std_logic;	   
    
           MGT_TXPolarity_i_s             : in  std_logic;	   
           MGT_RXPolarity_i_s             : in  std_logic;
           
           MGT_RXSlide_i_m                : in  std_logic;
           MGT_RXSlide_i_s                : in  std_logic;
                  
           MGT_ENTXCALIBIN_i_m            : in  std_logic;
           MGT_TXCALIB_i_m                : in  std_logic_vector(6 downto 0);
    
           MGT_ENTXCALIBIN_i_s            : in  std_logic; -- no Slave Tx in CSM application
           MGT_TXCALIB_i_s                : in  std_logic_vector(6 downto 0);       
           --=============--
           -- Status      --
           --=============--
           MGT_TXREADY_o_m                : out std_logic;
           MGT_RXREADY_o_m                : out std_logic;
           
           MGT_TX_ALIGNED_o_m             : out std_logic;
           MGT_TX_PIPHASE_o_m             : out std_logic_vector(6 downto 0); 
           
           MGT_TXREADY_o_s                : out std_logic;
           MGT_RXREADY_o_s                : out std_logic;
           
           MGT_TX_ALIGNED_o_s             : out std_logic;
           MGT_TX_PIPHASE_o_s             : out std_logic_vector(6 downto 0);         
           --==============--
           -- Data         --
           --==============--
           MGT_USRWORD_i_m                : in  std_logic_vector(31 downto 0); --tx 
           MGT_USRWORD_o_m                : out std_logic_vector(31 downto 0); --rx
           
           MGT_USRWORD_i_s                : in  std_logic_vector(31 downto 0);
           MGT_USRWORD_o_s                : out std_logic_vector(31 downto 0);       

           --===============--
           -- Serial intf.  --
           --===============--
           RXn_i_m                         : in  std_logic;
           RXp_i_m                         : in  std_logic;
           
           TXn_o_m                         : out std_logic;
           TXp_o_m                         : out std_logic;
           
           RXn_i_s                        : in  std_logic;
           RXp_i_s                        : in  std_logic;
           
           TXn_o_s                        : out std_logic;
           TXp_o_s                        : out std_logic   
       );
    end COMPONENT;
  
    COMPONENT cdc_tx is
      generic (
        g_CLOCK_A_RATIO : integer := 1    ; --! Ratio between strobe period and clock A period
        g_CLOCK_B_RATIO : integer := 8    ; --! Ratio between strobe period and clock B period (>=4)
        g_ACC_PHASE     : integer := 125*8; --! Phase accumulator number - only relevant for fixed phase operation
        g_PHASE_SIZE    : integer := 10     --! ceil(log2(g_ACC_PHASE))
      );
      port (
        -- Interface A (latch - from where data comes)
        reset_a_i        : in   std_logic;                                 --! reset (only de-assert when all clocks and strobe A are stable)	
        clk_a_i          : in   std_logic;                                 --! clock A
        data_a_i         : in   std_logic_vector;                          --! data A
        strobe_a_i       : in   std_logic;                                 --! strobe A
    	                                                                   
        -- Interface B (capture - to where data goes)                                                 
        clk_b_i          : in   std_logic;                                 --! clock B
        data_b_o         : out  std_logic_vector;                          --! data B (connected to vector of same size as data_a_i)
        strobe_b_o       : out  std_logic;                                 --! strobe B
        ready_b_o        : out  std_logic;                                 --! ready B (CDC is operating)
	
        -- Only relevant for fixed-phase operation
        clk_freerun_i    : in   std_logic;                                 --! Free-running clock (125MHz)	
        phase_o          : out  std_logic_vector(g_PHASE_SIZE-1 downto 0); --! Phase to check fixed-phase
        phase_calib_i    : in   std_logic_vector(g_PHASE_SIZE-1 downto 0); --! Phase measured in first reset
        phase_force_i    : in   std_logic                                  --! Force the phase to be the calibrated one
    
      );
    end component cdc_tx;
  
    COMPONENT lpgbtfpga_downlink
       GENERIC(
             -- Expert parameters
             c_multicyleDelay              : integer range 0 to 7 := 3;                          --! Multicycle delay
             c_clockRatio                  : integer := 8;                                       --! Clock ratio is clock_out / 40 (shall be an integer - E.g.: 320/40 = 8)
             c_outputWidth                 : integer                                             --! Transceiver's word size
        );
        port (
             -- Clocks
             clk_i                         : in  std_logic;                                      --! Downlink datapath clock (either 320 or 40MHz)
             clkEn_i                       : in  std_logic;                                      --! Clock enable (1 over 8 when encoding runs @ 320Mhz, '1' @ 40MHz)
             rst_n_i                       : in  std_logic;                                      --! Downlink reset signal (Tx ready from the transceiver)
     
             -- Down link
             userData_i                    : in  std_logic_vector(31 downto 0);                  --! Downlink data (user)
             ECData_i                      : in  std_logic_vector(1 downto 0);                   --! Downlink EC field
             ICData_i                      : in  std_logic_vector(1 downto 0);                   --! Downlink IC field
     
             -- Output
             mgt_word_o                    : out std_logic_vector((c_outputWidth-1) downto 0);   --! Downlink encoded frame (IC + EC + User Data + FEC)
     
             -- Configuration
             interleaverBypass_i           : in  std_logic;                                      --! Bypass downlink interleaver (test purpose only)
             encoderBypass_i               : in  std_logic;                                      --! Bypass downlink FEC (test purpose only)
             scramblerBypass_i             : in  std_logic;                                      --! Bypass downlink scrambler (test purpose only)
     
             -- Status
             rdy_o                         : out std_logic                                       --! Downlink ready status
        );
    END COMPONENT;
    
    COMPONENT lpgbtfpga_uplink
       GENERIC(
             -- General configuration
             DATARATE                        : integer range 0 to 2 := DATARATE_5G12;              --! Datarate selection can be: DATARATE_10G24 or DATARATE_5G12
             FEC                             : integer range 0 to 2 := FEC5;                       --! FEC selection can be: FEC5 or FEC12
     
             -- Expert parameters
             c_multicyleDelay                : integer range 0 to 7 := 3;                          --! Multicycle delay
             c_clockRatio                    : integer;                                            --! Clock ratio is mgt_userclk / 40 (shall be an integer)
             c_mgtWordWidth                  : integer;                                            --! Bus size of the input word
             c_allowedFalseHeader            : integer;                                            --! Number of false header allowed to avoid unlock on frame error
             c_allowedFalseHeaderOverN       : integer;                                            --! Number of header checked to know wether the lock is lost or not
             c_requiredTrueHeader            : integer;                                            --! Number of true header required to go in locked state
             c_bitslip_mindly                : integer := 1;                                       --! Number of clock cycle required when asserting the bitslip signal
             c_bitslip_waitdly               : integer := 40                                       --! Number of clock cycle required before being back in a stable state
        );
        PORT (
             -- Clock and reset
             uplinkClk_i                     : in  std_logic;                                      --! Input clock (Rx user clock from transceiver)
             uplinkClkOutEn_o                : out std_logic;                                      --! Clock enable to be used in the user's logic
             uplinkRst_n_i                   : in  std_logic;                                      --! Uplink reset signal (Rx ready from the transceiver)
     
             -- Input
             mgt_word_i                      : in  std_logic_vector((c_mgtWordWidth-1) downto 0);  --! Input frame coming from the MGT
     
             -- Data
             userData_o                      : out std_logic_vector(229 downto 0);                 --! User output (decoded data). The payload size varies depending on the
                                                                                                         --! datarate/FEC configuration:
                                                                                                         --!     * *FEC5 / 5.12 Gbps*: 112bit
                                                                                                         --!     * *FEC12 / 5.12 Gbps*: 98bit
                                                                                                         --!     * *FEC5 / 10.24 Gbps*: 230bit
                                                                                                         --!     * *FEC12 / 10.24 Gbps*: 202bit
             EcData_o                        : out std_logic_vector(1 downto 0);                   --! EC field value received from the LpGBT
             IcData_o                        : out std_logic_vector(1 downto 0);                   --! IC field value received from the LpGBT
     
             -- Control
             bypassInterleaver_i             : in  std_logic;                                      --! Bypass uplink interleaver (test purpose only)
             bypassFECEncoder_i              : in  std_logic;                                      --! Bypass uplink FEC (test purpose only)
             bypassScrambler_i               : in  std_logic;                                      --! Bypass uplink scrambler (test purpose only)
     
             -- Transceiver control
             mgt_bitslipCtrl_o               : out std_logic;                                      --! Control the Bitslib/RxSlide port of the Mgt
     
             -- Status
             dataCorrected_o                 : out std_logic_vector(229 downto 0);                 --! Flag allowing to know which bit(s) were toggled by the FEC
             IcCorrected_o                   : out std_logic_vector(1 downto 0);                   --! Flag allowing to know which bit(s) of the IC field were toggled by the FEC
             EcCorrected_o                   : out std_logic_vector(1 downto 0);                   --! Flag allowing to know which bit(s) of the EC field  were toggled by the FEC
             rdy_o                           : out std_logic;                                      --! Ready signal from the uplink decoder
             frameAlignerEven_o              : out std_logic                                       --! Number of bit slip is even (required only for advanced applications)

        );
    END COMPONENT;

    COMPONENT cdc_rx is
      generic (
        g_CLOCK_A_RATIO : integer := 8; --! Frequency ratio between slow and fast frequencies (>4)
        g_PHASE_SIZE    : integer := 3  --! log2(g_CLOCK_A_RATIO)
      );
      port (
        -- Interface A (latch - from where data comes)
        reset_a_i        : in   std_logic;                                 --! reset (only de-assert when all clocks and strobes are stable)		
        clk_a_i          : in   std_logic;                                 --! clock A
        data_a_i         : in   std_logic_vector;                          --! data A
        strobe_a_i       : in   std_logic;                                 --! strobe A
    
        -- Interface B (capture_a - to where data goes) 
        clk_b_i          : in   std_logic;                                 --! clock B
        data_b_o         : out  std_logic_vector;                          --! data B (connected to vector of same size as data_a_i)
        strobe_b_i       : in   std_logic;                                 --! strobe B
        ready_b_o        : out  std_logic;                                 --! Inteface is ready 
    
        -- Only relevant for fixed-phase operation
        phase_o          : out  std_logic_vector(g_PHASE_SIZE-1 downto 0); --! Phase to check fixed-phase
        phase_calib_i    : in   std_logic_vector(g_PHASE_SIZE-1 downto 0); --! Phase measured in first reset
        phase_force_i    : in   std_logic                                  --! Force the phase to be the calibrated one
    
      );
    end component cdc_rx;
	
    COMPONENT xlx_ku_mgt_ip_reset_synchronizer is                                            
       port (
          CLK_IN           : in  std_logic;
          RST_IN           : in  std_logic;
          RST_OUT          : out std_logic      
       );
    end component xlx_ku_mgt_ip_reset_synchronizer;

    -- User CDC Tx    
    signal downlinkData40     : std_logic_vector(35 downto 0)              ;
    signal downlinkData320    : std_logic_vector(35 downto 0)              ;
    signal downlinkStrobe320  : std_logic                                  ;
    signal mgt_txrdy_sync40   : std_logic                                  ;
    signal cdc_tx_reset       : std_logic                                  ;
    signal cdc_tx_ready       : std_logic                                  ;
	
    -- User CDC Rx    
    signal uplinkData320_m           : std_logic_vector(233 downto 0)        ;
    signal uplinkData320_s           : std_logic_vector(233 downto 0)        ;
        
    signal uplinkStrobe320_m         : std_logic                             ;
    signal uplinkStrobe320_s         : std_logic                             ;
        	
    signal uplinkData40_m            : std_logic_vector(233 downto 0)        ;
    signal uplinkData40_s            : std_logic_vector(233 downto 0)        ;
        
    signal cdc_rx_reset_m            : std_logic                             ;
    signal cdc_rx_reset_s            : std_logic                             ;  
      
    signal cdc_rx_ready_m            : std_logic                             ;
    signal cdc_rx_ready_s            : std_logic                             ;
    
    -- MGT
    signal uplinkReady_m           : std_logic                             ;
    signal uplinkReady_s           : std_logic                             ;    
    
    signal downlink_mgtword_m               : std_logic_vector(31 downto 0);
    
    signal uplink_mgtword_m                 : std_logic_vector(31 downto 0);
    signal uplink_mgtword_s                 : std_logic_vector(31 downto 0);
        
    signal mgt_rxslide_m                    : std_logic                  ;
    signal mgt_rxslide_s                    : std_logic                  ;    
    
    signal mgt_txrdy_s                      : std_logic                    ;
    signal mgt_rxrdy_m                      : std_logic                  ;
    signal mgt_rxrdy_s                      : std_logic                  ;
        
    signal clk_mgtTxClk_m                   : std_logic                    ;	
    signal clk_mgtRxClk_m                   : std_logic                    ;
       
    -- FEC latch flag
    signal uplinkdataCorrected_m              : std_logic_vector(229 downto 0);
    signal uplinkIcCorrected_m                : std_logic_vector(1 downto 0);  
    signal uplinkEcCorrected_m                : std_logic_vector(1 downto 0);  

    signal uplinkdataCorrected_s              : std_logic_vector(229 downto 0);
    signal uplinkIcCorrected_s                : std_logic_vector(1 downto 0);  
    signal uplinkEcCorrected_s                : std_logic_vector(1 downto 0); 


		 
begin                 --========####   Architecture Body   ####========--

    --========####   Downlink datapath   ####========--
	
    -- Reset scheme for downlink:
    -- downlinkRst_i     => Resets MGT Tx
    -- MGT Tx ready      => Resets User CDC Tx (responsible for generating the stable strobe aligned to 40MHz clock)
    -- User	CDC Tx ready => Resets downlink datapath
    txrdy_sync : xlx_ku_mgt_ip_reset_synchronizer                                     
       port map(
          CLK_IN  => downlinkClk_i,
          RST_IN  => mgt_txrdy_s,
          RST_OUT => mgt_txrdy_sync40 
       );
    cdc_tx_reset <= not mgt_txrdy_sync40;

    downlinkData40 <= downlinkIcData_i&downlinkEcData_i&downlinkUserData_i; 
    cdc_tx_inst : cdc_tx
      generic map(
        g_CLOCK_A_RATIO => 1    ,
        g_CLOCK_B_RATIO => 8    ,
        g_ACC_PHASE     => 125*8,
        g_PHASE_SIZE    => 10   
      )
      port map(
        -- Interface A (latch - from where data comes)
        reset_a_i        => cdc_tx_reset     ,
        clk_a_i          => downlinkClk_i    ,
        data_a_i         => downlinkData40   ,
        strobe_a_i       => '1'              ,

        -- Interface B (capture - to where data goes)                                                 
        clk_b_i          => clk_mgtTxClk_m   ,
        data_b_o         => downlinkData320  ,
        strobe_b_o       => downlinkStrobe320,
        ready_b_o        => cdc_tx_ready     ,

        -- Only relevant for fixed-phase operation
        clk_freerun_i    => clk_mgtfreedrpclk_i ,
        phase_o          => downlinkPhase_o     ,
        phase_calib_i    => downlinkPhaseCalib_i,
        phase_force_i    => downlinkPhaseForce_i

      );
	
    downlink_inst: lpgbtfpga_downlink    
       GENERIC MAP(    
            -- Expert parameters
            c_multicyleDelay      => 3,
            c_clockRatio          => 8,
            c_outputWidth         => 32
       )
       PORT MAP(
            -- Clocks
            clk_i                 => clk_mgtTxClk_m,
            clkEn_i               => downlinkStrobe320,
            rst_n_i               => cdc_tx_ready     ,
    
            -- Down link
            userData_i            => downlinkData320(31 downto 0) ,
            ECData_i              => downlinkData320(33 downto 32),
            ICData_i              => downlinkData320(35 downto 34),
    
            -- Output
            mgt_word_o            => downlink_mgtword_m           ,
    
            -- Configuration
            interleaverBypass_i   => downLinkBypassInterleaver_i  ,
            encoderBypass_i       => downLinkBypassFECEncoder_i   ,
            scramblerBypass_i     => downLinkBypassScrambler_i    ,
    
            -- Status
            rdy_o                 => downlinkReady_o
       );

    --========####   Uplink datapath   ####========--
    -- Reset scheme for uplink:
    -- uplinkRst_i           => Resets MGT Rx
    -- MGT Rx ready          => Resets uplink datapath
    -- Uplink datapath ready => Resets User CDC Rx
    cdc_rx_reset_m <= not uplinkReady_m;
    cdc_rx_reset_s <= not uplinkReady_s;
        
    uplink_inst_m: lpgbtfpga_uplink
       GENERIC MAP(
            -- General configuration
            DATARATE                        => DATARATE_10G24,
            FEC                             => FEC5,
    
            -- Expert parameters
            c_multicyleDelay                => 3,
            c_clockRatio                    => 8,
            c_mgtWordWidth                  => 32,
            c_allowedFalseHeader            => 5,
            c_allowedFalseHeaderOverN       => 64,
            c_requiredTrueHeader            => 30,
            c_bitslip_mindly                => 1,
            c_bitslip_waitdly               => 40
       )
       PORT MAP(
            -- Clock and reset
            uplinkClk_i                     => clk_mgtRxClk_m, -- common
            uplinkClkOutEn_o                => uplinkStrobe320_m,
            uplinkRst_n_i                   => mgt_rxrdy_m,
    
            -- Input
            mgt_word_i                      => uplink_mgtword_m,
    
            -- Data
            userData_o                      => uplinkData320_m(229 downto 0),
            EcData_o                        => uplinkData320_m(231 downto 230),
            IcData_o                        => uplinkData320_m(233 downto 232),
    
            -- Control
            bypassInterleaver_i             => uplinkBypassInterleaver_i_m,
            bypassFECEncoder_i              => uplinkBypassFECEncoder_i_m,
            bypassScrambler_i               => uplinkBypassScrambler_i_m,
    
            -- Transceiver control
            mgt_bitslipCtrl_o               => mgt_rxslide_m,
    
            -- Status
            dataCorrected_o                 => uplinkdataCorrected_m,
            IcCorrected_o                   => uplinkIcCorrected_m  ,
            EcCorrected_o                   => uplinkEcCorrected_m  ,
            rdy_o                           => uplinkReady_m,
			frameAlignerEven_o              => open
       );

    uplink_inst_s: lpgbtfpga_uplink
       GENERIC MAP(
            -- General configuration
            DATARATE                        => DATARATE_10G24,
            FEC                             => FEC5,
    
            -- Expert parameters
            c_multicyleDelay                => 3,
            c_clockRatio                    => 8,
            c_mgtWordWidth                  => 32,
            c_allowedFalseHeader            => 5,
            c_allowedFalseHeaderOverN       => 64,
            c_requiredTrueHeader            => 30,
            c_bitslip_mindly                => 1,
            c_bitslip_waitdly               => 40
       )
       PORT MAP(
            -- Clock and reset
            uplinkClk_i                     => clk_mgtRxClk_m, -- common
            uplinkClkOutEn_o                => uplinkStrobe320_s,
            uplinkRst_n_i                   => mgt_rxrdy_s,
    
            -- Input
            mgt_word_i                      => uplink_mgtword_s,
    
            -- Data
            userData_o                      => uplinkData320_s(229 downto 0),
            EcData_o                        => uplinkData320_s(231 downto 230),
            IcData_o                        => uplinkData320_s(233 downto 232),
    
            -- Control
            bypassInterleaver_i             => uplinkBypassInterleaver_i_s,
            bypassFECEncoder_i              => uplinkBypassFECEncoder_i_s,
            bypassScrambler_i               => uplinkBypassScrambler_i_s,
    
            -- Transceiver control
            mgt_bitslipCtrl_o               => mgt_rxslide_s,
    
            -- Status
            dataCorrected_o                 => uplinkdataCorrected_s,
            IcCorrected_o                   => uplinkIcCorrected_s  ,
            EcCorrected_o                   => uplinkEcCorrected_s  ,
            rdy_o                           => uplinkReady_s,
			frameAlignerEven_o              => open
       );
       	
   --! FEC Corrected Flag for debugging
   upLinkFECCorrected_m: process(clk_mgtRxClk_m)
     begin
       if rising_edge(clk_mgtRxClk_m) then
         if uplinkFECCorrectedClear_i_m = '1' then
             uplinkFECCorrectedLatched_o_m <= '0';
         else
             if( (unsigned(uplinkDataCorrected_m) /= 0) or 
                 (unsigned(uplinkIcCorrected_m)   /= 0) or 
                 (unsigned(uplinkEcCorrected_m)   /= 0) ) then
                 uplinkFECCorrectedLatched_o_m <= '1';
             end if;
         end if;
       end if;
   end process;


   upLinkFECCorrected_s: process(clk_mgtRxClk_m)
     begin
       if rising_edge(clk_mgtRxClk_m) then
         if uplinkFECCorrectedClear_i_s = '1' then
             uplinkFECCorrectedLatched_o_s <= '0';
         else
             if( (unsigned(uplinkDataCorrected_s) /= 0) or 
                 (unsigned(uplinkIcCorrected_s)   /= 0) or 
                 (unsigned(uplinkEcCorrected_s)   /= 0) ) then
                 uplinkFECCorrectedLatched_o_s <= '1';
             end if;
         end if;
       end if;
   end process;
   
   

    cdc_rx_inst_m : cdc_rx
      generic map(
        g_CLOCK_A_RATIO => 8, --! Frequency ratio between slow and fast frequencies (>4)
        g_PHASE_SIZE    => 3  --! log2(g_CLOCK_A_RATIO)
      )
      port map(
        -- Interface A (latch - from where data comes)
        reset_a_i        =>	cdc_rx_reset_m   ,
        clk_a_i          => clk_mgtRxClk_m , -- common
        data_a_i         => uplinkData320_m  ,
        strobe_a_i       => uplinkStrobe320_m,
    
        -- Interface B (capture_a - to where data goes) 
        clk_b_i          => uplinkClk_i    ,
        data_b_o         => uplinkData40_m   ,
        strobe_b_i       => '1'            ,
        ready_b_o        => cdc_rx_ready_m   ,
    
        -- Only relevant for fixed-phase operation
        phase_o          => uplinkPhase_o_m     ,
        phase_calib_i    => uplinkPhaseCalib_i_m,
        phase_force_i    => uplinkPhaseForce_i_m
      );
      
    cdc_rx_inst_s : cdc_rx
      generic map(
        g_CLOCK_A_RATIO => 8, --! Frequency ratio between slow and fast frequencies (>4)
        g_PHASE_SIZE    => 3  --! log2(g_CLOCK_A_RATIO)
      )
      port map(
        -- Interface A (latch - from where data comes)
        reset_a_i        =>	cdc_rx_reset_s   ,
        clk_a_i          => clk_mgtRxClk_m , -- common
        data_a_i         => uplinkData320_s  ,
        strobe_a_i       => uplinkStrobe320_s,
    
        -- Interface B (capture_a - to where data goes) 
        clk_b_i          => uplinkClk_i    ,
        data_b_o         => uplinkData40_s   ,
        strobe_b_i       => '1'            ,
        ready_b_o        => cdc_rx_ready_s   ,
    
        -- Only relevant for fixed-phase operation
        phase_o          => uplinkPhase_o_s     ,
        phase_calib_i    => uplinkPhaseCalib_i_s,
        phase_force_i    => uplinkPhaseForce_i_s
      );
            
	
    uplinkUserData_o_m <= uplinkData40_m(229 downto 0);   --! Uplink data (user)
    uplinkEcData_o_m   <= uplinkData40_m(231 downto 230); --! Uplink EC field
    uplinkIcData_o_m   <= uplinkData40_m(233 downto 232); --! Uplink IC field
    uplinkReady_o_m    <= cdc_rx_ready_m;


    uplinkUserData_o_s <= uplinkData40_s(229 downto 0);   --! Uplink data (user)
    uplinkEcData_o_s   <= uplinkData40_s(231 downto 230); --! Uplink EC field
    uplinkIcData_o_s   <= uplinkData40_s(233 downto 232); --! Uplink IC field
    uplinkReady_o_s    <= cdc_rx_ready_s;
    
    --========####   FPGA Transceiver   ####========--
    
 mgt_inst:   xlx_ku_mgt_10g24_2uplink                                     
       port map (
           --=============--
           -- Clocks      --
           --=============--
           MGT_REFCLK_i                 => clk_mgtrefclk_i,
           MGT_FREEDRPCLK_i             => clk_mgtfreedrpclk_i,
                 
           MGT_RXUSRCLK_o               => clk_mgtRxClk_m,
           MGT_TXUSRCLK_o               => clk_mgtTxClk_m,
           
           --=============--
           -- Resets      --
           --=============--
           MGT_TXRESET_i                => downlinkRst_i,
           MGT_RXRESET_i                => uplinkRst_i,
           
           --=============--
           -- Control     --
           --=============--
           MGT_TXPolarity_i_m             => mgt_txpolarity_i,	   
           MGT_RXPolarity_i_m             => mgt_rxpolarity_i_m,		   
    
           MGT_TXPolarity_i_s             => '0',	   
           MGT_RXPolarity_i_s             => mgt_rxpolarity_i_s,	
           
           MGT_RXSlide_i_m                => mgt_rxslide_m,
           MGT_RXSlide_i_s                => mgt_rxslide_s,
                  
           MGT_ENTXCALIBIN_i_m            => mgt_txcaliben_i,
           MGT_TXCALIB_i_m                => mgt_txcalib_i  ,
    
           MGT_ENTXCALIBIN_i_s            => '0', -- no Slave Tx in CSM application
           MGT_TXCALIB_i_s                => (others =>'0'),       
           --=============--
           -- Status      --
           --=============--
           MGT_TXREADY_o_m                => mgt_txrdy_s,
           MGT_RXREADY_o_m                => mgt_rxrdy_m,
           
           MGT_TX_ALIGNED_o_m             => mgt_txaligned_o,
           MGT_TX_PIPHASE_o_m             => mgt_txphase_o, 
           
           MGT_TXREADY_o_s                => open,
           MGT_RXREADY_o_s                => mgt_rxrdy_s,
           
           MGT_TX_ALIGNED_o_s             => open,
           MGT_TX_PIPHASE_o_s             => open,        
           --==============--
           -- Data         --
           --==============--
           MGT_USRWORD_i_m                => downlink_mgtword_m, --tx 
           MGT_USRWORD_o_m                => uplink_mgtword_m, --rx
           
           MGT_USRWORD_i_s                => (others => '0'),
           MGT_USRWORD_o_s                => uplink_mgtword_s, --rx       
           --===============--
           -- Serial intf.  --
           --===============--
           RXn_i_m                         => mgt_rxn_i_m,
           RXp_i_m                         => mgt_rxp_i_m,
           
           TXn_o_m                         => mgt_txn_o_m,
           TXp_o_m                         => mgt_txp_o_m,
           
           RXn_i_s                        => mgt_rxn_i_s,
           RXp_i_s                        => mgt_rxp_i_s,
           
           TXn_o_s                        => mgt_txn_o_s, -- -- no Slave Tx in CSM application
           TXp_o_s                        => mgt_txp_o_s   
       );


    clk_mgtTxClk_o <= clk_mgtTxClk_m;
    clk_mgtRxClk_o <= clk_mgtRxClk_m;
   
end behavioral;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--