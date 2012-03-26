/**
 * @file
 * @brief Defines pages for MMU in x86 architecture.
 *
 * @date 20.03.12
 * @author Gleb Efimov
 */

#ifndef MMU_PAGE_H_
#define MMU_PAGE_H_

/* This structure contains the value of one GDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure. */
struct gdt_entry_struct
{
   u16int limit_low;           // The lower 16 bits of the limit.
   u16int base_low;            // The lower 16 bits of the base.
   u8int  base_middle;         // The next 8 bits of the base.
   u8int  access;              // Access flags, determine what ring this segment can be used in.
   u8int  granularity;
   u8int  base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
   u16int limit;               // The upper 16 bits of all selector limits.
   u32int base;                // The address of the first gdt_entry_t struct.
}
 __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

void init_gdt(void);

#endif /* MMU_PAGE_H_ */
