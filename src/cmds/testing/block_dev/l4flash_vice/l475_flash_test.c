#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <lib/crypt/md5.h>
#include <util/math.h>
#include <util/pretty_print.h>


static struct flash_dev *get_flash_dev(struct block_dev *bdev) {
	int i = 0;
	struct flash_dev *fdev;

	while (1) {
		fdev = flash_by_id(i++);
		if (!fdev) {
			break;
		}
		if (!strcmp(block_dev_name(fdev->bdev), block_dev_name(bdev))) {
			return fdev;
		}
	}
	return NULL;
}


#define FLASH_START 0x8000800
#define FLASH_PAGE 8
static struct flash_dev *fdev;
static void print_npages(int n) 
  {
  for(int p=0;p<n;p++)
    {
    uint8_t b[FLASH_PAGE];
    flash_read(fdev, p*FLASH_PAGE, b, FLASH_PAGE);
    for(int i=0;i<FLASH_PAGE;i++) printf(" %02X", b[i]);
    printf("\n");
    }
  printf("\n");
  }

int main(int argc, char **argv)
  {
  struct block_dev *bdev = block_dev_find("stm32flash0");
  if (!bdev) {printf("Block device stm32flash0 not found\n");return -EINVAL;}
  fdev = get_flash_dev(bdev);
  if (!fdev) {printf("Flash device stm32flash0 not found\n");return -EINVAL;}

  printf("stm32flash0:\n%d blocks of %d bytes each\n\n", flash_get_blocks_num(fdev), flash_get_block_size(fdev, 0));
  printf("Initial at 0 offset\n");
  print_npages(4);
  printf("Erasing block 0\n");
  flash_erase(fdev, 0);
  print_npages(4);

  uint8_t b[FLASH_PAGE];
  
  printf("Writing 0, 1, 2,..7 in the 1st page\n");
  for(int i=0;i<FLASH_PAGE;i++) b[i]=i;
  flash_write(fdev, 0, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing 8, 9, A,..,F,FF,..,FF in the 2nd page\n");
  for(int i=0;i<FLASH_PAGE/2;i++) {b[i]=i+8; b[i+FLASH_PAGE/2]=0xFF;}
  flash_write(fdev, FLASH_PAGE, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing all FF in the 3rd page\n");
  for(int i=0;i<FLASH_PAGE;i++) {b[i]=0xFF;}
  flash_write(fdev, FLASH_PAGE*2, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing all FF in the 4th page\n");
  for(int i=0;i<FLASH_PAGE;i++) {b[i]=0xFF;}
  flash_write(fdev, FLASH_PAGE*3, b, FLASH_PAGE);
  print_npages(4);

  printf("Now try writing over:\n\n");
  
  printf("Writing 0, 1, 2,..7 in the 1st page (the same)\n");
  for(int i=0;i<FLASH_PAGE;i++) b[i]=i;
  flash_write(fdev, 0, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing 8, 9, A,..,17 in the 2nd page (the same in first half)\n");
  for(int i=0;i<FLASH_PAGE/2;i++) {b[i]=i+8; b[i+FLASH_PAGE/2]=i+8+FLASH_PAGE/2;}
  flash_write(fdev, FLASH_PAGE, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing 0, 1, 2,..7 in the 3rd page (over all FFs)\n");
  for(int i=0;i<FLASH_PAGE;i++) b[i]=i;
  flash_write(fdev, FLASH_PAGE*2, b, FLASH_PAGE);
  print_npages(4);

  printf("Writing all FF in the 4th page (all FFs over all FFs)\n");
  for(int i=0;i<FLASH_PAGE;i++) {b[i]=0xFF;}
  flash_write(fdev, FLASH_PAGE*3, b, FLASH_PAGE);
  print_npages(4);
 
  return 0;
}
