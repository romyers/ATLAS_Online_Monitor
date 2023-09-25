-------------------------------------------------------
--! @file
--! @author Julian Mendez <julian.mendez@cern.ch> (CERN - EP-ESE-BE)
--! @version 6.0
--! @brief GBT-FPGA IP - Device specific transceiver
-------------------------------------------------------

--! IEEE VHDL standard library:
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--! Xilinx devices library:
library unisim;
use unisim.vcomponents.all;

--! @brief MGT - Transceiver
--! @details 
--! The MGT module provides the interface to the transceivers to send the GBT-links via
--! high speed links (@4.8Gbps)
entity xlx_ku_mgt_10g24_2uplink is                                     
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
end xlx_ku_mgt_10g24_2uplink;

--! @brief MGT - Transceiver
--! @details The MGT module implements all the logic required to send the GBT frame on high speed
--! links: resets modules for the transceiver, Tx PLL and alignement logic to align the received word with the 
--! GBT frame header.
architecture structural of xlx_ku_mgt_10g24_2uplink is
    --================================ Signal Declarations ================================--

    COMPONENT xlx_ku_mgt_ip_10g24
      PORT (
        gthrxn_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        gthrxp_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        gthtxn_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        gthtxp_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
                        
        gtwiz_userclk_tx_active_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_userclk_rx_active_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_buffbypass_rx_reset_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_buffbypass_rx_start_user_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_buffbypass_rx_done_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_buffbypass_rx_error_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);       
        gtwiz_reset_clk_freerun_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_all_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_tx_pll_and_datapath_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_tx_datapath_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_rx_pll_and_datapath_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_rx_datapath_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);       
        gtwiz_reset_rx_cdr_stable_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_tx_done_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        gtwiz_reset_rx_done_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        -- double width
        gtwiz_userdata_tx_in : IN STD_LOGIC_VECTOR(63 DOWNTO 0);
        gtwiz_userdata_rx_out : OUT STD_LOGIC_VECTOR(63 DOWNTO 0);
        
        gtrefclk00_in : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
        --qpll0outclk_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        --qpll0outrefclk_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        drpaddr_in : IN STD_LOGIC_VECTOR(17 DOWNTO 0);    
        drpclk_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);  
        drpdi_in : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
        drpen_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        drpwe_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);

        
        
        
        rxcommadeten_in: IN STD_LOGIC_VECTOR(1 downto 0); 
        rxmcommaalignen_in: IN STD_LOGIC_VECTOR(1 downto 0);
        rxpcommaalignen_in: IN STD_LOGIC_VECTOR(1 downto 0);        
        rxpolarity_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxprbscntreset_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxprbssel_in : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
        rxslide_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxusrclk_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxusrclk2_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);

        txpippmen_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txpippmovrden_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txpippmpd_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txpippmsel_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txpippmstepsize_in : IN STD_LOGIC_VECTOR(9 DOWNTO 0);                
        txpolarity_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txusrclk_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        txusrclk2_in : IN STD_LOGIC_VECTOR(1 DOWNTO 0);
        
        drpdo_out : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
        drprdy_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);

