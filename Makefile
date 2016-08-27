 #
 #  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 #
 #  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 #
 #  X1000 series loader for u-boot/rtos/linux
 #
 #  This program is free software; you can redistribute it and/or modify it
 #  under  the terms of the GNU General  Public License as published by the
 #  Free Software Foundation;  either version 2 of the License, or (at your
 #  option) any later version.
 #
 #  You should have received a copy of the GNU General Public License along
 #  with this program; if not, write to the Free Software Foundation, Inc.,
 #  675 Mass Ave, Cambridge, MA 02139, USA.
 #
 #

SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi; fi)

#
# Top directory
#
TOPDIR := $(shell pwd)

#
# Cross compiler
#
CROSS_COMPILE := mips-linux-gnu-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy
AWK := awk

#
# Out & Tools directory
#
OUTDIR := $(TOPDIR)/out
$(shell [ -d $(OUTDIR) ] || mkdir -p $(OUTDIR))
OUTDIR := $(shell cd $(OUTDIR) && /bin/pwd)
$(if $(OUTDIR),,$(error output directory "$(OUTDIR)" does not exist))
TOOLSDIR := $(TOPDIR)/tools
SLEEPLIB := $(TOPDIR)/sleep-lib/sleep_lib_tcsm_pad.bin

ifneq ($(SLEEPLIB), $(wildcard $(SLEEPLIB)))
$(error Could not found sleep lib!)
endif

#
# Timestamp file
#
TIMESTAMP_FILE = $(TOPDIR)/include/generated/timestamp_autogenerated.h

#
# Includes
#
ifeq ($(TOPDIR)/include/generated/config.mk, $(wildcard $(TOPDIR)/include/generated/config.mk))
include $(TOPDIR)/config.mk
include $(TOPDIR)/include/generated/config.mk

ifdef BOARD
ifeq ($(TOPDIR)/boards/$(BOARD)/board.mk, $(wildcard $(TOPDIR)/boards/$(BOARD)/board.mk))
	include $(TOPDIR)/boards/$(BOARD)/board.mk
else
$(error Could not found board named "$(BOADR)"!)
endif

else
$(error Please define BOARD first!)
endif

ifeq ($(BOOT_NEXT_STAGE), 0)
CONFIG_BOOT_UBOOT := y
else
ifeq ($(BOOT_NEXT_STAGE), 1)
CONFIG_BOOT_KERNEL := y
else
$(error Boot next stage "$(BOOT_NEXT_STAGE)" has not support yet!)
endif
endif

#
# U-boot load & entry address
#
ifeq ($(CONFIG_BOOT_UBOOT), y)
BOOT_NEXT_STAGE_LOAD_ADDR := 0x80100000
BOOT_NEXT_STAGE_ENTRY_ADDR := 0x80100000
endif

#
# Kernel load & entry address
#
ifeq ($(CONFIG_BOOT_KERNEL), y)
BOOT_NEXT_STAGE_LOAD_ADDR := 0x80f00000

ifeq ($(KERNEL_IN_XIMAGE), 1)
OFFSET_LEN := 0x40
BOOT_NEXT_STAGE_LOAD_ADDR := $(shell $(AWK) 'BEGIN{printf("0x%x\n",            \
	'$(BOOT_NEXT_STAGE_LOAD_ADDR)'-$(OFFSET_LEN));                             \
	}')
endif

BOOT_NEXT_STAGE_ENTRY_ADDR := 0x80f00000
endif

#
# Configure flags
#
CFGFLAGS := -DCONFIG_BOOT_NEXT_STAGE_LOAD_ADDR=$(BOOT_NEXT_STAGE_LOAD_ADDR)    \
            -DCONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR=$(BOOT_NEXT_STAGE_ENTRY_ADDR)

ifeq ($(CONFIG_BOOT_KERNEL), y)
CFGFLAGS += -DCONFIG_BOOT_KERNEL
else
CFGFLAGS += -DCONFIG_BOOT_UBOOT
endif

ifdef BOOT_FROM
	ifeq ($(BOOT_FROM), nor)
		CFGFLAGS += -DCONFIG_BOOT_SPI_NOR -DCONFIG_BOOT_SFC
		CONFIG_BOOT_SPI_NOR=y
	endif
	ifeq ($(BOOT_FROM), nand)
		CFGFLAGS += -DCONFIG_BOOT_SPI_NAND -DCONFIG_BOOT_SFC
		CONFIG_BOOT_SPI_NAND=y
	endif
	ifeq ($(BOOT_FROM), mmc)
		CFGFLAGS += -DCONFIG_BOOT_MMC
		CONFIG_BOOT_MMC=y
	endif

#
# Compiler & Linker options
#
CFLAGS := -Os -g -G 0 -march=mips32r2 -mtune=mips32r2 -mabi=32 -fno-pic        \
          -fno-builtin -mno-abicalls -nostdlib -EL -msoft-float -std=gnu11     \
          -I$(TOPDIR)/include -ffunction-sections -fdata-sections

