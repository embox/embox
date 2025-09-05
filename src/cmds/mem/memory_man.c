/**
 * @file memory_man.c
 * @brief Memory utilities
 * @author Mikhael Kaa (Михаил Каа)
 * @date 05.09.2025
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

// Constants
#define PROGRESS_UPDATE_BYTES (8 * 1024)  // 8KB
#define BYTES_PER_LINE 16
#define MAX_ERRORS_TO_PRINT 10

static void print_usage(void);
static void mem_dump(uint8_t *buf, uint32_t len);
static uint32_t mem_test(uint8_t *buf, uint32_t len);

int main(int argc, char *argv[]) {
    uintptr_t addr, src;  
    uint32_t data, len;
    uint8_t *ptr;

    switch (argc) {
    case 1:
    case 2:
        print_usage();
        return -EINVAL;
    
    case 3:
        if (strcmp(argv[1], "read") == 0) {
            if (sscanf(argv[2], "%" SCNxPTR, &addr) != 1) { 
                printf("Invalid address format\n");
                return -EINVAL;
            }
            ptr = (uint8_t *)addr;
            printf("0x%02x\n", *ptr);
            return 0;
        }
        break;
    
    case 4:
        if (strcmp(argv[1], "test") == 0) {
            if (sscanf(argv[2], "%" SCNxPTR, &addr) != 1 ||
                sscanf(argv[3], "%x", &len) != 1)  
            {
                printf("Invalid arguments format\n");
                return -EINVAL;
            }
            return mem_test((uint8_t *)addr, len);
        }
        
        if (strcmp(argv[1], "dump") == 0) {
            if (sscanf(argv[2], "%" SCNxPTR, &addr) != 1 ||
                sscanf(argv[3], "%x", &len) != 1) 
            {
                printf("Invalid arguments format\n");
                return -EINVAL;
            }
            mem_dump((uint8_t *)addr, len);
            return 0;
        }
        
        if (strcmp(argv[1], "write") == 0) {
            if (sscanf(argv[2], "%" SCNxPTR, &addr) != 1 ||
                sscanf(argv[3], "%x", &data) != 1) 
            {
                printf("Invalid arguments format\n");
                return -EINVAL;
            }
            ptr = (uint8_t *)addr;
            *ptr = (uint8_t)data;
            return 0;
        }
        break;
    
    case 5:
        if (strcmp(argv[1], "cpy") == 0) {
            if (sscanf(argv[2], "%" SCNxPTR, &addr) != 1 ||
                sscanf(argv[3], "%" SCNxPTR, &src) != 1 ||
                sscanf(argv[4], "%x", &len) != 1) 
            {
                printf("Invalid arguments format\n");
                return -EINVAL;
            }
            memcpy((void *)addr, (void *)src, len);
            return 0;
        }
        break;

    default:
        print_usage();
        return -EINVAL;
    }
    
    print_usage();
    return -EINVAL;
}

static void print_usage(void) {
    printf("Usage: mem <command> [arguments]\n");
    printf("Commands:\n");
    printf("  dump <adr> <len>    - Hexdump of memory region\n");
    printf("  read <adr>          - Read byte from address\n");
    printf("  write <adr> <data>  - Write byte to address\n");
    printf("  test <adr> <len>    - Test memory region\n");
    printf("  cpy <dst> <src> <len> - Copy memory block\n");
}

static void mem_dump(uint8_t *buf, uint32_t len) {
    uint32_t i = 0;
    uintptr_t current_addr = (uintptr_t)buf;
    
    while (i < len) {
        /* Print address at start of each line */
        printf("0x%08" PRIxPTR ": ", current_addr);
        
        /* Print hex values */
        uint32_t bytes_printed = 0;
        for (uint32_t j = 0; j < BYTES_PER_LINE; j++) {
            if (i + j < len) {
                printf("%02x ", buf[i + j]);
                bytes_printed++;
            } else {
                printf("   "); /* Padding for incomplete lines */
            }
        }
        
        /* Print ASCII representation */
        printf("| ");
        for (uint32_t j = 0; j < bytes_printed; j++) {
            uint8_t c = buf[i + j];
            putchar((c >= 32 && c <= 126) ? c : '.');
        }
        printf("\n");
        
        i += bytes_printed;
        current_addr += bytes_printed;
    }
}

static uint32_t mem_test(uint8_t *buf, uint32_t len) {
    uint32_t error_count = 0;
    volatile uint8_t *b = buf;
    const uint8_t patterns[] = {0x00, 0x55, 0xAA, 0xFF};
    const int num_patterns = sizeof(patterns) / sizeof(patterns[0]);
    

    const uint32_t progress_interval = (len > PROGRESS_UPDATE_BYTES) ? 
                                      PROGRESS_UPDATE_BYTES : 
                                      (len / 16 + 1);

    if (len == 0) {
        printf("Zero-length test skipped\n");
        return 0;
    }

    printf("Testing %" PRIu32 " bytes...\n", len);

    for (uint32_t i = 0; i < len; i++) {
        uint8_t original = b[i];

        /* Test patterns */
        for (int p = 0; p < num_patterns; p++) {
            uint8_t test_val = patterns[p];
            b[i] = test_val;
            
            if (b[i] != test_val) {
                if (error_count < MAX_ERRORS_TO_PRINT) {
                    printf("ERROR @ 0x%08" PRIxPTR ": Wrote 0x%02X, Read 0x%02X\n", 
                          (uintptr_t)(b + i), test_val, b[i]);
                }
                error_count++;
            }
        }

        /* Restore original value */
        b[i] = original;
        if (b[i] != original) {
            if (error_count < MAX_ERRORS_TO_PRINT) {
                printf("RESTORE ERROR @ 0x%08" PRIxPTR "! Original: 0x%02X, Current: 0x%02X\n",
                      (uintptr_t)(b + i), original, b[i]);
            }
            error_count++;
        }

        /* Progress indicator */
        if (progress_interval > 0 && (i % progress_interval) == 0) {
            printf("Progress: %lu/%lu bytes (errors: %lu)\r", i, len, error_count);
            fflush(stdout); 
        }
    }

    printf("\n"); 

    /* Final report */
    if (error_count == 0) {
        printf("Memory test PASSED: %" PRIu32 " bytes\n", len);
    } else {
        printf("Memory test FAILED! Errors: %" PRIu32 "/%" PRIu32 " bytes\n", 
               error_count, len);
    }
    
    return error_count;
}