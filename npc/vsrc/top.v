// 顶层：把流水灯接到 NVBoard 的 16 个 LED 上
// clk / rst 由 C++ 仿真驱动程序驱动，不占用 NVBoard 引脚
module top(
  input clk,
  input rst,
  output [15:0] led
);
  light u_light(
    .clk(clk),
    .rst(rst),
    .led(led)
  );
endmodule
