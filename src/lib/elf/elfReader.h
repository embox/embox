/**
 * @file
 *
 * @date 19.4.10
 * @author Aleksandr Kirov
 */
//*****32-bit data types and sizes*****

//ELF32_Addr 4
#define Elf32_Addr unsigned long

//ELF32_Half 2
#define Elf32_Half unsigned short

//ELF32_Off 4
#define Elf32_Off unsigned long

//ELF32_Sword 4
#define Elf32_Sword signed long

//ELF32_Word 4
#define Elf32_Word unsigned long

//unsigned char 1
#define uchar unsigned char

#define EI_NIDENT 16

//ELF Header
typedef struct {
uchar e_ident[EI_NIDENT];    	     /* ident bytes */
Elf32_Half e_type;                   /* file type */
Elf32_Half e_machine;                /* target machine */
Elf32_Word e_version;                /* file version */
Elf32_Addr e_entry;                  /* start address */
Elf32_Off e_phoff;                   /* phdr file offset */
Elf32_Off e_shoff;                   /* shdr file offset */
Elf32_Word e_flags;                  /* file flags */
Elf32_Half e_ehsize;                 /* sizeof ehdr */
Elf32_Half e_phentsize;              /* sizeof phdr */
Elf32_Half e_phnum;                  /* number phdrs */
Elf32_Half e_shentsize;              /* sizeof shdr */
Elf32_Half e_shnum;                  /* number shdrs */
Elf32_Half e_shstrndx;               /* shdr string index */
} Elf32_Ehdr;

//ELF Section header
typedef struct {
Elf32_Word sh_name ;
Elf32_Word sh_type;
Elf32_Word sh_flags ;
Elf32_Addr sh_addr ;
Elf32_Off sh_offset ;
Elf32_Word sh_size ;
Elf32_Word sh_link ;
Elf32_Word sh_info ;
Elf32_Word sh_addralign ;
Elf32_Word sh_entsize ;
} Elf32_Shdr;


//*****printing functions*****
void printHeader(Elf32_Ehdr * header);
void printSectionHeader(Elf32_Shdr * sectionHeader);//without realisation now
