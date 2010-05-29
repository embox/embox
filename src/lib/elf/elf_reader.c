/**
 * @file
 * @brief ELF (Executable and Linkable Format) parser.
 *
 * @date 27.04.10
 * @author Aleksandr Kirov
 * FIXME: aggrh, spaghetti code turns your brain into Camembert cheese.(sikmir)
 */

#include <lib/elf_reader.h>
#include <string.h>

uint32_t elf_reverse_long(uint32_t num, uint8_t reversed) {
        switch (reversed) {
        case 0:
        case 1:
                break;
        case 2 :
                return   ((num & 0x000000ff) << 24)
                       + ((num & 0x0000ff00) << 8)
                       + ((num & 0x00ff0000) >> 8)
                       + ((num & 0xff000000) >> 24);
        }
        return num;
}

uint16_t elf_reverse_short(uint16_t num, uint8_t reversed) {
        switch (reversed) {
        case 0:
        case 1:
                break;
        case 2 :
                return ((num & 0x00ff ) << 8) + ((num & 0xff00) >> 8);
        }
        return num;
}

int32_t elf_read_header(FILE *fd, Elf32_Ehdr *hdr) {
        return fread(hdr, sizeof(Elf32_Ehdr), 1, fd);
}

int32_t elf_read_sections_table(FILE *fd, Elf32_Ehdr *hdr,
                                Elf32_Shdr *section_hdr_table) {
        size_t size, nmemb;
        long offset;
        if (elf_reverse_long(hdr->e_shoff, hdr->e_ident[5]) != 0) {
    		offset = elf_reverse_long(hdr->e_phoff, hdr->e_ident[5]);
                fseek(fd, offset, 0);
                size = elf_reverse_short(hdr->e_shentsize, hdr->e_ident[5]);
                nmemb = elf_reverse_short(hdr->e_shnum, hdr->e_ident[5]);
                return fread(&section_hdr_table, size, nmemb, fd);
        } else {/*Table doesn't exist*/
                return -1;
        }
}

int32_t elf_read_segments_table(FILE *fd, Elf32_Ehdr *hdr,
                                Elf32_Phdr *segment_hdr_table) {
        size_t size, nmemb;
        long offset = elf_reverse_long(hdr->e_phoff, hdr->e_ident[5]);
        if (offset != 0) {
                fseek(fd, offset, 0);
                size = elf_reverse_short(hdr->e_shentsize, hdr->e_ident[5]);
                nmemb = elf_reverse_short(hdr->e_shnum, hdr->e_ident[5]);
                return fread(segment_hdr_table, size, nmemb, fd);
        } else {/*Table doesn't exist*/
                return -1;
        }
}

int32_t elf_read_string_table(FILE *fd, Elf32_Ehdr *hdr,
                              Elf32_Shdr *section_hdr_table,
                              int8_t *names, int32_t *length) {
        size_t idx;
        long offset;
        if (elf_reverse_long(hdr->e_shoff,hdr->e_ident[5]) == 0) {
    		/*Empty table*/
    		return -2;
    	}
        if ( hdr->e_shstrndx != 0 ) {
    		idx = elf_reverse_short(hdr->e_shstrndx, hdr->e_ident[5]);
            	offset = elf_reverse_long(section_hdr_table[idx].sh_offset,
            					    hdr->e_ident[5]);
                fseek(fd, offset, 0);
                *length = elf_reverse_long(section_hdr_table[idx].sh_size,
                    				    hdr->e_ident[5]);
                fread(names, *length, 1, fd);
                return *length;
        } else {/*Not found*/
                return -1;
        }
}

int32_t elf_read_symbol_table(FILE *fd, Elf32_Ehdr *hdr,
                              Elf32_Shdr *section_header_table,
                              Elf32_Sym *symbol_table, int32_t *count) {
        size_t size, i;
        long offset;
        if (elf_reverse_long(hdr->e_shoff, hdr->e_ident[5]) == 0) {
    		/*empty table*/
    		return -2;
	}

        *count = 0;

        for (i = 0; i < elf_reverse_short(hdr->e_shnum, hdr->e_ident[5]); i++) {
                /*We find SHT_SYMTAB - sh_type with value - 2*/
                if (elf_reverse_long(section_header_table[i].sh_type,
                                             hdr->e_ident[5]) == 2) {
                        offset = elf_reverse_long(section_header_table[i].sh_offset,
                            				hdr->e_ident[5]);
                        fseek(fd, offset, 0);
                        size =  elf_reverse_long(section_header_table[i].sh_size,
                            				hdr->e_ident[5]);
                        fread(symbol_table, size, 1, fd);
                        *count = elf_reverse_long(section_header_table[i].sh_size,
                                                hdr->e_ident[5]) / sizeof(Elf32_Sym);
        		return *count;
                }
        }
        return (*count == 0) ? -1 : -3;
}

