----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 24.06.2020 09:12:23
-- Design Name: 
-- Module Name: buffer_VHDL - Behavioral
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
use work.rgb2gray_pkg.all;


entity rgb2grayHDL is

port (
       -----------------------General CLK-nRST -----------------------
		AXIS_ACLK	: in std_logic;
		AXIS_ARESETN	: in std_logic;
		
		----------------------------------SLAVE----------------------------------
		S_AXIS_TDATA	: in std_logic_vector(31 downto 0);
		S_AXIS_TKEEP	: in std_logic_vector(3 downto 0);
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
end rgb2grayHDL;

architecture Behavioral of rgb2grayHDL is

-- Constant declarations
constant NUMBER_OF_INPUT_WORDS : integer := 3;
constant NUMBER_OF_OUTPUT_BYTES : integer := 4;

-- State variables
signal rgb2gray_fsm : rgb2gray_fsm_type;

------------------------------- AXI LITE---------------------------------------
signal awready_axi_lite : std_logic;
signal wready_axi_lite : std_logic;
signal awvalid_axi_lite : std_logic;
signal wvalid_axi_lite : std_logic;
signal new_data_axi_lite : std_logic;
signal sel_parameters_axi_lite : std_logic;
------------------------------- AXI LITE---------------------------------------

--Signal declarations
signal FIFO_U32 : FIFO_U32_TYPE(0 to NUMBER_OF_INPUT_WORDS-1);
signal FIFO_U8 : FIFO_U8_TYPE(0 to NUMBER_OF_OUTPUT_BYTES*NUMBER_OF_INPUT_WORDS-1);
signal rcv_count_r2g : unsigned (1 downto 0);
signal send_count_r2g : unsigned (2 downto 0);
signal data_missed : std_logic;
signal m_valid : std_logic;
signal m_ready : std_logic;
signal tlast_count : unsigned (20 downto 0);
signal row_r2g : unsigned (11 downto 0); -- 512 = 200
signal col_r2g : unsigned (11 downto 0); -- 768 = 300
signal data_out : FIFO_U8_TYPE(0 to 3);

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
	               row_r2g <= resize(unsigned(S_AXI_WDATA),row_r2g'length);
	               sel_parameters_axi_lite <= '1';
	           else
	               col_r2g <= resize(unsigned(S_AXI_WDATA),col_r2g'length);
	           end if;
	        end if;
	    end if;
	end if;
 end process;
 
------------------------------- AXI LITE---------------------------------------

    gen_row:for i in 0 to NUMBER_OF_INPUT_WORDS-1 generate
         gen_col:for j in 0 to 3 generate
             FIFO_U8(4*i+j) <= FIFO_U32(i)(8*j+7 downto 8*j) ;
         end generate;
    end generate; 

 
data_missed <= '1' when (m_valid = '1' and m_ready = '0') else '0';
M_AXIS_TVALID <= m_valid;
m_ready <= M_AXIS_TREADY;

M_AXIS_TLAST <= '0';
S_AXIS_TREADY <= '1' when rgb2gray_fsm = RCV else '0';

M_AXIS_TKEEP <= "11";
M_AXIS_TLAST <= '1' when tlast_count = (col_r2g*row_r2g)-1 else '0';

rgb2gray_process:process(AXIS_ACLK)
begin
    if rising_edge(AXIS_ACLK) then
        if AXIS_ARESETN = '0' then
        --Others signals--
            rgb2gray_fsm <= START;
            rcv_count_r2g <= (others=>'0');
            send_count_r2g <= (others=>'0');
            m_valid <= '0';
        else
            --FSM--
            case rgb2gray_fsm is
            
                when START =>
                    rgb2gray_fsm <= RCV;
                
                when RCV =>  
                        if S_AXIS_TVALID = '1' then
                            rcv_count_r2g <= rcv_count_r2g + 1;
                            FIFO_U32(to_integer(rcv_count_r2g)) <= S_AXIS_TDATA;
                            if rcv_count_r2g = NUMBER_OF_INPUT_WORDS-1 then
                                rcv_count_r2g <= (others=>'0');
                                rgb2gray_fsm <= AVG;
                            end if;
                        end if;  
                
                when AVG =>
                
                    data_out(0) <= promedio(FIFO_U32(0)(7 downto 0),FIFO_U32(0)(15 downto 8),FIFO_U32(0)(23 downto 16));
                    data_out(1) <= promedio(FIFO_U32(0)(31 downto 24),FIFO_U32(1)(7 downto 0),FIFO_U32(1)(15 downto 8));
                    data_out(2) <= promedio(FIFO_U32(1)(23 downto 16),FIFO_U32(1)(31 downto 24),FIFO_U32(2)(7 downto 0));
                    data_out(3) <= promedio(FIFO_U32(2)(15 downto 8),FIFO_U32(2)(23 downto 16),FIFO_U32(2)(31 downto 24));
                    
                    if m_ready = '1' then
                        rgb2gray_fsm <= SEND;
                    end if;
                
                when SEND =>
                    if data_missed = '1' then
                        rgb2gray_fsm <= MISSED; 
                    elsif send_count_r2g = NUMBER_OF_OUTPUT_BYTES then
                        m_valid <= '0';
                        send_count_r2g <= (others=>'0');
                        rgb2gray_fsm <= RCV;
                    else
                        m_valid <= '0';
                        send_count_r2g <= send_count_r2g + 1;
                        tlast_count <= tlast_count + 1;
                        --M_AXIS_TDATA <= promedio(FIFO_U8((to_integer(send_count_r2g))*3),FIFO_U8((to_integer(send_count_r2g))*3+1),FIFO_U8((to_integer(send_count_r2g))*3+2));
                          M_AXIS_TDATA <= data_out(to_integer(send_count_r2g));  
                            if m_ready='1' then
                                m_valid <= '1';
                            else
                                rgb2gray_fsm <= WAIT_READY;
                            end if;
                    end if;
                            
                when WAIT_READY =>
                    if m_ready='1' then
                        m_valid <= '1';
                        rgb2gray_fsm <= SEND;
                    end if;
                    
                when MISSED =>
                    if m_ready='1' then
                        m_valid <= '0';
                        rgb2gray_fsm <= SEND;
                    end if;
                        
                when others => null;
            end case;  
            --FSM--      
        end if;
    end if;
end process;

end Behavioral;














