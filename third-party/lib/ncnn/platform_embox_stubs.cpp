namespace ncnn {

int get_kmp_blocktime() { return 0; }
void set_kmp_blocktime(int) {}

int get_flush_denormals() { return 0; }
void set_flush_denormals(int) {}

int get_physical_big_cpu_count() { return 1; }

int is_current_thread_running_on_a53_a55() { return 0; }

int cpu_support_arm_asimdhp() { return 0; }
int cpu_support_riscv_zvfh() { return 0; }

} // namespace ncnn