CHECKFLAGS := -Wall -Wuninitialized -Wstrict-prototypes -Wundef -Werror
LDFLAGS := -nostdlib -T ldscripts/x-loader.lds -EL --gc-sections
OBJCFLAGS := --gap-fill=0xff --remove-section=.dynsym
#DEBUGFLAGS := -DDEBUG
override CFLAGS := $(CHECKFLAGS) $(DEBUGFLAGS) $(CFLAGS) $(CFGFLAGS) $(BOARD_CFLAGS)

else
$(error Please define system boot method(nor/nand/mmc)!)
endif

#
# Sources
#
OBJS-y := start.o                                                              \
          main.o                                                               \
          boot.o                                                               \
          drivers/lpddr.o                                                      \
          drivers/uart.o                                                       \
          drivers/clk.o                                                        \
          drivers/gpio.o                                                       \
          drivers/i2c.o

OBJS-$(CONFIG_BOOT_KERNEL) += boot_sel.o

OBJS-y += common/printf.o                                                      \
          common/common.o

OBJS-y += boards/$(BOARD)/board.o

OBJS-$(CONFIG_BOOT_MMC) +=  drivers/mmc.o
OBJS-$(CONFIG_BOOT_SPI_NAND) += drivers/sfc.o                                  \
                                drivers/spinand.o
OBJS-$(CONFIG_BOOT_SPI_NOR) +=  drivers/sfc.o                                  \
                                drivers/spinor.o

OBJS := $(addprefix $(TOPDIR)/, $(OBJS-y))

LIBS-y :=
LIBS := $(addprefix $(TOPDIR)/, $(LIBS-y))

#
# Targets
#
ifneq ($(CONFIG_BOOT_MMC),y)
TARGET := $(OUTDIR)/x-loader-pad-with-sleep-lib.bin
else
TARGET := $(OUTDIR)/x-loader-pad-with-mbr-gpt-with-sleep-lib.bin
endif

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.S
	$(CC) -c $(CFLAGS) -D__ASSEMBLY__ $< -o $@

.PHONY: all clean Tips backup

all: clean $(TARGET) Tips

Tips: $(TARGET)
	@echo -e '\n  Image: "$(TARGET)" is ready\n'

$(OUTDIR)/x-loader-pad-with-mbr-gpt-with-sleep-lib.bin: $(OUTDIR)/x-loader-pad-with-mbr-gpt.bin
	cat $< $(SLEEPLIB) > $@

$(OUTDIR)/x-loader-pad-with-sleep-lib.bin: $(OUTDIR)/x-loader-pad.bin
	cat $< $(SLEEPLIB) > $@

$(OUTDIR)/x-loader-pad-with-mbr-gpt.bin: $(OUTDIR)/mbr-gpt.bin $(OUTDIR)/x-loader-pad.bin $(TOOLSDIR)/spl_params_fixer
	cat $(OUTDIR)/mbr-gpt.bin $(OUTDIR)/x-loader-pad.bin > $@
	$(TOOLSDIR)/spl_params_fixer $@ $(OUTDIR)/x-loader.bin > /dev/null

$(OUTDIR)/x-loader-pad.bin: $(OUTDIR)/x-loader.bin
	$(OBJDUMP) -D $(OUTDIR)/x-loader.elf > $(OUTDIR)/x-loader.elf.dump
	$(OBJCOPY) $(OBJCFLAGS) --pad-to=16384 -I binary -O binary $< $@

ifneq ($(CONFIG_BOOT_MMC),y)
$(OUTDIR)/x-loader.bin: $(OUTDIR)/x-loader.elf $(TOOLSDIR)/sfc_boot_checksum
	$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
	$(TOOLSDIR)/sfc_boot_checksum $@
else
$(OUTDIR)/x-loader.bin: $(OUTDIR)/x-loader.elf
	$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@
endif

$(OUTDIR)/x-loader.elf: $(TIMESTAMP_FILE) $(TOOLSDIR)/ddr_params_creator $(TOOLSDIR)/uart_baudrate_lut $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ -Map $(OUTDIR)/x-loader.map

ifneq ($(CONFIG_BOOT_MMC), y)
$(TOOLSDIR)/sfc_boot_checksum: $(TOOLSDIR)/sfc_boot_checksum.c
	gcc -o $@ -D__HOST__ -I$(TOPDIR)/include $<
	strip $@
else # CONFIG_BOOT_MMC #

$(TOOLSDIR)/spl_params_fixer: $(TOOLSDIR)/spl_params_fixer.c
	gcc -o $@ -D__HOST__ -DCONFIG_BOOT_MMC -I$(TOPDIR)/include $<
	strip $@