--        gtpowergood_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
--        rxbyteisaligned_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
--        rxbyterealign_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
--        rxcommadet_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0);
        
        rxoutclk_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxpmaresetdone_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxprbserr_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        rxprbslocked_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        txbufstatus_out : OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
        txoutclk_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
        txpmaresetdone_out : OUT STD_LOGIC_VECTOR(1 DOWNTO 0)
        
        --xprgdivresetdone_out : OUT STD_LOGIC_VECTOR(0 DOWNTO 0)
      );
    END COMPONENT;    

    
    COMPONENT drp_interface
        PORT(
            drp_clk : IN STD_LOGIC;
            drprdy_i : IN STD_LOGIC;
            drpdo_i : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
            drpen_o : OUT STD_LOGIC;
            drpwe_o: OUT STD_LOGIC;
            drpaddr_o: OUT STD_LOGIC_VECTOR(8 DOWNTO 0);
            drpdi_o: OUT STD_LOGIC_VECTOR(15 DOWNTO 0);

            rxprbscntreset_o: OUT STD_LOGIC;
            rxprbssel_o: OUT STD_LOGIC_VECTOR(3 DOWNTO 0);
            rxprbserr_i: IN STD_LOGIC;
            rxprbslocked_i: IN STD_LOGIC

        );
    END COMPONENT;

    
    COMPONENT tx_phase_aligner is
      generic(
        -- User choice of DRP control or port control
        -- Recommended nowadays to use in DRP control as a strange behaviour was observed using the port in PI code stepping mode
        g_DRP_NPORT_CTRL        : boolean                      := true;  --! Uses DRP control of port control for the transmitter PI
        g_DRP_ADDR_TXPI_PPM_CFG : std_logic_vector(8 downto 0) := ("010011010")  --! Check the transceiver user guide of your device for this address               
        );                                                                      
      port (
        --==============================================================================
        --! User control/monitor ports
        --==============================================================================    
        -- Clock / reset                                                       
        clk_sys_i : in std_logic;           --! system clock input
        reset_i   : in std_logic;  --! active high sync. reset (recommended to keep reset_i=1 while transceiver reset initialization is being performed)
    
        -- Top level interface                                                 
        tx_aligned_o : out std_logic;  --! Use it as a reset for the user transmitter logic
    
        -- Config (for different flavours)
        tx_pi_phase_calib_i   : in std_logic_vector(6 downto 0);  --! previous calibrated tx pi phase (tx_pi_phase_o after first reset calibration)
        tx_ui_align_calib_i   : in std_logic;  --! align with previous calibrated tx pi phase
        tx_fifo_fill_pd_max_i : in std_logic_vector(31 downto 0);  --! phase detector accumulated max output, sets precision of phase detector
                                               --! this is supposedly a static signal, this block shall be reset whenever this signal changes
                                               --! the time for each phase detection after a clear is given by tx_fifo_fill_pd_max_i * PERIOD_clk_txusr_i
        tx_fine_realign_i     : in std_logic;  --! A rising edge will cause the Tx to perform a fine realignment to the half-response
    
        -- It is only valid to re-shift clock once aligned (tx_aligned_o = '1') 
        ps_strobe_i     : in  std_logic;  --! pulse synchronous to clk_sys_i to activate a shift in the phase (only captured rising edge, so a signal larger than a pulse is also fine)
        ps_inc_ndec_i   : in  std_logic;  --! 1 increments phase by phase_step_i units, 0 decrements phase by phase_step_i units
        ps_phase_step_i : in  std_logic_vector(3 downto 0);  --! number of units to shift the phase of the receiver clock (see Xilinx transceiver User Guide to convert units in time)       
        ps_done_o       : out std_logic;  --! pulse synchronous to clk_sys_i to indicate a phase shift was performed
    
        -- Tx PI phase value
        tx_pi_phase_o : out std_logic_vector(6 downto 0);  --! phase shift accumulated
    
        -- Tx fifo fill level phase detector                                   
        tx_fifo_fill_pd_o : out std_logic_vector(31 downto 0);  --! phase detector output, when aligned this value should be close to (0x2_0000)
    
        --==============================================================================
        --! MGT ports
        --==============================================================================
        clk_txusr_i          : in std_logic;  --! txusr2clk                
        -- Tx fifo fill level - see Xilinx transceiver User Guide for more information      
        tx_fifo_fill_level_i : in std_logic;  --! connect to txbufstatus[0]
    
        -- Transmitter PI ports - see Xilinx transceiver User Guide for more information
        -- obs1: all txpi ports shall be connected to the transceiver even when using this block in DRP-mode                
        txpippmen_o       : out std_logic;  --! enable tx phase interpolator controller
        txpippmovrden_o   : out std_logic;  --! enable DRP control of tx phase interpolator
        txpippmsel_o      : out std_logic;  --! set to 1 when using tx pi ppm controler
        txpippmpd_o       : out std_logic;  --! power down transmitter phase interpolator 
        txpippmstepsize_o : out std_logic_vector(4 downto 0);  --! sets step size and direction of phase shift with port control PI code stepping mode
    
        -- DRP interface - see Xilinx transceiver User Guide for more information
        -- obs2: connect clk_sys_i to drpclk
        -- obs3: if using this block in port-mode, DRP output can be left floating and input connected to '0'               
        drpaddr_o : out std_logic_vector(8 downto 0);  --! For devices with a 10-bit DRP address interface, connect MSB to '0'
        drpen_o   : out std_logic;          --! DRP enable transaction
        drpdi_o   : out std_logic_vector(15 downto 0);  --! DRP data write
        drprdy_i  : in  std_logic;          --! DRP finished transaction
        drpdo_i   : in  std_logic_vector(15 downto 0);  --! DRP data read; not used nowadays, write only interface
        drpwe_o   : out std_logic           --! DRP write enable
    
        );
    end COMPONENT;

    -- Reset signals (master)
    signal tx_reset_done                    : std_logic;
    signal txfsm_reset_done_m                 : std_logic;
    signal rx_reset_done                    : std_logic;
    signal rxfsm_reset_done                 : std_logic;
    
    signal rxBuffBypassRst                  : std_logic;
    signal gtwiz_userclk_rx_active_int      : std_logic;
    signal gtwiz_buffbypass_rx_reset_in_s   : std_logic;
    signal gtwiz_userclk_tx_active_int      : std_logic;
    signal gtwiz_buffbypass_tx_reset_in_m   : std_logic;
    
    signal gtwiz_userclk_tx_reset_int_m       : std_logic;
    signal inv_gtwiz_userclk_tx_reset_int_m   : std_logic;
    
    signal gtwiz_userclk_rx_reset_int_m       : std_logic;
    signal inv_gtwiz_userclk_rx_reset_int_m   : std_logic;
        
    signal txpmaresetdone_m                   : std_logic;
    signal rxpmaresetdone_m                   : std_logic;
    
    signal rx_reset_sig                     : std_logic;
    signal tx_reset_sig                     : std_logic;
    
    signal MGT_USRWORD_m                    : std_logic_vector(31 downto 0);
        
    -- Clock signals
    signal rx_wordclk_sig_m                   : std_logic;
    signal tx_wordclk_sig_m                   : std_logic;
    signal rxoutclk_sig_m                     : std_logic;
    signal txoutclk_sig_m                     : std_logic;
    
    signal rxoutclk_out_tmp                   : std_logic;
    signal txoutclk_out_tmp                   : std_logic;
    
    -- Tx phase aligner signals
    signal txbufstatus_m                    : std_logic_vector(1 downto 0); 
    
    signal txpippmen_m                      : std_logic;
    signal txpippmovrden_m                  : std_logic;
    signal txpippmsel_m                     : std_logic;
    signal txpippmpd_m                      : std_logic;
    signal txpippmstepsize_in_m               : std_logic_vector(4 downto 0);
    
    signal drpaddr_m                        : std_logic_vector(8 downto 0);
    signal drpen_m                          : std_logic;
    signal drpdi_m                          : std_logic_vector(15 downto 0);
    signal drprdy_m                         : std_logic;
    signal drpdo_m                          : std_logic_vector(15 downto 0);
    signal drpwe_m                          : std_logic;
    signal tx_reset_hptd_ip_m                 : std_logic;
    signal tx_reset_hptd_ip_sync_m            : std_logic;
    signal MGT_TX_ALIGNED_m                 : std_logic;
	signal MGT_TX_ALIGNED_sync_m              : std_logic;
	
	

    -- signals (slave)

    signal txfsm_reset_done_s                 : std_logic;

    signal gtwiz_buffbypass_tx_reset_in_s   : std_logic;

        
    signal txpmaresetdone_s                   : std_logic;
    signal rxpmaresetdone_s                   : std_logic;

    signal MGT_USRWORD_s                    : std_logic_vector(31 downto 0);
        
    -- Clock signals

    
    -- Tx phase aligner signals (in CSM application, no slave TX!)
    signal txbufstatus_s                    : std_logic_vector(1 downto 0); 
    signal txpippmen_s                      : std_logic;
    signal txpippmovrden_s                  : std_logic;
    signal txpippmsel_s                     : std_logic;
    signal txpippmpd_s                      : std_logic;
    signal txpippmstepsize_in_s               : std_logic_vector(4 downto 0);

    signal drpaddr_s                        : std_logic_vector(8 downto 0);
    signal drpen_s                          : std_logic;
    signal drpdi_s                          : std_logic_vector(15 downto 0);
    signal drprdy_s                         : std_logic;
    signal drpdo_s                          : std_logic_vector(15 downto 0);
    signal drpwe_s                          : std_logic;
    signal tx_reset_hptd_ip_s                 : std_logic;
    signal tx_reset_hptd_ip_sync_s            : std_logic;
    signal MGT_TX_ALIGNED_s                 : std_logic;
	signal MGT_TX_ALIGNED_sync_s              : std_logic;


    ----if enable pattern_checker
    signal rxprbscntreset_m                : std_logic;
    signal rxprbssel_m                     : STD_LOGIC_VECTOR(3 DOWNTO 0);
    signal rxprbserr_m                    : std_logic;
    signal rxprbslocked_m                 : std_logic;
    signal drpaddr_ibert_m                    : std_logic_vector(8 downto 0);
    signal drpen_ibert_m                      : std_logic;
    signal drpdi_ibert_m                      : std_logic_vector(15 downto 0);
    signal drpwe_ibert_m                      : std_logic;

    signal drpaddr_toip_m                    : std_logic_vector(8 downto 0);
    signal drpen_toip_m                      : std_logic;
    signal drpdi_toip_m                      : std_logic_vector(15 downto 0);
    signal drpwe_toip_m                      : std_logic;


    signal rxprbscntreset_s                : std_logic;
    signal rxprbssel_s                     : STD_LOGIC_VECTOR(3 DOWNTO 0);
    signal rxprbserr_s                    : std_logic;
    signal rxprbslocked_s                 : std_logic;
    signal drpaddr_ibert_s                    : std_logic_vector(8 downto 0);
    signal drpen_ibert_s                      : std_logic;
    signal drpdi_ibert_s                      : std_logic_vector(15 downto 0);
    signal drpwe_ibert_s                      : std_logic;

    signal drpaddr_toip_s                    : std_logic_vector(8 downto 0);
    signal drpen_toip_s                      : std_logic;
    signal drpdi_toip_s                      : std_logic_vector(15 downto 0);
    signal drpwe_toip_s                      : std_logic;

    ----if enable pattern_checker end
		
