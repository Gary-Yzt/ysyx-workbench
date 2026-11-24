#include <nvboard.h>
#include <Vtop.h>
#include <cstdio>
#include <cstdlib>

static TOP_NAME dut;

void nvboard_bind_all_pins(TOP_NAME *top);

// 一拍时钟 = 低电平求值一次 + 高电平求值一次
static void single_cycle() {
  dut.clk = 0; dut.eval();
  dut.clk = 1; dut.eval();
}

// 复位：rst 拉高保持 n 个周期
static void reset(int n) {
  dut.rst = 1;
  while (n-- > 0) single_cycle();
  dut.rst = 0;
}

int main(int argc, char *argv[]) {
  // 可选参数：仿真多少个周期后退出；不传则一直运行（按 Ctrl+C 退出）
  long long max_cycles = -1;
  if (argc > 1) max_cycles = atoll(argv[1]);

  nvboard_bind_all_pins(&dut);   // 按 constr/top.nxdc 自动生成的代码绑定引脚
  nvboard_init();                // 打开 NVBoard 虚拟板卡窗口

  reset(10);                     // 复位 10 个周期

  unsigned short last_led = dut.led;
  printf("reset done, led = 0x%04x\n", last_led);
  fflush(stdout);

  long long tick = 0;
  for (long long cycles = 0; max_cycles < 0 || cycles < max_cycles; cycles++) {
    // 每 5 万个周期刷新一次虚拟板卡，既能及时响应窗口事件，又不会拖慢仿真
    if (++tick >= 50000) { tick = 0; nvboard_update(); }

    single_cycle();

    if (dut.led != last_led) {   // 灯的亮灭发生变化时打印一行，便于无界面验证
      last_led = dut.led;
      printf("cycle %lld: led = 0x%04x\n", cycles, last_led);
      fflush(stdout);
      nvboard_update();
    }
  }

  nvboard_quit();
  return 0;
}