ifeq ($(CONFIG_GPT_TABLE), y)
ifneq ($(TOPDIR)/boards/$(BOARD)/partitions.tab, $(wildcard $(TOPDIR)/boards/$(BOARD)/partitions.tab))
$(error Can not found "partitions.tab" for board $(BOARD))
else
$(OUTDIR)/mbr-gpt.bin:	$(TOOLSDIR)/gpt_creator
	$< $(TOPDIR)/boards/$(BOARD)/partitions.tab $(OUTDIR)/mbr-of-gpt.bin $(OUTDIR)/gpt.bin
	cat $(OUTDIR)/mbr-of-gpt.bin $(OUTDIR)/gpt.bin > $@
	dd if=/dev/zero of=$(OUTDIR)/file.bin bs=512 count=33
	cat $@ $(OUTDIR)/file.bin > $(OUTDIR)/file2.bin
	dd if=$(OUTDIR)/file2.bin of=$@ bs=512 count=34
	rm $(OUTDIR)/file* -rf
endif

else
$(OUTDIR)/mbr-gpt.bin: $(OUTDIR)/mbr.bin
	dd if=/dev/zero of=$(OUTDIR)/gpt.bin bs=512 count=33
	cat $(OUTDIR)/mbr.bin $(OUTDIR)/gpt.bin > $@

$(OUTDIR)/mbr.bin: $(TOOLSDIR)/mbr_creator.c
	gcc -o $(TOOLSDIR)/mbr_creator -D__HOST__ -I$(TOPDIR)/include $<
	strip $(TOOLSDIR)/mbr_creator
	$(TOOLSDIR)/mbr_creator \
		p0off=$(CONFIG_MBR_P0_OFF),p0end=$(CONFIG_MBR_P0_END),p0type=$(CONFIG_MBR_P0_TYPE) \
		p1off=$(CONFIG_MBR_P1_OFF),p1end=$(CONFIG_MBR_P1_END),p1type=$(CONFIG_MBR_P1_TYPE) \
		p2off=$(CONFIG_MBR_P2_OFF),p2end=$(CONFIG_MBR_P2_END),p2type=$(CONFIG_MBR_P2_TYPE) \
		p3off=$(CONFIG_MBR_P3_OFF),p3end=$(CONFIG_MBR_P3_END),p3type=$(CONFIG_MBR_P3_TYPE) \
		-o $@ > /dev/zero
endif

endif

$(TOOLSDIR)/ddr_params_creator: $(TOOLSDIR)/ddr_params_creator.c
	gcc -o $@ -D__HOST__ -I$(TOPDIR)/include $<
	strip $@
	$@ > $(TOPDIR)/include/generated/ddr_reg_values.h

$(TOOLSDIR)/uart_baudrate_lut: $(TOOLSDIR)/uart_baudrate_lut.c
	gcc -o $@ -D__HOST__ -I$(TOPDIR)/include $<
	strip $@
	$@ > $(TOPDIR)/include/generated/uart_baudrate_reg_values.h

$(TIMESTAMP_FILE):
	@LC_ALL=C date +'#define X_LOADER_DATE "%b %d %C%y"' > $@.tmp
	@LC_ALL=C date +'#define X_LOADER_TIME "%T"' >> $@.tmp
	@cmp -s $@ $@.tmp && rm -f $@.tmp || mv -f $@.tmp $@

else
all:
	@echo "Please configure first!- see README" >&2
	@exit 1
endif

#
# Board config
#
unconfig:
	rm -f include/generated/config.h include/generated/config.mk

#===============================================================================
phoenix_nor_config: unconfig
	@./mkconfig $(@:_config) phoenix nor

phoenix_nand_config: unconfig
	@./mkconfig $(@:_config) phoenix nand

phoenix_mmc_config: unconfig
	@./mkconfig $(@:_config) phoenix mmc

halley2_nor_config: unconfig
	@./mkconfig $(@:_config) halley2 nor

halley2_nand_config: unconfig
	@./mkconfig $(@:_config) halley2 nand

halley2_mmc_config: unconfig
	@./mkconfig $(@:_config) halley2 mmc

#
# Put your board's config here
#

#===============================================================================

clean:
	rm -rf $(OUTDIR)/* \
			$(OBJS) \
			$(TOOLSDIR)/sfc_boot_checksum \
			$(TOOLSDIR)/ddr_params_creator \
			$(TOOLSDIR)/uart_baudrate_lut \
			$(TOOLSDIR)/mbr_creator \
			$(TOOLSDIR)/spl_params_fixer \
			$(TOPDIR)/include/generated/ddr_reg_values.h \
			$(TOPDIR)/include/generated/uart_baudrate_reg_values.h \
			$(TIMESTAMP_FILE)

distclean: clean unconfig

backup: distclean
	F=`basename $(TOPDIR)` ; cd .. ; \
	tar --force-local --exclude=.git -Jcvf `date "+$$F-%Y-%m-%d-%T.tar.xz"` $$F
