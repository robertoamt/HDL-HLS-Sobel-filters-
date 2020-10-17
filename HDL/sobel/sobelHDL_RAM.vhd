----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 24.06.2020 09:12:23
-- Design Name: 
-- Module Name: sobel_VHDL_v1 - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.sobel_pkg.all;


entity sobelHDL_ABS is
	
port (
       -----------------------General CLK-nRST -----------------------
		AXIS_ACLK	: in std_logic;
		AXIS_ARESETN	: in std_logic;
		
		----------------------------------SLAVE----------------------------------
		S_AXIS_TDATA	: in std_logic_vector(7 downto 0);
		S_AXIS_TKEEP	: in std_logic_vector(1 downto 0);
		S_AXIS_TLAST	: in std_logic;
		S_AXIS_TREADY	: out std_logic;	
		S_AXIS_TVALID	: in std_logic;
		
		----------------------------------MASTER----------------------------------
		M_AXIS_TDATA	: out std_logic_vector(7 downto 0);
		M_AXIS_TKEEP	: out std_logic_vector(1 downto 0);
		M_AXIS_TLAST	: out std_logic;
		M_AXIS_TREADY	: in std_logic;	
		M_AXIS_TVALID	: out std_logic;
		
		-----------------------------------AXI LITE SLAVE-------------------------
		S_AXI_ACLK	: in std_logic;
		S_AXI_AWADDR	: in std_logic_vector(3 downto 0);
		S_AXI_AWVALID	: in std_logic;
		S_AXI_AWREADY	: out std_logic;
		S_AXI_WDATA	: in std_logic_vector(31 downto 0);  
		S_AXI_WSTRB	: in std_logic_vector(3 downto 0);
		S_AXI_WVALID	: in std_logic;
		S_AXI_WREADY	: out std_logic;
		S_AXI_BRESP	: out std_logic_vector(1 downto 0);
		S_AXI_BVALID	: out std_logic;
		S_AXI_BREADY	: in std_logic
	   );
end sobelHDL_ABS;

architecture Behavioral of sobelHDL_ABS is



------------------------------ State variables-------------------------------
signal sobel_fsm : sobel_fsm_type;  

signal data_missed : std_logic;
signal s_ready : std_logic;
signal s_valid : std_logic;
signal m_ready : std_logic;
signal m_valid : std_logic;
signal data_out : std_logic_vector(7 downto 0);
signal data_calc : std_logic_vector(7 downto 0);
signal data_inter : std_logic_vector(7 downto 0);
------------------------------ State variables-------------------------------

-----------------------------------Counters----------------------------------
signal rcv_col_count_sobel : unsigned (10 downto 0);
signal rcv_col_count_sobel_st0 : unsigned (10 downto 0);
signal rcv_row_count_sobel : unsigned (10 downto 0);
signal send_col_count_sobel : unsigned (10 downto 0);
signal send_row_count_sobel : unsigned (10 downto 0);
-----------------------------------Counters----------------------------------


------------------------------------Stage signals----------------------------
signal en_st0 : std_logic;
signal en_st1 : std_logic;
signal en_st2 : std_logic;
signal en_st3 : std_logic;
------------------------------------Stage signals----------------------------

------------------------------- AXI LITE---------------------------------------
signal row_sobel : unsigned (10 downto 0); -- 512 = 200
signal col_sobel : unsigned (10 downto 0); -- 768 = 300
signal awready_axi_lite : std_logic;
signal wready_axi_lite : std_logic;
signal awvalid_axi_lite : std_logic;
signal wvalid_axi_lite : std_logic;
signal new_data_axi_lite : std_logic;
signal sel_parameters_axi_lite : std_logic;
------------------------------- AXI LITE---------------------------------------

--------------------------------Line Buffer------------------------------------
    signal data_in : STD_LOGIC_VECTOR (7 downto 0);
    signal line_buffer_1_addr_rd : STD_LOGIC_VECTOR (10 downto 0);
    signal line_buffer_1_ren : STD_LOGIC;
    signal line_buffer_1_q0 : STD_LOGIC_VECTOR (7 downto 0);
    signal line_buffer_1_wen : STD_LOGIC;
    signal line_buffer_2_addr_rd : STD_LOGIC_VECTOR (10 downto 0);
    signal line_buffer_2_ren : STD_LOGIC;
    signal line_buffer_2_q0 : STD_LOGIC_VECTOR (7 downto 0);
    signal line_buffer_2_wen : STD_LOGIC;
    signal line_buffer_1_addr_wr : STD_LOGIC_VECTOR (10 downto 0);
    signal line_buffer_2_addr_wr : STD_LOGIC_VECTOR (10 downto 0);
    signal global_process_enable : std_logic;
    
    signal w_2, w_1, w_0 : window_TYPE(0 to 2);
    
