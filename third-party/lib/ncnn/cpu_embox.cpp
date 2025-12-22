#include "platform.h"
#include "cpu.h"

namespace ncnn {

static int g_num_threads = 1;
static int g_cpu_powersave = 0;

int get_cpu_count()               { return 1; }
int get_little_cpu_count()        { return 1; }
int get_big_cpu_count()           { return 0; }
int get_max_threads()             { return 1; }
int get_default_num_threads()     { return 1; }

int get_cpu_powersave()           { return g_cpu_powersave; }
// void set_cpu_powersave(int mode)  { g_cpu_powersave = mode; }

void set_omp_num_threads(int n)   { g_num_threads = (n > 0 ? n : 1); }
int  get_omp_num_threads()        { return g_num_threads; }

int get_cpu_support_arm_neon()    { return 0; }
int get_cpu_support_arm_vfpv4()   { return 0; }
int get_cpu_support_arm_asimdhp() { return 0; }
int get_cpu_support_arm_sve()     { return 0; }
int get_cpu_support_arm_sve2()    { return 0; }

int get_cpu_support_x86_avx()     { return 0; }
int get_cpu_support_x86_avx2()    { return 0; }
int get_cpu_support_x86_fma()     { return 0; }
int get_cpu_support_x86_avx512()  { return 0; }

int get_cpu_support_riscv_v()     { return 0; }

int set_sched_affinity(const CpuSet&)
{
    return 0;
}

} // namespace ncnn
