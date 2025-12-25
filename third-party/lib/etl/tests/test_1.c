// #include <embox/unit.h>
// #include <shell/shell.h>
// #include <stdio.h>

// static int etl_test_cmd(int argc, char **argv) {
//     printf("ETL Test command executed\n");

//     // Sample C code printing values
//     int vec[10];
//     for (int i = 0; i < 5; i++) {
//         vec[i] = i * 10;
//     }
//     printf("ETL vector contents:\n");
//     for (int i = 0; i < 5; i++) {
//         printf("  %d\n", vec[i]);
//     }
//     return 0;
// }

// static const shell_cmd_t shell_etl_test_cmd = {
//     .cmd = "etl_test",
//     .help = "Run ETL test",
//     .handler = etl_test_cmd,
// };

// EMBOX_UNIT_INIT(etl_test_shell_init);

// static int etl_test_shell_init(void) {
//     shell_cmd_register(&shell_etl_test_cmd);
//     return 0;
// }
