-- Author: Bobby McGovern (rmcgov@umich.edu)
-- Description: This is a rudimentary controller for the JTAG master. When `en` 
--              is brought high, it configures the ASDs to accept calibration
--              pulses and the AMTs to forward them serially.
--              These instructions can be modified below, as the constants
--              `ASD_control_word` and `AMT_control_word`.

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity jtag_control is
    generic(
        inst_len : positive := 5; -- length of instruction register
        num_data_len_bits  : positive := 8; -- number of bits needed to express data register length
                                            -- 8 bits: longest data word is 180 bits
        num_chain_len_bits : positive := 5  -- number of bits needed to express JTAG chain length
                                            -- 5 bits: longest JTAG chain is 18 AMT
    );
    port(
        clk : in std_logic; -- 2x speed of tck
        -- TODO: async reset
        en  : in std_logic;
        busy : in std_logic;
        
        en_master : out std_logic;
        inst_reg  : out std_logic_vector(inst_len - 1 downto 0);
        data_len  : out unsigned(num_data_len_bits - 1 downto 0);
        data_reg  : out std_logic_vector(2**num_data_len_bits - 1 downto 0)
--        ;
--        chain_len : out unsigned(5 downto 0)  -- 6 bits: longest JTAG chain is (18 AMT) * (3 ASD) = 54 -- FIXME
    );
end jtag_control;

-- Description: this architecture configures the AMT and ASDs to use calibration pulses
architecture calib_config of jtag_control is
    type state_type is (
        IDLE,
        PRE_CONFIG_ASD,
        CONFIG_ASD,
        PRE_CONFIG_AMT,
        CONFIG_AMT,
        POST_CONFIG_AMT
    );
    signal state_curr, state_next : state_type := IDLE;
    
    -- TODO: make these generic?
    constant AMT_ASDCONTROL : std_logic_vector(inst_len - 1 downto 0) := "0" & "1001";
    constant ASD_word1 : std_logic_vector(0 to 52) := -- cf. 2.7, Table 6, 2.8.3, 6.3 ASD UG
        "11111111" & -- all channels receive test pulses
        "111" &      --    8 calibration injection capacitors per channel
        "11011000" & --  -39 mV DISC1 thresh.
        "110" &      --   32 mV DISC2 thresh.
        "0010" &     --  2.5 mV DISC1 hyst.
        "0110" &     --   20 ns integration gate
        "010" &      --  5.9 uA rundown current
        "000" &      --   17 ns deadtime
        ("00" & "00" & "00" & "00" & "00" & "00" & "00" & "00") & -- all channels ON
        "0";         -- ASD mode
    constant ASD_word2 : std_logic_vector(0 to 52) :=
        "11111111" & -- all channels receive test pulses
        "101" &      --    6 calibration injection capacitors per channel
        "11011000" & --  -39 mV DISC1 thresh.
        "110" &      --   32 mV DISC2 thresh.
        "0010" &     --  2.5 mV DISC1 hyst.
        "0110" &     --   20 ns integration gate
        "010" &      --  5.9 uA rundown current
        "000" &      --   17 ns deadtime
        ("00" & "00" & "00" & "00" & "00" & "00" & "00" & "00") & -- all channels ON
        "0";         -- ASD mode
    constant ASD_word3 : std_logic_vector(0 to 52) :=
        "11111111" & -- all channels receive test pulses
        "011" &      --    4 calibration injection capacitors per channel
        "11011000" & --  -39 mV DISC1 thresh.
        "110" &      --   32 mV DISC2 thresh.
        "0010" &     --  2.5 mV DISC1 hyst.
        "0110" &     --   20 ns integration gate
        "010" &      --  5.9 uA rundown current
        "000" &      --   17 ns deadtime
        ("00" & "00" & "00" & "00" & "00" & "00" & "00" & "00") & -- all channels ON
        "0";         -- ASD mode
    constant ASD_control_word : std_logic_vector(160 - 1 downto 0) 
        := ASD_word3 & ASD_word2 & ASD_word1 & "0";
    
    constant AMT_CONTROL : std_logic_vector(inst_len - 1 downto 0) := "1" & "1000";
    constant AMT_control_word : std_logic_vector(179 downto 0) := -- cf. Table 8, 3.1 AMT UG
        "111111111111" & -- CSR14: enable channels
        "111111111111" & -- CSR13: enable channels
        "000"&"111111111" & -- CSR12: enable errors
        "000000000000" & -- CSR11: 
--        "000001110100" & -- CSR10: enable pair, trailer, header, serial  
        "0000"&"0100"&"0001" & -- CSR10: disable trigger matching, enable relative '0', enable serial, disable header & trailer, use leading.
        "10"&"11"&"000"&"0"&"1111" & -- CSR09: TODO strobe select? --"10" no strobe, "11" 80Mbps,"1111" prog TDC ID
        "110111101011" & -- CSR08: 3563 count_roll_over
        "000000000000" & -- CSR07:    0 coarse_time_offset
        "110110001000" & -- CSR06: 3464 bunch_count_offset 
        "000000000000" & -- CSR05:    0 event_count_offset
        "110101100000" & -- CSR04: 3424 reject_count_offset
        "000000011111" & -- CSR03:   31 match_window
        "000000100111" & -- CSR02:   39 search_window
        "000000100000" & -- CSR01:   32 mask_window
        "000000000000" ; -- CSR00: 