--=================================================================================================--
begin                 --========####   Architecture Body   ####========-- 
--=================================================================================================--
   
    --==================================== User Logic =====================================--
   
    --=============--
    -- Assignments --
    --=============--        

    drpaddr_toip_m <= drpaddr_m WHEN drpen_m = '1' ELSE drpaddr_ibert_m;
    drpen_toip_m   <= drpen_m   WHEN drpen_m = '1' ELSE drpen_ibert_m;
    drpdi_toip_m   <= drpdi_m   WHEN drpen_m = '1' ELSE drpdi_ibert_m;
    drpwe_toip_m   <= drpwe_m   WHEN drpen_m = '1' ELSE drpwe_ibert_m;

    drpaddr_toip_s <= drpaddr_s WHEN drpen_s = '1' ELSE drpaddr_ibert_s;
    drpen_toip_s   <= drpen_s   WHEN drpen_s = '1' ELSE drpen_ibert_s;
    drpdi_toip_s   <= drpdi_s   WHEN drpen_s = '1' ELSE drpdi_ibert_s;
    drpwe_toip_s   <= drpwe_s   WHEN drpen_s = '1' ELSE drpwe_ibert_s;


    MGT_TXREADY_o_m          <= tx_reset_done and MGT_TX_ALIGNED_sync_m ; -- and txfsm_reset_done;
	
    txAlignedSynch_tx: entity work.xlx_ku_mgt_ip_reset_synchronizer
        PORT MAP(
           clk_in                                   => tx_wordclk_sig_m,
           rst_in                                   => MGT_TX_ALIGNED_m,
           rst_out                                  => MGT_TX_ALIGNED_sync_m
        );
    MGT_TX_ALIGNED_o_m <= MGT_TX_ALIGNED_sync_m;

    tx_reset_hptd_ip_m       <= not(tx_reset_done);    
    
    txPhaseAlignerResetSynch_sys: entity work.xlx_ku_mgt_ip_reset_synchronizer
        PORT MAP(
           clk_in                                   => MGT_FREEDRPCLK_i,
           rst_in                                   => tx_reset_hptd_ip_m,
           rst_out                                  => tx_reset_hptd_ip_sync_m
        );

    MGT_RXREADY_o_m          <= rx_reset_done and rxfsm_reset_done; 
    MGT_RXREADY_o_s          <= rx_reset_done and rxfsm_reset_done;
          
    MGT_RXUSRCLK_o         <= rx_wordclk_sig_m;   
    MGT_TXUSRCLK_o         <= tx_wordclk_sig_m;
        
    rx_reset_sig           <= MGT_RXRESET_i or not(tx_reset_done and MGT_TX_ALIGNED_m);-- MGT_RXRESET_i; -- or not(tx_reset_done and MGT_TX_ALIGNED_m); -- and txfsm_reset_done);
    tx_reset_sig           <= MGT_TXRESET_i;    

    rxBuffBypassRst        <= not(gtwiz_userclk_rx_active_int) or (not(tx_reset_done) and not(MGT_TX_ALIGNED_m));
      
    resetDoneSynch_rx: entity work.xlx_ku_mgt_ip_reset_synchronizer
        PORT MAP(
           clk_in                                   => rx_wordClk_sig_m,
           rst_in                                   => rxBuffBypassRst,
           rst_out                                  => gtwiz_buffbypass_rx_reset_in_s
        );
         
      
