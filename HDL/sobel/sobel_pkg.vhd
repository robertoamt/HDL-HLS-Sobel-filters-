library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

package sobel_pkg is

component axi_lite_core is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 4;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC
);  
end component;

component line_buffer IS
    generic (
        DataWidth : INTEGER;
        AddressRange : INTEGER;
        AddressWidth : INTEGER );
    port (
        clk : IN STD_LOGIC;
        reset : IN STD_LOGIC;
        address0 : IN STD_LOGIC_VECTOR (10 downto 0);
        ce0 : IN STD_LOGIC;
        q0 : OUT STD_LOGIC_VECTOR (7 downto 0);
        address1 : IN STD_LOGIC_VECTOR (10 downto 0);
        ce1 : IN STD_LOGIC;
        we1 : IN STD_LOGIC;
        d1 : IN STD_LOGIC_VECTOR (7 downto 0) );
    end component;

type sobel_fsm_type is (START,NORMAL,SEND,MISSED);
type window_TYPE is array (natural range <>) of std_logic_vector(7 downto 0);

function sobel_kernel (b_2_t, b_1_t, b_in_t : window_TYPE) return std_logic_vector;

end package sobel_pkg;

package body sobel_pkg is

function sobel_kernel (b_2_t, b_1_t, b_in_t : window_TYPE) 
return std_logic_vector is 

variable b_in : window_TYPE (0 to b_in_t'length-1 ) := b_in_t;
variable b_1 : window_TYPE (0 to b_1_t'length-1) := b_1_t;
variable b_2 : window_TYPE (0 to b_2_t'length-1) := b_2_t;
variable x_result_row0 : unsigned (10 downto 0) := (others=>'0');
variable x_result_row2 : unsigned (10 downto 0) := (others=>'0');
variable x_result : unsigned (10 downto 0) := (others=>'0');
variable y_result_row0 : unsigned (10 downto 0) := (others=>'0');
variable y_result_row2 : unsigned (10 downto 0) := (others=>'0');
variable y_result : unsigned (10 downto 0) := (others=>'0');
variable total_result : unsigned (7 downto 0);
constant width : integer := x_result_row0'length;

begin

x_result_row2 := resize(unsigned(b_2(b_2'high)),width) + resize(2*unsigned(b_2(b_2'high-1)),width) + resize(unsigned(b_2(b_2'high-2)),width); 
x_result_row0 := resize(unsigned(b_in(b_in'high)),width)+  resize(2*unsigned(b_in(b_in'high-1)),width) + resize(unsigned(b_in(b_in'high-2)),width); 

if x_result_row0 > x_result_row2 then
    x_result := x_result_row0 - x_result_row2;
else 
    x_result := x_result_row2 - x_result_row0;
end if;

y_result_row2 := resize(unsigned(b_2(b_2'high)),width) + resize(2*unsigned(b_1(b_1'high)),width) + resize(unsigned(b_in(b_in'high)),width); 
y_result_row0 := resize(unsigned(b_2(b_2'high-2)),width) + resize(2*unsigned(b_1(b_1'high-2)),width) + resize(unsigned(b_in(b_in'high-2)),width);

if y_result_row0 > y_result_row2 then
    y_result := y_result_row0 - y_result_row2;
else 
    y_result := y_result_row2 - y_result_row0;
end if;

total_result := resize((x_result + y_result),total_result'length);

return std_logic_vector(total_result);

end;

end package body sobel_pkg;
