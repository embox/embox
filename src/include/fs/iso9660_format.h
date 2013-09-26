/**
 * @file
 *
 * @date Sep 26, 2013
 * @author: Anton Bondarev
 */

#ifndef ISO9660_FORMAT_H_
#define ISO9660_FORMAT_H_


#define ISO_VD_BOOT         0
#define ISO_VD_PRIMARY      1
#define ISO_VD_SUPPLEMENTAL 2
#define ISO_VD_PARTITION    3
#define ISO_VD_END          255


#define ISODCL(from, to) (to - from + 1)

//#pragma pack(push, 1)

typedef struct iso_volume_descriptor {
  unsigned char type                    [ISODCL(  1,   1)]; /* 711 */
  unsigned char id                      [ISODCL(  2,   6)];
  unsigned char version                 [ISODCL(  7,   7)]; /* 711 */
  unsigned char flags                   [ISODCL(  8,   8)];
  unsigned char system_id               [ISODCL(  9,  40)]; /* achars */
  unsigned char volume_id               [ISODCL( 41,  72)]; /* dchars */
  unsigned char unused1                 [ISODCL( 73,  80)];
  unsigned char volume_space_size       [ISODCL( 81,  88)]; /* 733 */
  unsigned char escape_sequences        [ISODCL( 89, 120)];
  unsigned char volume_set_size         [ISODCL(121, 124)]; /* 723 */
  unsigned char volume_sequence_number  [ISODCL(125, 128)]; /* 723 */
  unsigned char logical_block_size      [ISODCL(129, 132)]; /* 723 */
  unsigned char path_table_size         [ISODCL(133, 140)]; /* 733 */
  unsigned char type_l_path_table       [ISODCL(141, 144)]; /* 731 */
  unsigned char opt_type_l_path_table   [ISODCL(145, 148)]; /* 731 */
  unsigned char type_m_path_table       [ISODCL(149, 152)]; /* 732 */
  unsigned char opt_type_m_path_table   [ISODCL(153, 156)]; /* 732 */
  unsigned char root_directory_record   [ISODCL(157, 190)]; /* 9.1 */
  unsigned char volume_set_id           [ISODCL(191, 318)]; /* dchars */
  unsigned char publisher_id            [ISODCL(319, 446)]; /* achars */
  unsigned char preparer_id             [ISODCL(447, 574)]; /* achars */
  unsigned char application_id          [ISODCL(575, 702)]; /* achars */
  unsigned char copyright_file_id       [ISODCL(703, 739)]; /* 7.5 dchars */
  unsigned char abstract_file_id        [ISODCL(740, 776)]; /* 7.5 dchars */
  unsigned char bibliographic_file_id   [ISODCL(777, 813)]; /* 7.5 dchars */
  unsigned char creation_date           [ISODCL(814, 830)]; /* 8.4.26.1 */
  unsigned char modification_date       [ISODCL(831, 847)]; /* 8.4.26.1 */
  unsigned char expiration_date         [ISODCL(848, 864)]; /* 8.4.26.1 */
  unsigned char effective_date          [ISODCL(865, 881)]; /* 8.4.26.1 */
  unsigned char file_structure_version  [ISODCL(882, 882)]; /* 711 */
  unsigned char unused2                 [ISODCL(883, 883)];
  unsigned char application_data        [ISODCL(884, 1395)];
  unsigned char unused3                 [ISODCL(1396, 2048)];
} __attribute__ ((packed)) iso_volume_descriptor_t;

typedef struct iso_directory_record {
  unsigned char length                  [ISODCL( 1,  1)]; /* 711 */
  unsigned char ext_attr_length         [ISODCL( 2,  2)]; /* 711 */
  unsigned char extent                  [ISODCL( 3, 10)]; /* 733 */
  unsigned char size                    [ISODCL(11, 18)]; /* 733 */
  unsigned char date                    [ISODCL(19, 25)]; /* 7 by 711 */
  unsigned char flags                   [ISODCL(26, 26)];
  unsigned char file_unit_size          [ISODCL(27, 27)]; /* 711 */
  unsigned char interleave              [ISODCL(28, 28)]; /* 711 */
  unsigned char volume_sequence_number  [ISODCL(29, 32)]; /* 723 */
  unsigned char name_len                [ISODCL(33, 33)]; /* 711 */
  unsigned char name                    [0];
} __attribute__ ((packed)) iso_directory_record_t;

typedef struct iso_extended_attributes {
  unsigned char owner                   [ISODCL(  1,   4)]; /* 723 */
  unsigned char group                   [ISODCL(  5,   8)]; /* 723 */
  unsigned char perm                    [ISODCL(  9,  10)]; /* 9.5.3 */
  unsigned char ctime                   [ISODCL( 11,  27)]; /* 8.4.26.1 */
  unsigned char mtime                   [ISODCL( 28,  44)]; /* 8.4.26.1 */
  unsigned char xtime                   [ISODCL( 45,  61)]; /* 8.4.26.1 */
  unsigned char ftime                   [ISODCL( 62,  78)]; /* 8.4.26.1 */
  unsigned char recfmt                  [ISODCL( 79,  79)]; /* 711 */
  unsigned char recattr                 [ISODCL( 80,  80)]; /* 711 */
  unsigned char reclen                  [ISODCL( 81,  84)]; /* 723 */
  unsigned char system_id               [ISODCL( 85, 116)]; /* achars */
  unsigned char system_use              [ISODCL(117, 180)];
  unsigned char version                 [ISODCL(181, 181)]; /* 711 */
  unsigned char len_esc                 [ISODCL(182, 182)]; /* 711 */
  unsigned char reserved                [ISODCL(183, 246)];
  unsigned char len_au                  [ISODCL(247, 250)]; /* 723 */
} __attribute__ ((packed)) iso_extended_attributes_t;

typedef struct iso_pathtable_record {
  unsigned char length;
  unsigned char ext_attr_length;
  unsigned long extent;
  unsigned short int parent;
  char name[0];
} __attribute__ ((packed)) iso_pathtable_record_t;

//#pragma pack(pop)

#endif /* ISO9660_FORMAT_H_ */