---------------------------------Line Buffer-----------------------------------

begin

------------------------------- AXI LITE---------------------------------------
     axi_lite_slave: component axi_lite_core
generic map (
    C_S_AXI_ADDR_WIDTH  => 4,
    C_S_AXI_DATA_WIDTH  =>32)
port map(
    ACLK  => S_AXI_ACLK,               
    ARESET => AXIS_ARESETN,              
    AWADDR  => S_AXI_AWADDR,             
    AWVALID  => awvalid_axi_lite,             
    AWREADY  => awready_axi_lite,             
    WDATA   => S_AXI_WDATA,              
    WSTRB => S_AXI_WSTRB,               
    WVALID    => wvalid_axi_lite,            
    WREADY    => wready_axi_lite,           
    BRESP    => S_AXI_BRESP,            
    BVALID   => S_AXI_BVALID,           
    BREADY  => S_AXI_BREADY             
);   


 awvalid_axi_lite <= S_AXI_AWVALID ;
 wvalid_axi_lite <= S_AXI_WVALID ;
 S_AXI_AWREADY <= awready_axi_lite;
 S_AXI_WREADY <= wready_axi_lite;  
 new_data_axi_lite <= (awvalid_axi_lite and wvalid_axi_lite and awready_axi_lite and wready_axi_lite);
 
 axi_stream_process:process(AXIS_ACLK)
 begin
    if rising_edge(AXIS_ACLK) then
        if AXIS_ARESETN = '0' then
            sel_parameters_axi_lite <= '0';
        else
            if new_data_axi_lite = '1' then
               if  sel_parameters_axi_lite = '0' then
	               row_sobel <= resize(unsigned(S_AXI_WDATA),row_sobel'length);
	               sel_parameters_axi_lite <= '1';
	           else
	               col_sobel <= resize(unsigned(S_AXI_WDATA),col_sobel'length);
	           end if;
	        end if;
	    end if;
	end if;
 end process;
 
--row_sobel <= to_unsigned(5,row_sobel'length);
--col_sobel <= to_unsigned(7,col_sobel'length);
 
------------------------------- AXI LITE---------------------------------------

--------------------------------Line Buffers-----------------------------------

    line_buffer_1 : component line_buffer
    generic map (
        DataWidth => 8,
        AddressRange => 10,
        AddressWidth => 11)
    port map (
        clk => AXIS_ACLK,
        reset => AXIS_ARESETN,
        address0 => line_buffer_1_addr_rd,
        ce0 => line_buffer_1_ren,
        q0 => line_buffer_1_q0,
        address1 => line_buffer_1_addr_wr,
        ce1 => line_buffer_1_wen,
        we1 => '1',
        d1 => line_buffer_2_q0);

    line_buffer_2 : component line_buffer
    generic map (
        DataWidth => 8,
        AddressRange => 10,
        AddressWidth => 11)
    port map (
        clk => AXIS_ACLK,
        reset => AXIS_ARESETN,
        address0 => line_buffer_2_addr_rd,
        ce0 => line_buffer_2_ren,
        q0 => line_buffer_2_q0,
        address1 => line_buffer_2_addr_wr,
        ce1 => line_buffer_2_wen,
        we1 => '1',
        d1 => data_in);
       
--------------------------------Line Buffers-----------------------------------

rcv_counter_process:process(AXIS_ACLK)
    begin
        if rising_edge(AXIS_ACLK) then
            if AXIS_ARESETN = '0' then
                rcv_col_count_sobel <= (others=>'0');
                rcv_col_count_sobel_st0 <= (others=>'0');
                rcv_row_count_sobel <= (others=>'0');
            else
                if s_valid = '1' then
                
                    rcv_col_count_sobel_st0 <= rcv_col_count_sobel;--Addr delayed 1 clk cicle
                    
                    if rcv_col_count_sobel = col_sobel-1 then
                        rcv_col_count_sobel <= (others=>'0');
                        rcv_row_count_sobel <= rcv_row_count_sobel + 1;
                    else
                        rcv_col_count_sobel <= rcv_col_count_sobel + 1;
                    end if;
                end if;
            end if;
        end if;
    end process;
 
 send_counter_process:process(AXIS_ACLK)
    begin
        if rising_edge(AXIS_ACLK) then
            if AXIS_ARESETN = '0' or S_AXIS_TLAST='1' then
                send_col_count_sobel <= (others=>'0');
                send_row_count_sobel <= (others=>'0');
            else
                if (m_valid = '1' and m_ready='1' and data_missed='0' ) then
                    if send_col_count_sobel = col_sobel-1 then
                        send_col_count_sobel <= (others=>'0');
                        send_row_count_sobel <= send_row_count_sobel + 1;
                    else
                        send_col_count_sobel <= send_col_count_sobel + 1;
                    end if;
                end if;
            end if;
        end if;
    end process;
    
line_buffer_1_addr_rd <= std_logic_vector(rcv_col_count_sobel);
line_buffer_2_addr_rd <= std_logic_vector(rcv_col_count_sobel); 
line_buffer_1_addr_wr <= std_logic_vector(rcv_col_count_sobel_st0);
line_buffer_2_addr_wr <= std_logic_vector(rcv_col_count_sobel_st0); 

M_AXIS_TKEEP <= "11";
M_AXIS_TLAST <= '1' when (send_row_count_sobel = row_sobel-1 and send_col_count_sobel = col_sobel-1) else '0';
S_AXIS_TREADY <= s_ready;
s_valid <= S_AXIS_TVALID; 
m_ready <= M_AXIS_TREADY;
M_AXIS_TVALID <= m_valid;
M_AXIS_TDATA <= (others=>'0') when (send_col_count_sobel= 0 or send_col_count_sobel= 1 or send_row_count_sobel = 0 or send_row_count_sobel = 1) else  data_out;

data_missed <= '1' when (m_valid = '1' and m_ready = '0') else '0';
s_ready <= '1' when (sobel_fsm = NORMAL and data_missed = '0') else '0';

line_buffer_1_wen <= en_st0 when s_ready='1' else '0';
line_buffer_2_wen <= en_st0 when s_ready='1' else '0';
line_buffer_1_ren <= s_valid when s_ready='1' else '0';
line_buffer_2_ren <= s_valid when s_ready='1' else '0';
--------------------------------FSM--------------------------------------------
    rcv_module_process:process(AXIS_ACLK)
    begin
        if rising_edge(AXIS_ACLK) then
            if AXIS_ARESETN = '0' then
                m_valid <= '0';
                en_st0 <= '0';
                en_st1 <= '0';
                en_st2 <= '0';
                en_st3 <= '0';
            else
                case sobel_fsm is
                    when START =>
                        m_valid <= '0'; --dato no valido a enviar
                        sobel_fsm <= NORMAL;
                    when NORMAL =>
                        if data_missed = '1' then --El dato se perdió
                            sobel_fsm <= MISSED;
                        else
                        m_valid <= '0'; --dato no valido a enviar
                        data_in <= S_AXIS_TDATA; --backup del dato
                        en_st0 <= s_valid;
                        en_st1 <= en_st0;
                        en_st2 <= en_st1;
                        en_st3 <= en_st2 ;
                        --data_out <= data_calc;
                        
                        
                            --stage 0
                            if en_st0 = '1' then
	                           w_0 <= data_in & w_0(0 to w_0'high-1);
	                           w_1 <= line_buffer_2_q0 & w_1(0 to w_1'high-1);
	                           w_2 <= line_buffer_1_q0 & w_2(0 to w_2'high-1); 
                            end if;
                            
                            --stage 1         
                            if en_st1 = '1' then
                            data_calc <= sobel_kernel(w_2,w_1,w_0);
                            end if;
                            
                            if en_st2 = '1' then
                            data_inter <= data_calc;
                            end if;
                            
                            if en_st3='1' then
                            data_out <= data_inter;
                                if m_ready = '1' then
                                    m_valid <= '1';
                                else
                                    sobel_fsm <= SEND;   
                                end if; 
                             end if;  
                        end if;
                    when SEND =>
                        if m_ready = '1' then
                            m_valid <= '1';
                            sobel_fsm <= NORMAL;
                        end if;
                    when MISSED => 
                        if m_ready = '1' then -- El modulo de la derecha puede aceptar dato
                            sobel_fsm <= NORMAL;
                            m_valid <= '0'; --limpio valid
                        end if;
                    when others => null;
                end case;
            end if;
        end if;
    end process;

--------------------------------FSM--------------------------------------------
end Behavioral;