----Bobby orginal configuration
--    constant AMT_CONTROL : std_logic_vector(inst_len - 1 downto 0) := "1" & "1000";
--    constant AMT_control_word : std_logic_vector(179 downto 0) := -- cf. Table 8, 3.1 AMT UG
--        "111111111111" & -- CSR14: enable channels
--        "111111111111" & -- CSR13: enable channels
--        "000"&"111111111" & -- CSR12: enable errors
--        "000000000000" & -- CSR11: 
--        "000001110100" & -- CSR10: enable pair, trailer, header, serial
--        "00"&"11"&"000"&"0"&"0000" & -- CSR09: TODO strobe select?
--        "110111101011" & -- CSR08: 3563 count_roll_over
--        "000000000000" & -- CSR07:    0 coarse_time_offset
--        "110110001000" & -- CSR06: 3464 bunch_count_offset 
--        "000000000000" & -- CSR05:    0 event_count_offset
--        "110101100000" & -- CSR04: 3424 reject_count_offset
--        "000000011111" & -- CSR03:   31 match_window
--        "000000100111" & -- CSR02:   39 search_window
--        "000000100000" & -- CSR01:   32 mask_window
--        "000000000000" ; -- CSR00:         
        
begin
    sync : process(clk) is
    begin
        if rising_edge(clk) then
            state_curr <= state_next;
        end if;
    end process sync;
    
    comb : process(state_curr, en, busy) is
    begin
        state_logic : case state_curr is
            when IDLE =>
                en_master <= '0';
                if en = '1' then
                    state_next <= PRE_CONFIG_ASD;
                else
                    state_next <= IDLE;
                end if;
            when PRE_CONFIG_ASD =>
                en_master <= '1';
                if busy = '1' then
                    state_next <= CONFIG_ASD;
                else
                    state_next <= PRE_CONFIG_ASD;
                end if;
            when CONFIG_ASD =>
                en_master <= '0';
                if busy = '0' then
                    state_next <= PRE_CONFIG_AMT;
                else
                    state_next <= CONFIG_ASD;
                end if;
            when PRE_CONFIG_AMT =>
                en_master <= '1';
                if busy = '1' then
                    state_next <= CONFIG_AMT;
                else
                    state_next <= PRE_CONFIG_AMT;
                end if;
            when CONFIG_AMT =>
                en_master <= '0';
                if busy = '0' then
                    state_next <= POST_CONFIG_AMT;
                else
                    state_next <= CONFIG_AMT;
                end if;
            when POST_CONFIG_AMT =>
                en_master <= '0';
                if en = '0' then 
                    state_next <= IDLE;
                else
                    state_next <= POST_CONFIG_AMT;
                end if;
        end case;
        
        tx_logic : case state_curr is
            when IDLE | PRE_CONFIG_ASD | CONFIG_ASD | POST_CONFIG_AMT =>
                inst_reg <= AMT_ASDCONTROL;
                data_len <= to_unsigned(ASD_control_word'length, 8);
                data_reg(2**num_data_len_bits - 1 downto ASD_control_word'length) <= (others => '0');
                data_reg(ASD_control_word'range) <= ASD_control_word;
            when PRE_CONFIG_AMT | CONFIG_AMT =>
                inst_reg <= AMT_CONTROL;
                data_len <= to_unsigned(AMT_control_word'length, 8);
                data_reg(2**num_data_len_bits - 1 downto AMT_control_word'length) <= (others => '0');
                data_reg(AMT_control_word'length - 1 downto 0) <= AMT_control_word;
        end case;
    end process comb;

end calib_config;

---- Description: this architecture executes only the IDCODE instruction on the AMT
--architecture simple_IDCODE of jtag_control is
--    type state_type is (
--        IDLE,
--        PRE_IDCODE,
--        IDCODE,
--        POST_IDCODE
--    );
--    signal state_curr, state_next : state_type := IDLE;
    
--    -- TODO: make these generic?
--    constant IDCODE_inst : std_logic_vector(inst_len - 1 downto 0) := "1"&"0001";
--    constant IDCODE_data : std_logic_vector(31 downto 0) := "0011"&"1000101110000101"&"00000011000"&"1";
--    --0011 & 1000101110000101 & 00000011000 & 1 -- AMT-3
--    --0001 & 1000101110000011 & 00010011000 & 1 -- AMT-1
--    --0001 & 1000101110000101 & 00000011000 & 1 -- AMT-2
--begin
--    sync : process(clk) is
--    begin
--        if rising_edge(clk) then
--            state_curr <= state_next;
--        end if;
--    end process sync;
    
--    comb : process(state_curr, en, busy) is
--    begin
--        state_logic : case state_curr is
--            when IDLE =>
--                en_master <= '0';
--                if en = '1' then
--                    state_next <= PRE_IDCODE;
--                else
--                    state_next <= IDLE;
--                end if;
--            when PRE_IDCODE =>
--                en_master <= '1';
--                if busy = '1' then
--                    state_next <= IDCODE;
--                else
--                    state_next <= PRE_IDCODE;
--                end if;
--            when IDCODE =>
--                en_master <= '0';
--                if busy = '0' then
--                    state_next <= POST_IDCODE;
--                else
--                    state_next <= IDCODE;
--                end if;
--            when POST_IDCODE =>
--                en_master <= '0';
--                if en = '0' then 
--                    state_next <= IDLE;
--                else
--                    state_next <= POST_IDCODE;
--                end if;
--        end case;
        
--        tx_logic : case state_curr is
--            when others =>
--                inst_reg <= IDCODE_inst;
--                data_len <= to_unsigned(IDCODE_data'length, 8);
--                data_reg(2**num_data_len_bits - 1 downto IDCODE_data'length) <= (others => '0');
--                data_reg(IDCODE_data'range) <= IDCODE_data;
--        end case;
--    end process comb;

--end simple_IDCODE;
