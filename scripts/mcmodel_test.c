// scripts/mcmodel_test.c
     // Test program to detect -mcmodel=medlow relocation issues
     char large_array[2 * 1024 * 1024]; // 2 MiB array to force large relocation

     char *get_ptr() {
         return &large_array[0];
     }