--    resetSynch_tx: entity work.xlx_ku_mgt_ip_reset_synchronizer
--        PORT MAP(
--           clk_in                                   => tx_wordclk_sig,
--           rst_in                                   => not(gtwiz_userclk_tx_active_int),
--           rst_out                                  => gtwiz_buffbypass_tx_reset_in_s
--        );
      
    gtwiz_userclk_tx_reset_int_m <= not(txpmaresetdone_m);
    gtwiz_userclk_rx_reset_int_m <= not(rxpmaresetdone_m and rxpmaresetdone_s); 
    
    inv_gtwiz_userclk_tx_reset_int_m    <= not gtwiz_userclk_tx_reset_int_m;
    inv_gtwiz_userclk_rx_reset_int_m    <= not gtwiz_userclk_rx_reset_int_m;
        

      
      rxWordClkBuf_inst_m: bufg_gt
          port map (
             O                                        => rx_wordclk_sig_m, 
             I                                        => rxoutclk_sig_m,
             CE                                       => inv_gtwiz_userclk_rx_reset_int_m, 
             DIV                                      => "000",
             CLR                                      => '0',
             CLRMASK                                  => '0',
             CEMASK                                   => '0'
          ); 


                                  
        txWordClkBuf_inst: bufg_gt
          port map (
             O                                        => tx_wordclk_sig_m, 
             I                                        => txoutclk_sig_m,
             CE                                       => inv_gtwiz_userclk_tx_reset_int_m, 
             DIV                                      => "000",
             CLR                                      => '0',
             CLRMASK                                  => '0',
             CEMASK                                   => '0'
          ); 
      
      activetxUsrClk_proc: process(gtwiz_userclk_tx_reset_int_m, tx_wordclk_sig_m)
      begin
        if gtwiz_userclk_tx_reset_int_m = '1' then
            gtwiz_userclk_tx_active_int <= '0';
        elsif rising_edge(tx_wordclk_sig_m) then
            gtwiz_userclk_tx_active_int <= '1';
        end if;
        
      end process;
      
                
      activerxUsrClk_proc: process(gtwiz_userclk_rx_reset_int_m, rx_wordclk_sig_m)
      begin
        if gtwiz_userclk_rx_reset_int_m = '1' then
            gtwiz_userclk_rx_active_int <= '0';
        elsif rising_edge(rx_wordclk_sig_m) then
            gtwiz_userclk_rx_active_int <= '1';
        end if;
      
      end process;
      
      rxWordPipeline_proc: process(rx_reset_done, rx_wordclk_sig_m)
      begin
          if rx_reset_done = '0' then
              MGT_USRWORD_o_m <= (others => '0');
              MGT_USRWORD_o_s <= (others => '0');
              
          elsif rising_edge(rx_wordclk_sig_m) then
              MGT_USRWORD_o_m <= MGT_USRWORD_m;
              MGT_USRWORD_o_s <= MGT_USRWORD_s;
          end if;      
      end process;          

      xlx_ku_mgt_std_i: xlx_ku_mgt_ip_10g24
         PORT MAP (  
         
             gthrxn_in(0)                          => RXn_i_m,
             gthrxn_in(1)                          => RXn_i_s,
             
             gthrxp_in(0)                          => RXp_i_m,
             gthrxp_in(1)                          => RXp_i_s,
             
             gthtxn_out(0)                         => TXn_o_m,
             gthtxn_out(1)                         => TXn_o_s,
             
             gthtxp_out(0)                         => TXp_o_m,
             gthtxp_out(1)                         => TXp_o_s,
             
             --common sig
             
             gtwiz_userclk_tx_active_in(0)         => gtwiz_userclk_tx_active_int,
             gtwiz_userclk_rx_active_in(0)         => gtwiz_userclk_rx_active_int,
             gtwiz_buffbypass_rx_reset_in(0)       => gtwiz_buffbypass_rx_reset_in_s,
             gtwiz_buffbypass_rx_start_user_in(0)  => '0',
             gtwiz_buffbypass_rx_done_out(0)       => rxfsm_reset_done,
             gtwiz_buffbypass_rx_error_out         => open,
             gtwiz_reset_clk_freerun_in(0)         => MGT_FREEDRPCLK_i,
             gtwiz_reset_all_in(0)                 => '0',
             gtwiz_reset_tx_pll_and_datapath_in(0) => tx_reset_sig,
             gtwiz_reset_tx_datapath_in(0)         => '0',
             gtwiz_reset_rx_pll_and_datapath_in(0) => '0',
             gtwiz_reset_rx_datapath_in(0)         => rx_reset_sig,
             gtwiz_reset_rx_cdr_stable_out         => open,   
             gtwiz_reset_tx_done_out(0)            => tx_reset_done,
             gtwiz_reset_rx_done_out(0)            => rx_reset_done,

----------------------------------------------------------------
             -- double data width
             gtwiz_userdata_tx_in(31 downto 0)          => MGT_USRWORD_i_m,
             gtwiz_userdata_tx_in(63 downto 32)         => MGT_USRWORD_i_s,
                          
             gtwiz_userdata_rx_out(31 downto 0)         => MGT_USRWORD_m,
             gtwiz_userdata_rx_out(63 downto 32)        => MGT_USRWORD_s,

             -- DRP clk 
             drpclk_in(0)                               => MGT_FREEDRPCLK_i,
             drpclk_in(1)                               => MGT_FREEDRPCLK_i,
                          
             -- DRP bus (used by the tx phase aligner)
             drpaddr_in(8 downto 0)                     => drpaddr_toip_m,
             drpaddr_in(17 downto 9)                    => drpaddr_toip_s,             
             
             drpdi_in(15 downto 0)                      => drpdi_toip_m,
             drpdi_in(31 downto 16)                     => drpdi_toip_s,             
             
             drpen_in(0)                                => drpen_toip_m,
             drpen_in(1)                                => drpen_toip_s,             
             
             drpwe_in(0)                                => drpwe_toip_m,
             drpwe_in(1)                                => drpwe_toip_s,             
             
             drpdo_out(15 downto 0)                     => drpdo_m,
             drpdo_out(31 downto 16)                    => drpdo_s,             
             
             drprdy_out(0)                              => drprdy_m,
             drprdy_out(1)                              => drprdy_s,


             ----if enable pattern_checker
            rxprbscntreset_in(0)                        => rxprbscntreset_m,
            rxprbscntreset_in(1)                        => rxprbscntreset_s,
            rxprbssel_in(3 downto 0)                    => rxprbssel_m,
            rxprbssel_in(7 downto 4)                    => rxprbssel_s,
            rxprbserr_out(0)                            => rxprbserr_m,
            rxprbserr_out(1)                            => rxprbserr_s,
            rxprbslocked_out(0)                         => rxprbslocked_m,
            rxprbslocked_out(1)                         => rxprbslocked_s,

            
            
            
            
            ----if enable pattern_checker end




             gtrefclk00_in(0)                           => MGT_REFCLK_i,  --Top sma MGT REFCLK
             --gtrefclk00_in(1)                           => MGT_REFCLK_i,  --Top sma MGT REFCLK

             rxcommadeten_in(0)                         => '1',
             rxcommadeten_in(1)                         => '1',            
             
             rxmcommaalignen_in(0)                      => '0',
             rxmcommaalignen_in(1)                      => '0',             
             
             rxpcommaalignen_in(0)                      => '0',
             rxpcommaalignen_in(1)                      => '0',             


             rxpolarity_in(0)                      => MGT_RXPolarity_i_m,
             rxpolarity_in(1)                      => MGT_RXPolarity_i_s,

             txpolarity_in(0)                      => MGT_TXPolarity_i_m,
             txpolarity_in(1)                      => MGT_TXPolarity_i_s, -- no Slave Tx in CSM application
             
             
             rxslide_in(0)                         => MGT_RXSlide_i_m,
             rxslide_in(1)                         => MGT_RXSlide_i_s,

             rxusrclk_in(0)                        => rx_wordclk_sig_m,
             rxusrclk_in(1)                        => rx_wordclk_sig_m,             
             
             rxusrclk2_in(0)                       => rx_wordclk_sig_m,
             rxusrclk2_in(1)                       => rx_wordclk_sig_m,             
             
             rxoutclk_out(0)                       => rxoutclk_sig_m,
             rxoutclk_out(1)                       => rxoutclk_out_tmp, -- open,             
             
             txusrclk_in(0)                        => tx_wordclk_sig_m,
             txusrclk_in(1)                        => tx_wordclk_sig_m,             
             
             txusrclk2_in(0)                       => tx_wordclk_sig_m,
             txusrclk2_in(1)                       => tx_wordclk_sig_m,             
             
             txoutclk_out(0)                       => txoutclk_sig_m,
             txoutclk_out(1)                       => txoutclk_out_tmp, -- open,
             
 
             -- Phase Interpolator control / monitoring signals
             txpippmen_in(0)                        => txpippmen_m,
             txpippmen_in(1)                        => txpippmen_s,             
             
             txpippmovrden_in(0)                    => txpippmovrden_m,
             txpippmovrden_in(1)                    => txpippmovrden_s,             
             
             txpippmpd_in(0)                        => txpippmpd_m,
             txpippmpd_in(1)                        => txpippmpd_s,             
             
             txpippmsel_in(0)                       => txpippmsel_m,
             txpippmsel_in(1)                       => txpippmsel_s,             
             
             txpippmstepsize_in(4 downto 0)         => txpippmstepsize_in_m,             
             txpippmstepsize_in(9 downto 5)         => txpippmstepsize_in_s,   
             
             rxpmaresetdone_out(0)                  => rxpmaresetdone_m,
             rxpmaresetdone_out(1)                  => rxpmaresetdone_s,             
             
             txpmaresetdone_out(0)                  => txpmaresetdone_m,
             txpmaresetdone_out(1)                  => txpmaresetdone_s, -- no Slave Tx in CSM application
                            
             -- Tx buffer status
             txbufstatus_out(1 downto 0)                => txbufstatus_m,
             txbufstatus_out(3 downto 2)                => txbufstatus_s  
    
             --gtwiz_buffbypass_tx_reset_in(0)       => gtwiz_buffbypass_tx_reset_in_s,
             --gtwiz_buffbypass_tx_start_user_in(0)  => '0',
             --gtwiz_buffbypass_tx_done_out(0)       => txfsm_reset_done,
             --gtwiz_buffbypass_tx_error_out         => open,

                 
             --txprgdivresetdone_out                  => open

             
         );
               

    cmp_tx_phase_aligner_m : tx_phase_aligner
      generic map(
        g_DRP_NPORT_CTRL        => true,
        g_DRP_ADDR_TXPI_PPM_CFG => "010011010"    
        )                                                                      
      port map(
        --==============================================================================
        --! User control/monitor ports
        --==============================================================================    
        -- Clock / reset                                                       
        clk_sys_i             => MGT_FREEDRPCLK_i     , 
        reset_i               => tx_reset_hptd_ip_sync_m,

        tx_aligned_o          => MGT_TX_ALIGNED_m ,

        tx_pi_phase_calib_i   => MGT_TXCALIB_i_m    ,
        tx_ui_align_calib_i   => MGT_ENTXCALIBIN_i_m,
        tx_fifo_fill_pd_max_i => x"00400000"      ,

        tx_fine_realign_i     => '0',

        ps_strobe_i           => '0',
        ps_inc_ndec_i         => '0',
        ps_phase_step_i       => (others => '0'),      
        ps_done_o             => open,

        tx_pi_phase_o         => MGT_TX_PIPHASE_o_m,

        tx_fifo_fill_pd_o     => open,
    
        --==============================================================================
        --! MGT ports
        --==============================================================================
        clk_txusr_i           => tx_wordclk_sig_m  , 

        tx_fifo_fill_level_i  => txbufstatus_m(0)  ,
           
        txpippmen_o           => txpippmen_m       ,
        txpippmovrden_o       => txpippmovrden_m   ,
        txpippmsel_o          => txpippmsel_m      ,
        txpippmpd_o           => txpippmpd_m       ,
        txpippmstepsize_o     => txpippmstepsize_in_m,
            
        drpaddr_o             => drpaddr_m,
        drpen_o               => drpen_m,
        drpdi_o               => drpdi_m,
        drprdy_i              => drprdy_m,
        drpdo_i               => drpdo_m,
        drpwe_o               => drpwe_m
        );


    cmp_tx_phase_aligner_s : tx_phase_aligner
      generic map(
        g_DRP_NPORT_CTRL        => true,
        g_DRP_ADDR_TXPI_PPM_CFG => "010011010"    
        )                                                                      
      port map(
        --==============================================================================
        --! User control/monitor ports
        --==============================================================================    
        -- Clock / reset                                                       
        clk_sys_i             => MGT_FREEDRPCLK_i     , 
        reset_i               => tx_reset_hptd_ip_sync_s,

        tx_aligned_o          => MGT_TX_ALIGNED_s ,

        tx_pi_phase_calib_i   => MGT_TXCALIB_i_s    ,
        tx_ui_align_calib_i   => MGT_ENTXCALIBIN_i_s,
        tx_fifo_fill_pd_max_i => x"00400000"      ,

        tx_fine_realign_i     => '0',

        ps_strobe_i           => '0',
        ps_inc_ndec_i         => '0',
        ps_phase_step_i       => (others => '0'),      
        ps_done_o             => open,

        tx_pi_phase_o         => MGT_TX_PIPHASE_o_s,

        tx_fifo_fill_pd_o     => open,
    
        --==============================================================================
        --! MGT ports
        --==============================================================================
        clk_txusr_i           => tx_wordclk_sig_m  , 

        tx_fifo_fill_level_i  => txbufstatus_s(0)  ,
           
        txpippmen_o           => txpippmen_s       ,
        txpippmovrden_o       => txpippmovrden_s   ,
        txpippmsel_o          => txpippmsel_s      ,
        txpippmpd_o           => txpippmpd_s       ,
        txpippmstepsize_o     => txpippmstepsize_in_s,
            
        drpaddr_o             => drpaddr_s,
        drpen_o               => drpen_s,
        drpdi_o               => drpdi_s,
        drprdy_i              => drprdy_s,
        drpdo_i               => drpdo_s,
        drpwe_o               => drpwe_s
        );

    drp_interface_inst_m: drp_interface
    PORT MAP(
        drp_clk          => MGT_FREEDRPCLK_i,
        drprdy_i         => drprdy_m,
        drpdo_i          => drpdo_m,
        drpen_o          => drpen_ibert_m,
        drpwe_o          => drpwe_ibert_m,
        drpaddr_o        => drpaddr_ibert_m,
        drpdi_o          => drpdi_ibert_m,
        rxprbscntreset_o => rxprbscntreset_m,
        rxprbssel_o      => rxprbssel_m,
        rxprbserr_i      => rxprbserr_m,
        rxprbslocked_i   => rxprbslocked_m
    );

    drp_interface_inst_s: drp_interface
    PORT MAP(
        drp_clk          => MGT_FREEDRPCLK_i,
        drprdy_i         => drprdy_s,
        drpdo_i          => drpdo_s,
        drpen_o          => drpen_ibert_s,
        drpwe_o          => drpwe_ibert_s,
        drpaddr_o        => drpaddr_ibert_s,
        drpdi_o          => drpdi_ibert_s,
        rxprbscntreset_o => rxprbscntreset_s,
        rxprbssel_o      => rxprbssel_s,
        rxprbserr_i      => rxprbserr_s,
        rxprbslocked_i   => rxprbslocked_s
    );
        
        
        
end structural;
--=================================================================================================--
--#################################################################################################--
--=================================================================================================--