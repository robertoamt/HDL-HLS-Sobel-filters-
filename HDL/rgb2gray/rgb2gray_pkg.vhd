library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

package rgb2gray_pkg is

--Type declarations
type rgb2gray_fsm_type is (START,RCV,AVG,SEND,MISSED,WAIT_READY);
type FIFO_U32_TYPE is array (natural range <>) of std_logic_vector(31 downto 0);
type FIFO_U8_TYPE is array (natural range<>) of std_logic_vector(7 downto 0);

----------------------------Component declaration--------------------------
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

function promedio (data1,data2,data3 : std_logic_vector) return std_logic_vector;


end package rgb2gray_pkg;

----------------------------------------------------------------------

package body rgb2gray_pkg is 

---------------AVG function
function promedio (data1,data2,data3 : std_logic_vector)
return std_logic_vector is
variable d_1 : std_logic_vector (0 to data1'length-1) := data1;
variable d_2 : std_logic_vector (0 to data2'length-1) := data2;
variable d_3 : std_logic_vector (0 to data3'length-1) := data3;
variable suma : unsigned (9 downto 0);
constant width : integer := suma'length;
variable result : unsigned (7 downto 0);
begin
    suma := resize(unsigned(d_1),width) + resize(unsigned(d_2),width) + resize(unsigned(d_3),width);
    suma := suma/3;
    result := resize(suma,result'length);
    return std_logic_vector(result);
end;

end package body rgb2gray_pkg;
