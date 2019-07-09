/**
 * @file
 * @brief Hardcoded loader of qt library section
 * from LMA section to VMA section
 *
 * @date 08.07.2019
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <string.h>

#define QT_TEXT_VMA   _qt_text_vma
#define QT_TEXT_LMA   _qt_text_lma
#define QT_TEXT_LEN   _qt_text_len

#define QT_RODATA_VMA _qt_rodata_vma
#define QT_RODATA_LMA _qt_rodata_lma
#define QT_RODATA_LEN _qt_rodata_len

#define QT_DATA_VMA   _qt_data_vma
#define QT_DATA_LMA   _qt_data_lma
#define QT_DATA_LEN   _qt_data_len

#define QT_BSS_VMA    _qt_bss_vma
#define QT_BSS_LEN    _qt_bss_len

extern char QT_TEXT_VMA, QT_TEXT_LMA, QT_TEXT_LEN;
extern char QT_RODATA_VMA, QT_RODATA_LMA, QT_RODATA_LEN;
extern char QT_DATA_VMA, QT_DATA_LMA, QT_DATA_LEN;
extern char QT_BSS_VMA, QT_BSS_LEN;

static void load_section(void *vma, void *lma, unsigned int len) {
	memcpy(vma, lma, len);
}

static void zero_bss_section(void *vma, unsigned int len) {
	memset(vma, 0, len);
}

extern void cxx_invoke_constructors(void);

int main(int argc, char **argv) {
	load_section(&QT_TEXT_VMA, &QT_TEXT_LMA, (unsigned int) &QT_TEXT_LEN);
	load_section(&QT_RODATA_VMA, &QT_RODATA_LMA, (unsigned int) &QT_RODATA_LEN);
	load_section(&QT_DATA_VMA, &QT_DATA_LMA, (unsigned int) &QT_DATA_LEN);
	zero_bss_section(&QT_BSS_VMA, (unsigned int) &QT_BSS_LEN);

	cxx_invoke_constructors();

	return 0;
}