int32_t elf_read_rel_table(FILE *fd, Elf32_Ehdr *hdr,
                           Elf32_Shdr *section_hdr_table,
                           Elf32_Rel *rel_table, int32_t *count) {
        size_t size, i;
        long offset;
        if (elf_reverse_long(hdr->e_shoff, hdr->e_ident[5]) == 0) {
    		/*Section header table is empty*/
    		return -2;
        }
        *count = 0;

        for ( i = 0; i < elf_reverse_short(hdr->e_shnum, hdr->e_ident[5]) ; i++) {
                /*We find SHT_REL - sh_type with value - 9*/
                if (elf_reverse_long(section_hdr_table[i].sh_type,
                                         hdr->e_ident[5]) == 9) {
                        offset = elf_reverse_long(section_hdr_table[i].sh_offset,
                    				    hdr->e_ident[5]);
                        fseek(fd, offset, 0);
                        size = elf_reverse_long(section_hdr_table[i].sh_size, hdr->e_ident[5]);
                        fread(rel_table + *count, size, 1, fd);
                        *count += elf_reverse_long(section_hdr_table[i].sh_size,
                                                   hdr->e_ident[5]) / sizeof(Elf32_Rel);
                }
        }
        return (*count != 0) ? *count : -1;
}

int32_t elf_read_rela_table(FILE *fd, Elf32_Ehdr *hdr,
                            Elf32_Shdr * section_hdr_table,
                            Elf32_Rela * rela_table, int32_t * count) {
        size_t size, i;
        long offset;
        if (elf_reverse_long(hdr->e_shoff, hdr->e_ident[5]) == 0) {
    		/*Section header table is empty*/
    		return -2;
        }
        *count = 0;

        for (i = 0; i < elf_reverse_short(hdr->e_shnum, hdr->e_ident[5]) ; i++) {
                /*We find SHT_RELA - sh_type with value - 4*/
                if (elf_reverse_long(section_hdr_table[i].sh_type,
                                     hdr->e_ident[5]) == 4) {
                        offset = elf_reverse_long(section_hdr_table[i].sh_offset,
                    				hdr->e_ident[5]);
                        fseek(fd, offset, 0);
                        size = elf_reverse_long(section_hdr_table[i].sh_size,
                            				hdr->e_ident[5]);
                        fread(rela_table + *count, size, 1, fd);
                        *count += elf_reverse_long(section_hdr_table[i].sh_size,
                                            hdr->e_ident[5]) / sizeof(Elf32_Rela);
                }
        }
        return (*count != 0) ? *count : -1;
}

int32_t read_name(int8_t *names_array, int32_t index, int8_t *name) {
        int32_t i = index;

        do {
                name[i-index] = names_array[i];
                i++;
        } while ((names_array[i-1] != '\0') && (i < MAX_NAME_LENGTH));

        return (i == MAX_NAME_LENGTH) ? 0 : i;
}

int32_t elf_read_symbol_string_table(FILE *fd, Elf32_Ehdr *hdr,
                                     Elf32_Shdr *section_hdr_table,
                                     int8_t *sections_names,
                                     int8_t *names, int32_t *ret_length) {
        size_t size, i, length;
        int8_t section_name[100];
	long offset;

        if(sections_names == 0) {
                return -3;
        }

        if (elf_reverse_long(hdr->e_shoff, hdr->e_ident[5]) == 0) {
    		/*empty table*/
                return -2;
        }

        *ret_length = 0;

        for (i = 0; i < elf_reverse_short(hdr->e_shnum,hdr->e_ident[5]) ; i++) {
                /*We find SHT_STRTAB - sh_type with value - 3*/
                if (elf_reverse_long(section_hdr_table[i].sh_type,
                                     hdr->e_ident[5]) != 2 ) {
            		continue;
                }
                length = read_name(sections_names ,elf_reverse_long(
                                           section_hdr_table[i].sh_name,
                                           hdr->e_ident[5]), section_name);

                if((length != 0) && strstr((const char*)section_name, ".strtab")) {
                        /*We found section with name .strtab and type SHT_STRTAB*/
                        /*such strings ,must contain symbol names*/
			offset = elf_reverse_long(section_hdr_table[i].sh_offset,
								    hdr->e_ident[5]);
                        fseek(fd, offset, 0);

			size = elf_reverse_long(section_hdr_table[i].sh_size,
								    hdr->e_ident[5]);
                        fread(names, size, 1, fd);
                        *ret_length = elf_reverse_long(section_hdr_table[i].sh_size,
                                                                    hdr->e_ident[5]);
                        return *ret_length;
                }
        }
        return -1;
}

