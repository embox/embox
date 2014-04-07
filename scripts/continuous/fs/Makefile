
BASE_DIR := .
MOUNT_POINT := $(BASE_DIR)/mountpoint
SUDO=sudo

IMG_RO_CONTENT := $(BASE_DIR)/img-ro
IMG_RW_CONTENT := $(BASE_DIR)/img-rw

fs := vfat
imgs := $(addsuffix .img,$(fs))

.PHONY : all
all : $(imgs)
clean :
	-rm $(imgs)

vfat.img : writeable=1
vfat.img : mkfs_opts=

$(MOUNT_POINT)/. :
	mkdir -p $@

%.img : | $(MOUNT_POINT)
	dd if=/dev/zero bs=1M count=8 of=$@
	mkfs.$* $@
	$(SUDO) mount $@ -t $(fs) $(MOUNT_POINT)
	-$(SUDO) cp -R $(IMG_RO_CONTENT)/* $(MOUNT_POINT)
	$(if $(writeable),-$(SUDO) cp -R $(IMG_RW_CONTENT)/* $(MOUNT_POINT))
	sudo umount $(MOUNT_POINT)

