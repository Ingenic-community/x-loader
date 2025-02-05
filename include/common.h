/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include <boards_common.h>
#ifdef XLOADER_BOARD_CONFIG_FILE
#include XLOADER_BOARD_CONFIG_FILE
#endif

#ifndef __LANGUAGE_ASSEMBLY__

#include <types.h>
#include <uart.h>
#include <gpio.h>
#include <cpm.h>
#include <clk.h>
#include <mmc.h>
#include <sfc.h>
#include <spiflash.h>
#include <lpddr/lpddr.h>
#include <lpddr/lpddr_chip.h>
#include <i2c.h>
#include <efuse.h>
#include <pmon.h>
#include <rtc.h>
#include <pmu.h>
#include <efuse.h>
#include <wdt.h>
#include <aes.h>
#endif

#define CONFIG_SPL_VERSION 1

/*
 * CPU freq uint(MHz)
 */
#ifndef __LANGUAGE_ASSEMBLY__
extern uint32_t cpu_freq;
#endif

/*
 * Kernel params addr
 */
#define KERNEL_PARAMETER_ADDR    0x80004000

/*
 * Sleep lib
 */
#if (defined CONFIG_BOOT_SFC)

/*
 * 12K(SPL) + 4K(PAD)
 */
#define SLEEP_LIB_OFFSET  (16 * 1024)

#elif (defined CONFIG_BOOT_MMC)

/*
 * 17K(MBR+GPT) + 12K(SPL) + 4K(PAD)
 */
#define SLEEP_LIB_OFFSET  (33 * 1024)

#endif
#define SLEEP_LIB_LENGTH  (8 * 1024)
#define SLEEP_LIB_TCSM    (0xb3422000)

/*
 * Memory size flag pass to u-boot
 */
#define CONFIG_MEM_SIZE_MAGIC_ADDR (SLEEP_LIB_TCSM + SLEEP_LIB_LENGTH - 4)

/*
 * ASCII: "64MB"
 */
#define MEM_SIZE_MAGIC_64M       0x36344d42

/*
 * ASCII: "32MB"
 */
#define MEM_SIZE_MAGIC_32M       0x33324d42

/*
 * Uart index and baudrate to u-boot
 */
#define CONFIG_UART_INDEX_ADDR  (SLEEP_LIB_TCSM + SLEEP_LIB_LENGTH - 8)
#define CONFIG_UART_BAUDRATE_ADDR (SLEEP_LIB_TCSM + SLEEP_LIB_LENGTH - 12)

/*
 * Cache defines
 */
#define CONFIG_SYS_DCACHE_SIZE      16384
#define CONFIG_SYS_ICACHE_SIZE      16384
#define CONFIG_SYS_CACHELINE_SIZE   32

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ROUND(a,b)      (((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND(n,d)      (((n) + ((d)/2)) / (d))
#define DIV_ROUND_UP(n,d)   (((n) + (d) - 1) / (d))
#define roundup(x, y)       ((((x) + ((y) - 1)) / (y)) * (y))

#define ALIGN(x,a)      __ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

/*
 * The ALLOC_CACHE_ALIGN_BUFFER macro is used to allocate a buffer on the
 * stack that meets the minimum architecture alignment requirements for DMA.
 * Such a buffer is useful for DMA operations where flushing and invalidating
 * the cache before and after a read and/or write operation is required for
 * correct operations.
 *
 * When called the macro creates an array on the stack that is sized such
 * that:
 *
 * 1) The beginning of the array can be advanced enough to be aligned.
 *
 * 2) The size of the aligned portion of the array is a multiple of the minimum
 *    architecture alignment required for DMA.
 *
 * 3) The aligned portion contains enough space for the original number of
 *    elements requested.
 *
 * The macro then creates a pointer to the aligned portion of this array and
 * assigns to the pointer the address of the first element in the aligned
 * portion of the array.
 *
 * Calling the macro as:
 *
 *     ALLOC_CACHE_ALIGN_BUFFER(uint32_t, buffer, 1024);
 *
 * Will result in something similar to saying:
 *
 *     uint32_t    buffer[1024];
 *
 * The following differences exist:
 *
 * 1) The resulting buffer is guaranteed to be aligned to the value of
 *    ARCH_DMA_MINALIGN.
 *
 * 2) The buffer variable created by the macro is a pointer to the specified
 *    type, and NOT an array of the specified type.  This can be very important
 *    if you want the address of the buffer, which you probably do, to pass it
 *    to the DMA hardware.  The value of &buffer is different in the two cases.
 *    In the macro case it will be the address of the pointer, not the address
 *    of the space reserved for the buffer.  However, in the second case it
 *    would be the address of the buffer.  So if you are replacing hard coded
 *    stack buffers with this macro you need to make sure you remove the & from
 *    the locations where you are taking the address of the buffer.
 *
 * Note that the size parameter is the number of array elements to allocate,
 * not the number of bytes.
 *
 * This macro can not be used outside of function scope, or for the creation
 * of a function scoped static buffer.  It can not be used to create a cache
 * line aligned global buffer.
 */
#define ARCH_DMA_MINALIGN       CONFIG_SYS_CACHELINE_SIZE
#define PAD_COUNT(s, pad) ((s - 1) / pad + 1)
#define PAD_SIZE(s, pad) (PAD_COUNT(s, pad) * pad)
#define ALLOC_ALIGN_BUFFER_PAD(type, name, size, align, pad)        \
    char __##name[ROUND(PAD_SIZE(size * sizeof(type), pad), align)  \
              + (align - 1)];                   \
                                    \
    type *name = (type *) ALIGN((uintptr_t)__##name, align)
#define ALLOC_ALIGN_BUFFER(type, name, size, align)     \
    ALLOC_ALIGN_BUFFER_PAD(type, name, size, align, 1)
#define ALLOC_CACHE_ALIGN_BUFFER_PAD(type, name, size, pad)     \
    ALLOC_ALIGN_BUFFER_PAD(type, name, size, ARCH_DMA_MINALIGN, pad)
#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size)          \
    ALLOC_ALIGN_BUFFER(type, name, size, ARCH_DMA_MINALIGN)

/*
 * DEFINE_CACHE_ALIGN_BUFFER() is similar to ALLOC_CACHE_ALIGN_BUFFER, but it's
 * purpose is to allow allocating aligned buffers outside of function scope.
 * Usage of this macro shall be avoided or used with extreme care!
 */
#define DEFINE_ALIGN_BUFFER(type, name, size, align)            \
    static char __##name[roundup(size * sizeof(type), align)]   \
            __aligned(align);               \
                                    \
    static type *name = (type *)__##name
#define DEFINE_CACHE_ALIGN_BUFFER(type, name, size)         \
    DEFINE_ALIGN_BUFFER(type, name, size, ARCH_DMA_MINALIGN)

/*
 *  Configure language
 */
#ifdef __LANGUAGE_ASSEMBLY__
#define _ULCAST_
#else
#define _ULCAST_ (unsigned long)
#endif

/* handy sizes */
#define SZ_1K               0x00000400
#define SZ_2K               0x00000800
#define SZ_4K               0x00001000
#define SZ_8K               0x00002000
#define SZ_16K              0x00004000
#define SZ_32K              0x00008000
#define SZ_64K              0x00010000
#define SZ_128K             0x00020000
#define SZ_256K             0x00040000
#define SZ_512K             0x00080000

#define SZ_1M               0x00100000
#define SZ_2M               0x00200000
#define SZ_4M               0x00400000
#define SZ_8M               0x00800000
#define SZ_16M              0x01000000
#define SZ_24M              0x01800000
#define SZ_31M              0x01F00000
#define SZ_32M              0x02000000
#define SZ_64M              0x04000000
#define SZ_128M             0x08000000
#define SZ_256M             0x10000000
#define SZ_512M             0x20000000

#define SZ_1G               0x40000000
#define SZ_2G               0x80000000

/*
 * Status register bits available in all MIPS CPUs.
 */
#define ST0_IM          0x0000ff00
#define  STATUSB_IP0        8
#define  STATUSF_IP0        (_ULCAST_(1) <<  8)
#define  STATUSB_IP1        9
#define  STATUSF_IP1        (_ULCAST_(1) <<  9)
#define  STATUSB_IP2        10
#define  STATUSF_IP2        (_ULCAST_(1) << 10)
#define  STATUSB_IP3        11
#define  STATUSF_IP3        (_ULCAST_(1) << 11)
#define  STATUSB_IP4        12
#define  STATUSF_IP4        (_ULCAST_(1) << 12)
#define  STATUSB_IP5        13
#define  STATUSF_IP5        (_ULCAST_(1) << 13)
#define  STATUSB_IP6        14
#define  STATUSF_IP6        (_ULCAST_(1) << 14)
#define  STATUSB_IP7        15
#define  STATUSF_IP7        (_ULCAST_(1) << 15)
#define  STATUSB_IP8        0
#define  STATUSF_IP8        (_ULCAST_(1) <<  0)
#define  STATUSB_IP9        1
#define  STATUSF_IP9        (_ULCAST_(1) <<  1)
#define  STATUSB_IP10       2
#define  STATUSF_IP10       (_ULCAST_(1) <<  2)
#define  STATUSB_IP11       3
#define  STATUSF_IP11       (_ULCAST_(1) <<  3)
#define  STATUSB_IP12       4
#define  STATUSF_IP12       (_ULCAST_(1) <<  4)
#define  STATUSB_IP13       5
#define  STATUSF_IP13       (_ULCAST_(1) <<  5)
#define  STATUSB_IP14       6
#define  STATUSF_IP14       (_ULCAST_(1) <<  6)
#define  STATUSB_IP15       7
#define  STATUSF_IP15       (_ULCAST_(1) <<  7)
#define ST0_CH          0x00040000
#define ST0_SR          0x00100000
#define ST0_TS          0x00200000
#define ST0_BEV         0x00400000
#define ST0_RE          0x02000000
#define ST0_FR          0x04000000
#define ST0_CU          0xf0000000
#define ST0_CU0         0x10000000
#define ST0_CU1         0x20000000
#define ST0_CU2         0x40000000
#define ST0_CU3         0x80000000
#define ST0_XX          0x80000000  /* MIPS IV naming */

/*
 * Symbolic register names for 32 bit ABI
 */
#define zero    $0  /* wired zero */
#define AT      $1  /* assembler temp  - uppercase because of ".set at" */
#define v0      $2  /* return value */
#define v1      $3
#define a0      $4  /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8  /* caller saved */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16 /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24 /* caller saved */
#define t9      $25
#define jp      $25 /* PIC jump register */
#define k0      $26 /* kernel scratch */
#define k1      $27
#define gp      $28 /* global pointer */
#define sp      $29 /* stack pointer */
#define fp      $30 /* frame pointer */
#define s8      $30 /* same like fp! */
#define ra      $31 /* return address */

/*
 * Coprocessor 0 register names
 */
#define CP0_INDEX           $0
#define CP0_RANDOM          $1
#define CP0_ENTRYLO0        $2
#define CP0_ENTRYLO1        $3
#define CP0_CONF            $3
#define CP0_CONTEXT         $4
#define CP0_PAGEMASK        $5
#define CP0_WIRED           $6
#define CP0_INFO            $7
#define CP0_BADVADDR        $8
#define CP0_COUNT           $9
#define CP0_ENTRYHI         $10
#define CP0_COMPARE         $11
#define CP0_STATUS          $12
#define CP0_CAUSE           $13
#define CP0_EPC             $14
#define CP0_PRID            $15
#define CP0_EBASE           $15
#define CP0_CONFIG          $16
#define CP0_CONFIG1         $16
#define CP0_CONFIG7         $16
#define CP0_LLADDR          $17
#define CP0_WATCHLO         $18
#define CP0_WATCHHI         $19
#define CP0_XCONTEXT        $20
#define CP0_FRAMEMASK       $21
#define CP0_DIAGNOSTIC      $22
#define CP0_DEBUG           $23
#define CP0_DEPC            $24
#define CP0_PERFORMANCE     $25
#define CP0_ECC             $26
#define CP0_CACHEERR        $27
#define CP0_TAGLO           $28
#define CP0_TAGHI           $29
#define CP0_ERROREPC        $30
#define CP0_DESAVE          $31

#define PMON_CSR    $17, 7
#define PMON_HIGH   $17, 4
#define PMON_LC     $17, 5
#define PMON_RC     $17, 6

#define CORE_CTRL       $12, 2
#define CORE_STATUS     $12, 3
#define CORE_REIM       $12, 4
#define CORE_REIM_LOW   $12, 7

/*
 * Bits in the coprocessor 0 config register.
 */
/* Generic bits.  */
#define CONF_CM_CACHABLE_NO_WA      0
#define CONF_CM_CACHABLE_WA     1
#define CONF_CM_UNCACHED        2
#define CONF_CM_CACHABLE_NONCOHERENT    3
#define CONF_CM_CACHABLE_CE     4
#define CONF_CM_CACHABLE_COW        5
#define CONF_CM_CACHABLE_CUW        6
#define CONF_CM_CACHABLE_ACCELERATED    7
#define CONF_CM_CMASK           7
#define CONF_BE         (_ULCAST_(1) << 15)


/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define INDEX_INVALIDATE_I      0x00
#define INDEX_WRITEBACK_INV_D   0x01
#define INDEX_LOAD_TAG_I        0x04
#define INDEX_LOAD_TAG_D        0x05
#define INDEX_STORE_TAG_I       0x08
#define INDEX_STORE_TAG_D       0x09
#define HIT_INVALIDATE_I        0x10
#define HIT_INVALIDATE_D        0x11
#define HIT_WRITEBACK_INV_D     0x15

/*
 * R4000-specific cacheops
 */
#define CREATE_DIRTY_EXCL_D     0x0d
#define FILL                    0x14
#define HIT_WRITEBACK_I         0x18
#define HIT_WRITEBACK_D         0x19

/*
 * R4000SC and R4400SC-specific cacheops
 */
#define INDEX_INVALIDATE_SI     0x02
#define INDEX_WRITEBACK_INV_SD  0x03
#define INDEX_LOAD_TAG_SI       0x06
#define INDEX_LOAD_TAG_SD       0x07
#define INDEX_STORE_TAG_SI      0x0A
#define INDEX_STORE_TAG_SD      0x0B
#define CREATE_DIRTY_EXCL_SD    0x0f
#define HIT_INVALIDATE_SI       0x12
#define HIT_INVALIDATE_SD       0x13
#define HIT_WRITEBACK_INV_SD    0x17
#define HIT_WRITEBACK_SD        0x1b
#define HIT_SET_VIRTUAL_SI      0x1e
#define HIT_SET_VIRTUAL_SD      0x1f


/*
 *  Configure language
 */
#ifdef __LANGUAGE_ASSEMBLY__
#define _ATYPE_
#define _ATYPE32_
#define _ATYPE64_
#define _CONST64_(x)    x
#else
#define _ATYPE_     __PTRDIFF_TYPE__
#define _ATYPE32_   int
#define _ATYPE64_   __s64
#ifdef CONFIG_64BIT
#define _CONST64_(x)    x ## L
#else
#define _CONST64_(x)    x ## LL
#endif
#endif

/*
 *  32-bit MIPS address spaces
 */
#ifdef __LANGUAGE_ASSEMBLY__
#define _ACAST32_
#define _ACAST64_
#else
#define _ACAST32_       (_ATYPE_)(_ATYPE32_)    /* widen if necessary */
#define _ACAST64_       (_ATYPE64_)     /* do _not_ narrow */
#endif

/*
 * Returns the kernel segment base of a given address
 */
#define KSEGX(a)        ((_ACAST32_ (a)) & 0xe0000000)

/*
 * Returns the physical address of a CKSEGx / XKPHYS address
 */
#define CPHYSADDR(a)        ((_ACAST32_(a)) & 0x1fffffff)
#define XPHYSADDR(a)        ((_ACAST64_(a)) &           \
                 _CONST64_(0x000000ffffffffff))


#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)

/*
 * Map an address to a certain kernel segment
 */
#define KSEG0ADDR(a)        (CPHYSADDR(a) | KSEG0)
#define KSEG1ADDR(a)        (CPHYSADDR(a) | KSEG1)
#define KSEG2ADDR(a)        (CPHYSADDR(a) | KSEG2)
#define KSEG3ADDR(a)        (CPHYSADDR(a) | KSEG3)

/*
 * Memory segments (32bit kernel mode addresses)
 * These are the traditional names used in the 32-bit universe.
 */
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CKUSEG          0x00000000
#define CKSEG0          0x80000000
#define CKSEG1          0xa0000000
#define CKSEG2          0xc0000000
#define CKSEG3          0xe0000000



/*
 * Define the module base addresses
 */
/* AHB0 BUS Devices Base */
#define HARB0_BASE      0xb3000000
#define DDRC_BASE       0xb34f0000
#define DDR_PHY_OFFSET  (-0x4e0000 + 0x1000)
#define X2D_BASE        0xb3030000
#define GPU_BASE        0xb3040000
#define LCDC0_BASE      0xb3050000
#define CIM_BASE        0xb3060000
#define COMPRESS_BASE   0xb3070000
#define IPU0_BASE       0xb3080000
#define GPVLC_BASE      0xb3090000
#define IPU1_BASE       0xb30b0000
#define MONITOR_BASE    0xb30f0000

/* AHB1 BUS Devices Base */
#define SCH_BASE    0xb3200000
#define VDMA_BASE   0xb3210000
#define EFE_BASE    0xb3240000
#define MCE_BASE    0xb3250000
#define DBLK_BASE   0xb3270000
#define VMAU_BASE   0xb3280000
#define SDE_BASE    0xb3290000
#define AUX_BASE    0xb32a0000
#define TCSM_BASE   0xb32c0000
#define JPGC_BASE   0xb32e0000
#define SRAM_BASE   0xb32f0000

/* AHB2 BUS Devices Base */
#define HARB2_BASE  0xb3400000
#define NEMC_BASE   0xb3410000
#define PDMA_BASE   0xb3420000
#define SFC_BASE    0xb3440000
#define MSC0_BASE   0xb3450000
#define MSC1_BASE   0xb3460000
#define MSC2_BASE   0xb3470000
#define GPS_BASE    0xb3480000
#define EHCI_BASE   0xb3490000
#define OHCI_BASE   0xb34a0000
#define ETHC_BASE   0xb34b0000
#define BCH_BASE    0xb34d0000
#define TSSI0_BASE  0xb34e0000
#define TSSI1_BASE  0xb34f0000
#define OTG_BASE    0xb3500000
#define EFUSE_BASE  0xb3540000
#define AES_BASE    0xb3430000

#define OST_BASE    0xb2000000
#define HDMI_BASE   0xb0180000

/* APB BUS Devices Base */
#define CPM_BASE    0xb0000000
#define INTC_BASE   0xb0001000
#define TCU_BASE    0xb0002000
#define RTC_BASE    0xb0003000
#define GPIO_BASE   0xb0010000
#define AIC0_BASE   0xb0020000
#define AIC1_BASE   0xb0021000
#define UART0_BASE  0xb0030000
#define UART1_BASE  0xb0031000
#define UART2_BASE  0xb0032000
#define UART3_BASE  0xb0033000
#define UART4_BASE  0xb0034000
#define SSC_BASE    0xb0040000
#define SSI0_BASE   0xb0043000
#define SSI1_BASE   0xb0044000
#define I2C0_BASE   0xb0050000
#define I2C1_BASE   0xb0051000
#define I2C2_BASE   0xb0052000
#define I2C3_BASE   0xb0053000
#define I2C4_BASE   0xb0054000
#define KMC_BASE    0xb0060000
#define DES_BASE    0xb0061000
#define SADC_BASE   0xb0070000
#define PCM0_BASE   0xb0071000
#define OWI_BASE    0xb0072000
#define PCM1_BASE   0xb0074000
#define WDT_BASE    0xb0002000

/* NAND CHIP Base Address*/
#define NEMC_CS1_BASE 0xbb000000
#define NEMC_CS2_BASE 0xba000000
#define NEMC_CS3_BASE 0xb9000000
#define NEMC_CS4_BASE 0xb8000000
#define NEMC_CS5_BASE 0xb7000000
#define NEMC_CS6_BASE 0xb6000000

#define AUX_BASE    0xb32a0000

/*
 * Bits
 */
#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)
#define BIT8  (1<<8)
#define BIT9  (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)

#ifndef  __LANGUAGE_ASSEMBLY__
/*
 * Functions to access the R10000 performance counters.  These are basically
 * mfc0 and mtc0 instructions from and to coprocessor register with a 5-bit
 * performance counter number encoded into bits 1 ... 5 of the instruction.
 * Only performance counters 0 to 1 actually exist, so for a non-R10000 aware
 * disassembler these will look like an access to sel 0 or 1.
 */
#define read_r10k_perf_cntr(counter)                \
({                              \
    unsigned int __res;                 \
    __asm__ __volatile__(                   \
    "mfpc\t%0, %1"                      \
    : "=r" (__res)                      \
    : "i" (counter));                   \
                                \
    __res;                          \
})

#define write_r10k_perf_cntr(counter,val)           \
do {                                \
    __asm__ __volatile__(                   \
    "mtpc\t%0, %1"                      \
    :                           \
    : "r" (val), "i" (counter));                \
} while (0)

#define read_r10k_perf_event(counter)               \
({                              \
    unsigned int __res;                 \
    __asm__ __volatile__(                   \
    "mfps\t%0, %1"                      \
    : "=r" (__res)                      \
    : "i" (counter));                   \
                                \
    __res;                          \
})

#define write_r10k_perf_cntl(counter,val)           \
do {                                \
    __asm__ __volatile__(                   \
    "mtps\t%0, %1"                      \
    :                           \
    : "r" (val), "i" (counter));                \
} while (0)

/*
 * Macros to access the system control coprocessor
 */

#define __read_32bit_c0_register(source, sel)               \
({ int __res;                               \
    if (sel == 0)                           \
        __asm__ __volatile__(                   \
            "mfc0\t%0, " #source "\n\t"         \
            : "=r" (__res));                \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips32\n\t"              \
            "mfc0\t%0, " #source ", " #sel "\n\t"       \
            ".set\tmips0\n\t"               \
            : "=r" (__res));                \
    __res;                              \
})

#define __read_64bit_c0_register(source, sel)               \
({ unsigned long long __res;                        \
    if (sizeof(unsigned long) == 4)                 \
        __res = __read_64bit_c0_split(source, sel);     \
    else if (sel == 0)                      \
        __asm__ __volatile__(                   \
            ".set\tmips3\n\t"               \
            "dmfc0\t%0, " #source "\n\t"            \
            ".set\tmips0"                   \
            : "=r" (__res));                \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dmfc0\t%0, " #source ", " #sel "\n\t"      \
            ".set\tmips0"                   \
            : "=r" (__res));                \
    __res;                              \
})

#define __write_32bit_c0_register(register, sel, value)         \
do {                                    \
    if (sel == 0)                           \
        __asm__ __volatile__(                   \
            "mtc0\t%z0, " #register "\n\t"          \
            : : "Jr" ((unsigned int)(value)));      \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips32\n\t"              \
            "mtc0\t%z0, " #register ", " #sel "\n\t"    \
            ".set\tmips0"                   \
            : : "Jr" ((unsigned int)(value)));      \
} while (0)

#define __write_64bit_c0_register(register, sel, value)         \
do {                                    \
    if (sizeof(unsigned long) == 4)                 \
        __write_64bit_c0_split(register, sel, value);       \
    else if (sel == 0)                      \
        __asm__ __volatile__(                   \
            ".set\tmips3\n\t"               \
            "dmtc0\t%z0, " #register "\n\t"         \
            ".set\tmips0"                   \
            : : "Jr" (value));              \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dmtc0\t%z0, " #register ", " #sel "\n\t"   \
            ".set\tmips0"                   \
            : : "Jr" (value));              \
} while (0)

#define __read_ulong_c0_register(reg, sel)              \
    ((sizeof(unsigned long) == 4) ?                 \
    (unsigned long) __read_32bit_c0_register(reg, sel) :        \
    (unsigned long) __read_64bit_c0_register(reg, sel))

#define __write_ulong_c0_register(reg, sel, val)            \
do {                                    \
    if (sizeof(unsigned long) == 4)                 \
        __write_32bit_c0_register(reg, sel, val);       \
    else                                \
        __write_64bit_c0_register(reg, sel, val);       \
} while (0)

/*
 * On RM7000/RM9000 these are uses to access cop0 set 1 registers
 */
#define __read_32bit_c0_ctrl_register(source)               \
({ int __res;                               \
    __asm__ __volatile__(                       \
        "cfc0\t%0, " #source "\n\t"             \
        : "=r" (__res));                    \
    __res;                              \
})

#define __write_32bit_c0_ctrl_register(register, value)         \
do {                                    \
    __asm__ __volatile__(                       \
        "ctc0\t%z0, " #register "\n\t"              \
        : : "Jr" ((unsigned int)(value)));          \
} while (0)

/*
 * These versions are only needed for systems with more than 38 bits of
 * physical address space running the 32-bit kernel.  That's none atm :-)
 */
#define __read_64bit_c0_split(source, sel)              \
({                                  \
    unsigned long long __val;                   \
    unsigned long __flags;                      \
                                    \
    local_irq_save(__flags);                    \
    if (sel == 0)                           \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dmfc0\t%M0, " #source "\n\t"           \
            "dsll\t%L0, %M0, 32\n\t"            \
            "dsrl\t%M0, %M0, 32\n\t"            \
            "dsrl\t%L0, %L0, 32\n\t"            \
            ".set\tmips0"                   \
            : "=r" (__val));                \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dmfc0\t%M0, " #source ", " #sel "\n\t"     \
            "dsll\t%L0, %M0, 32\n\t"            \
            "dsrl\t%M0, %M0, 32\n\t"            \
            "dsrl\t%L0, %L0, 32\n\t"            \
            ".set\tmips0"                   \
            : "=r" (__val));                \
    local_irq_restore(__flags);                 \
                                    \
    __val;                              \
})

#define __write_64bit_c0_split(source, sel, val)            \
do {                                    \
    unsigned long __flags;                      \
                                    \
    local_irq_save(__flags);                    \
    if (sel == 0)                           \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dsll\t%L0, %L0, 32\n\t"            \
            "dsrl\t%L0, %L0, 32\n\t"            \
            "dsll\t%M0, %M0, 32\n\t"            \
            "or\t%L0, %L0, %M0\n\t"             \
            "dmtc0\t%L0, " #source "\n\t"           \
            ".set\tmips0"                   \
            : : "r" (val));                 \
    else                                \
        __asm__ __volatile__(                   \
            ".set\tmips64\n\t"              \
            "dsll\t%L0, %L0, 32\n\t"            \
            "dsrl\t%L0, %L0, 32\n\t"            \
            "dsll\t%M0, %M0, 32\n\t"            \
            "or\t%L0, %L0, %M0\n\t"             \
            "dmtc0\t%L0, " #source ", " #sel "\n\t"     \
            ".set\tmips0"                   \
            : : "r" (val));                 \
    local_irq_restore(__flags);                 \
} while (0)

#define read_c0_index()     __read_32bit_c0_register($0, 0)
#define write_c0_index(val) __write_32bit_c0_register($0, 0, val)

#define read_c0_entrylo0()  __read_ulong_c0_register($2, 0)
#define write_c0_entrylo0(val)  __write_ulong_c0_register($2, 0, val)

#define read_c0_entrylo1()  __read_ulong_c0_register($3, 0)
#define write_c0_entrylo1(val)  __write_ulong_c0_register($3, 0, val)

#define read_c0_conf()      __read_32bit_c0_register($3, 0)
#define write_c0_conf(val)  __write_32bit_c0_register($3, 0, val)

#define read_c0_context()   __read_ulong_c0_register($4, 0)
#define write_c0_context(val)   __write_ulong_c0_register($4, 0, val)

#define read_c0_userlocal() __read_ulong_c0_register($4, 2)
#define write_c0_userlocal(val) __write_ulong_c0_register($4, 2, val)

#define read_c0_pagemask()  __read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)  __write_32bit_c0_register($5, 0, val)

#define read_c0_wired()     __read_32bit_c0_register($6, 0)
#define write_c0_wired(val) __write_32bit_c0_register($6, 0, val)

#define read_c0_info()      __read_32bit_c0_register($7, 0)

#define read_c0_cache()     __read_32bit_c0_register($7, 0) /* TX39xx */
#define write_c0_cache(val) __write_32bit_c0_register($7, 0, val)

#define read_c0_badvaddr()  __read_ulong_c0_register($8, 0)
#define write_c0_badvaddr(val)  __write_ulong_c0_register($8, 0, val)

#define read_c0_count()     __read_32bit_c0_register($9, 0)
#define write_c0_count(val) __write_32bit_c0_register($9, 0, val)

#define read_c0_count2()    __read_32bit_c0_register($9, 6) /* pnx8550 */
#define write_c0_count2(val)    __write_32bit_c0_register($9, 6, val)

#define read_c0_count3()    __read_32bit_c0_register($9, 7) /* pnx8550 */
#define write_c0_count3(val)    __write_32bit_c0_register($9, 7, val)

#define read_c0_entryhi()   __read_ulong_c0_register($10, 0)
#define write_c0_entryhi(val)   __write_ulong_c0_register($10, 0, val)

#define read_c0_compare()   __read_32bit_c0_register($11, 0)
#define write_c0_compare(val)   __write_32bit_c0_register($11, 0, val)

#define read_c0_compare2()  __read_32bit_c0_register($11, 6) /* pnx8550 */
#define write_c0_compare2(val)  __write_32bit_c0_register($11, 6, val)

#define read_c0_compare3()  __read_32bit_c0_register($11, 7) /* pnx8550 */
#define write_c0_compare3(val)  __write_32bit_c0_register($11, 7, val)

#define read_c0_status()    __read_32bit_c0_register($12, 0)
#ifdef CONFIG_MIPS_MT_SMTC
#define write_c0_status(val)                        \
do {                                    \
    __write_32bit_c0_register($12, 0, val);             \
    __ehb();                            \
} while (0)
#else
/*
 * Legacy non-SMTC code, which may be hazardous
 * but which might not support EHB
 */
#define write_c0_status(val)    __write_32bit_c0_register($12, 0, val)
#endif /* CONFIG_MIPS_MT_SMTC */

#define read_c0_cause()     __read_32bit_c0_register($13, 0)
#define write_c0_cause(val) __write_32bit_c0_register($13, 0, val)

#define read_c0_epc()       __read_ulong_c0_register($14, 0)
#define write_c0_epc(val)   __write_ulong_c0_register($14, 0, val)

#define read_c0_prid()      __read_32bit_c0_register($15, 0)

#define read_c0_config()    __read_32bit_c0_register($16, 0)
#define read_c0_config1()   __read_32bit_c0_register($16, 1)
#define read_c0_config2()   __read_32bit_c0_register($16, 2)
#define read_c0_config3()   __read_32bit_c0_register($16, 3)
#define read_c0_config4()   __read_32bit_c0_register($16, 4)
#define read_c0_config5()   __read_32bit_c0_register($16, 5)
#define read_c0_config6()   __read_32bit_c0_register($16, 6)
#define read_c0_config7()   __read_32bit_c0_register($16, 7)
#define write_c0_config(val)    __write_32bit_c0_register($16, 0, val)
#define write_c0_config1(val)   __write_32bit_c0_register($16, 1, val)
#define write_c0_config2(val)   __write_32bit_c0_register($16, 2, val)
#define write_c0_config3(val)   __write_32bit_c0_register($16, 3, val)
#define write_c0_config4(val)   __write_32bit_c0_register($16, 4, val)
#define write_c0_config5(val)   __write_32bit_c0_register($16, 5, val)
#define write_c0_config6(val)   __write_32bit_c0_register($16, 6, val)
#define write_c0_config7(val)   __write_32bit_c0_register($16, 7, val)

/*
 * The WatchLo register.  There may be upto 8 of them.
 */
#define read_c0_watchlo0()  __read_ulong_c0_register($18, 0)
#define read_c0_watchlo1()  __read_ulong_c0_register($18, 1)
#define read_c0_watchlo2()  __read_ulong_c0_register($18, 2)
#define read_c0_watchlo3()  __read_ulong_c0_register($18, 3)
#define read_c0_watchlo4()  __read_ulong_c0_register($18, 4)
#define read_c0_watchlo5()  __read_ulong_c0_register($18, 5)
#define read_c0_watchlo6()  __read_ulong_c0_register($18, 6)
#define read_c0_watchlo7()  __read_ulong_c0_register($18, 7)
#define write_c0_watchlo0(val)  __write_ulong_c0_register($18, 0, val)
#define write_c0_watchlo1(val)  __write_ulong_c0_register($18, 1, val)
#define write_c0_watchlo2(val)  __write_ulong_c0_register($18, 2, val)
#define write_c0_watchlo3(val)  __write_ulong_c0_register($18, 3, val)
#define write_c0_watchlo4(val)  __write_ulong_c0_register($18, 4, val)
#define write_c0_watchlo5(val)  __write_ulong_c0_register($18, 5, val)
#define write_c0_watchlo6(val)  __write_ulong_c0_register($18, 6, val)
#define write_c0_watchlo7(val)  __write_ulong_c0_register($18, 7, val)

/*
 * The WatchHi register.  There may be upto 8 of them.
 */
#define read_c0_watchhi0()  __read_32bit_c0_register($19, 0)
#define read_c0_watchhi1()  __read_32bit_c0_register($19, 1)
#define read_c0_watchhi2()  __read_32bit_c0_register($19, 2)
#define read_c0_watchhi3()  __read_32bit_c0_register($19, 3)
#define read_c0_watchhi4()  __read_32bit_c0_register($19, 4)
#define read_c0_watchhi5()  __read_32bit_c0_register($19, 5)
#define read_c0_watchhi6()  __read_32bit_c0_register($19, 6)
#define read_c0_watchhi7()  __read_32bit_c0_register($19, 7)

#define write_c0_watchhi0(val)  __write_32bit_c0_register($19, 0, val)
#define write_c0_watchhi1(val)  __write_32bit_c0_register($19, 1, val)
#define write_c0_watchhi2(val)  __write_32bit_c0_register($19, 2, val)
#define write_c0_watchhi3(val)  __write_32bit_c0_register($19, 3, val)
#define write_c0_watchhi4(val)  __write_32bit_c0_register($19, 4, val)
#define write_c0_watchhi5(val)  __write_32bit_c0_register($19, 5, val)
#define write_c0_watchhi6(val)  __write_32bit_c0_register($19, 6, val)
#define write_c0_watchhi7(val)  __write_32bit_c0_register($19, 7, val)

#define read_c0_xcontext()  __read_ulong_c0_register($20, 0)
#define write_c0_xcontext(val)  __write_ulong_c0_register($20, 0, val)

#define read_c0_intcontrol()    __read_32bit_c0_ctrl_register($20)
#define write_c0_intcontrol(val) __write_32bit_c0_ctrl_register($20, val)

#define read_c0_framemask() __read_32bit_c0_register($21, 0)
#define write_c0_framemask(val) __write_32bit_c0_register($21, 0, val)

/* RM9000 PerfControl performance counter control register */
#define read_c0_perfcontrol()   __read_32bit_c0_register($22, 0)
#define write_c0_perfcontrol(val) __write_32bit_c0_register($22, 0, val)

#define read_c0_diag()      __read_32bit_c0_register($22, 0)
#define write_c0_diag(val)  __write_32bit_c0_register($22, 0, val)

#define read_c0_diag1()     __read_32bit_c0_register($22, 1)
#define write_c0_diag1(val) __write_32bit_c0_register($22, 1, val)

#define read_c0_diag2()     __read_32bit_c0_register($22, 2)
#define write_c0_diag2(val) __write_32bit_c0_register($22, 2, val)

#define read_c0_diag3()     __read_32bit_c0_register($22, 3)
#define write_c0_diag3(val) __write_32bit_c0_register($22, 3, val)

#define read_c0_diag4()     __read_32bit_c0_register($22, 4)
#define write_c0_diag4(val) __write_32bit_c0_register($22, 4, val)

#define read_c0_diag5()     __read_32bit_c0_register($22, 5)
#define write_c0_diag5(val) __write_32bit_c0_register($22, 5, val)

#define read_c0_debug()     __read_32bit_c0_register($23, 0)
#define write_c0_debug(val) __write_32bit_c0_register($23, 0, val)

#define read_c0_depc()      __read_ulong_c0_register($24, 0)
#define write_c0_depc(val)  __write_ulong_c0_register($24, 0, val)

/*
 * MIPS32 / MIPS64 performance counters
 */
#define read_c0_perfctrl0() __read_32bit_c0_register($25, 0)
#define write_c0_perfctrl0(val) __write_32bit_c0_register($25, 0, val)
#define read_c0_perfcntr0() __read_32bit_c0_register($25, 1)
#define write_c0_perfcntr0(val) __write_32bit_c0_register($25, 1, val)
#define read_c0_perfctrl1() __read_32bit_c0_register($25, 2)
#define write_c0_perfctrl1(val) __write_32bit_c0_register($25, 2, val)
#define read_c0_perfcntr1() __read_32bit_c0_register($25, 3)
#define write_c0_perfcntr1(val) __write_32bit_c0_register($25, 3, val)
#define read_c0_perfctrl2() __read_32bit_c0_register($25, 4)
#define write_c0_perfctrl2(val) __write_32bit_c0_register($25, 4, val)
#define read_c0_perfcntr2() __read_32bit_c0_register($25, 5)
#define write_c0_perfcntr2(val) __write_32bit_c0_register($25, 5, val)
#define read_c0_perfctrl3() __read_32bit_c0_register($25, 6)
#define write_c0_perfctrl3(val) __write_32bit_c0_register($25, 6, val)
#define read_c0_perfcntr3() __read_32bit_c0_register($25, 7)
#define write_c0_perfcntr3(val) __write_32bit_c0_register($25, 7, val)

/* RM9000 PerfCount performance counter register */
#define read_c0_perfcount() __read_64bit_c0_register($25, 0)
#define write_c0_perfcount(val) __write_64bit_c0_register($25, 0, val)

#define read_c0_ecc()       __read_32bit_c0_register($26, 0)
#define write_c0_ecc(val)   __write_32bit_c0_register($26, 0, val)

#define read_c0_derraddr0() __read_ulong_c0_register($26, 1)
#define write_c0_derraddr0(val) __write_ulong_c0_register($26, 1, val)

#define read_c0_cacheerr()  __read_32bit_c0_register($27, 0)

#define read_c0_derraddr1() __read_ulong_c0_register($27, 1)
#define write_c0_derraddr1(val) __write_ulong_c0_register($27, 1, val)

#define read_c0_taglo()     __read_32bit_c0_register($28, 0)
#define write_c0_taglo(val) __write_32bit_c0_register($28, 0, val)

#define read_c0_dtaglo()    __read_32bit_c0_register($28, 2)
#define write_c0_dtaglo(val)    __write_32bit_c0_register($28, 2, val)

#define read_c0_taghi()     __read_32bit_c0_register($29, 0)
#define write_c0_taghi(val) __write_32bit_c0_register($29, 0, val)

#define read_c0_errorepc()  __read_ulong_c0_register($30, 0)
#define write_c0_errorepc(val)  __write_ulong_c0_register($30, 0, val)

/* MIPSR2 */
#define read_c0_hwrena()    __read_32bit_c0_register($7, 0)
#define write_c0_hwrena(val)    __write_32bit_c0_register($7, 0, val)

#define read_c0_intctl()    __read_32bit_c0_register($12, 1)
#define write_c0_intctl(val)    __write_32bit_c0_register($12, 1, val)

#define read_c0_srsctl()    __read_32bit_c0_register($12, 2)
#define write_c0_srsctl(val)    __write_32bit_c0_register($12, 2, val)

#define read_c0_srsmap()    __read_32bit_c0_register($12, 3)
#define write_c0_srsmap(val)    __write_32bit_c0_register($12, 3, val)

#define read_c0_ebase()     __read_32bit_c0_register($15, 1)
#define write_c0_ebase(val) __write_32bit_c0_register($15, 1, val)

/*
 * Macros to access the floating point coprocessor control registers
 */
#define read_32bit_cp1_register(source)             \
({ int __res;                           \
    __asm__ __volatile__(                   \
    ".set\tpush\n\t"                    \
    ".set\treorder\n\t"                 \
    "cfc1\t%0,"STR(source)"\n\t"                \
    ".set\tpop"                     \
    : "=r" (__res));                    \
    __res;})

#define rddsp(mask)                         \
({                                  \
    unsigned int __res;                     \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push                \n"     \
    "   .set    noat                \n"     \
    "   # rddsp $1, %x1             \n"     \
    "   .word   0x7c000cb8 | (%x1 << 16)    \n"     \
    "   move    %0, $1              \n"     \
    "   .set    pop             \n"     \
    : "=r" (__res)                          \
    : "i" (mask));                          \
    __res;                              \
})

#define wrdsp(val, mask)                        \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # wrdsp $1, %x1                 \n" \
    "   .word   0x7c2004f8 | (%x1 << 11)        \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (val), "i" (mask));                   \
} while (0)

#define mfhi0()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mfhi  %0, $ac0        \n"         \
    "   .word   0x00000810      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mfhi1()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mfhi  %0, $ac1        \n"         \
    "   .word   0x00200810      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mfhi2()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mfhi  %0, $ac2        \n"         \
    "   .word   0x00400810      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mfhi3()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mfhi  %0, $ac3        \n"         \
    "   .word   0x00600810      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mflo0()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mflo  %0, $ac0        \n"         \
    "   .word   0x00000812      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mflo1()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mflo  %0, $ac1        \n"         \
    "   .word   0x00200812      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mflo2()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mflo  %0, $ac2        \n"         \
    "   .word   0x00400812      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mflo3()                             \
({                                  \
    unsigned long __treg;                       \
                                    \
    __asm__ __volatile__(                       \
    "   .set    push            \n"         \
    "   .set    noat            \n"         \
    "   # mflo  %0, $ac3        \n"         \
    "   .word   0x00600812      \n"         \
    "   move    %0, $1          \n"         \
    "   .set    pop         \n"         \
    : "=r" (__treg));                       \
    __treg;                             \
})

#define mthi0(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mthi  $1, $ac0                \n" \
    "   .word   0x00200011              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mthi1(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mthi  $1, $ac1                \n" \
    "   .word   0x00200811              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mthi2(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mthi  $1, $ac2                \n" \
    "   .word   0x00201011              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mthi3(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mthi  $1, $ac3                \n" \
    "   .word   0x00201811              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mtlo0(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mtlo  $1, $ac0                \n" \
    "   .word   0x00200013              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mtlo1(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mtlo  $1, $ac1                \n" \
    "   .word   0x00200813              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mtlo2(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mtlo  $1, $ac2                \n" \
    "   .word   0x00201013              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

#define mtlo3(x)                            \
do {                                    \
    __asm__ __volatile__(                       \
    "   .set    push                    \n" \
    "   .set    noat                    \n" \
    "   move    $1, %0                  \n" \
    "   # mtlo  $1, $ac3                \n" \
    "   .word   0x00201813              \n" \
    "   .set    pop                 \n" \
    :                               \
    : "r" (x));                         \
} while (0)

/*
 * TLB operations.
 *
 * It is responsibility of the caller to take care of any TLB hazards.
 */
static inline void tlb_probe(void)
{
    __asm__ __volatile__(
        ".set noreorder\n\t"
        "tlbp\n\t"
        ".set reorder");
}

static inline void tlb_read(void)
{
    __asm__ __volatile__(
        ".set noreorder\n\t"
        "tlbr\n\t"
        ".set reorder");
}

static inline void tlb_write_indexed(void)
{
    __asm__ __volatile__(
        ".set noreorder\n\t"
        "tlbwi\n\t"
        ".set reorder");
}

static inline void tlb_write_random(void)
{
    __asm__ __volatile__(
        ".set noreorder\n\t"
        "tlbwr\n\t"
        ".set reorder");
}

/*
 * Manipulate bits in a c0 register.
 */
#define __BUILD_SET_C0(name)                    \
static inline unsigned int                  \
set_c0_##name(unsigned int set)                 \
{                               \
    unsigned int res;                   \
                                \
    res = read_c0_##name();                 \
    res |= set;                     \
    write_c0_##name(res);                   \
                                \
    return res;                     \
}                               \
                                \
static inline unsigned int                  \
clear_c0_##name(unsigned int clear)             \
{                               \
    unsigned int res;                   \
                                \
    res = read_c0_##name();                 \
    res &= ~clear;                      \
    write_c0_##name(res);                   \
                                \
    return res;                     \
}                               \
                                \
static inline unsigned int                  \
change_c0_##name(unsigned int change, unsigned int new)     \
{                               \
    unsigned int res;                   \
                                \
    res = read_c0_##name();                 \
    res &= ~change;                     \
    res |= (new & change);                  \
    write_c0_##name(res);                   \
                                \
    return res;                     \
}

__BUILD_SET_C0(status)
__BUILD_SET_C0(cause)
__BUILD_SET_C0(config)
__BUILD_SET_C0(intcontrol)
__BUILD_SET_C0(intctl)
__BUILD_SET_C0(srsmap)

#endif

/*
 * IO
 */
/*
 * Sane hardware offers swapping of I/O space accesses in hardware; less
 * sane hardware forces software to fiddle with this ...
 */
#if defined(CONFIG_SWAP_IO_SPACE) && defined(__MIPSEB__)

#define __ioswab8(x) (x)
#define __ioswab16(x) swab16(x)
#define __ioswab32(x) swab32(x)

#else
#define __ioswab8(x) (x)
#define __ioswab16(x) (x)
#define __ioswab32(x) (x)
#endif

#define __raw_readb(addr) (*(volatile unsigned char *)(addr))
#define __raw_readw(addr) (*(volatile unsigned short *)(addr))
#define __raw_readl(addr) (*(volatile unsigned int *)(addr))
#define readb(addr) __raw_readb((addr))
#define readw(addr) __ioswab16(__raw_readw((addr)))
#define readl(addr) __ioswab32(__raw_readl((addr)))

#define __raw_writeb(b, addr) (*(volatile unsigned char *)(addr)) = (b)
#define __raw_writew(b, addr) (*(volatile unsigned short *)(addr)) = (b)
#define __raw_writel(b, addr) (*(volatile unsigned int *)(addr)) = (b)
#define writeb(b, addr) __raw_writeb((b), (addr))
#define writew(b, addr) __raw_writew(__ioswab16(b), (addr))
#define writel(b, addr) __raw_writel(__ioswab32(b), (addr))

#ifndef __LANGUAGE_ASSEMBLY__
#define ASMMACRO(name, code...)                     \
__asm__(".macro " #name "; " #code "; .endm");              \
                                    \
static inline void name(void)                       \
{                                   \
    __asm__ __volatile__ (#name);                   \
}

ASMMACRO(_ssnop,
     sll    $0, $0, 1
    )

ASMMACRO(_ehb,
     sll    $0, $0, 3
    )

ASMMACRO(mtc0_tlbw_hazard,
    _ssnop; _ssnop; _ehb
    )
ASMMACRO(tlbw_use_hazard,
    _ssnop; _ssnop; _ssnop; _ehb
    )
ASMMACRO(tlb_probe_hazard,
     _ssnop; _ssnop; _ssnop; _ehb
    )
ASMMACRO(irq_enable_hazard,
     _ssnop; _ssnop; _ssnop; _ehb
    )
ASMMACRO(irq_disable_hazard,
    _ssnop; _ssnop; _ssnop; _ehb
    )
ASMMACRO(back_to_back_c0_hazard,
     _ssnop; _ssnop; _ssnop; _ehb
    )

ASMMACRO(enable_fpu_hazard,
     nop; nop; nop; nop
)

ASMMACRO(disable_fpu_hazard,
     _ehb
)

#define __enable_fpu()                          \
do {                                    \
    set_c0_status(ST0_CU1);                     \
    enable_fpu_hazard();                        \
} while (0)

#define __disable_fpu()                         \
do {                                    \
    clear_c0_status(ST0_CU1);                   \
    disable_fpu_hazard();                       \
} while (0)


#define disable_fpu()                           \
do {                                    \
    __disable_fpu();                    \
} while (0)

#endif

/*
 * Bit ops
 */
#ifndef __LANGUAGE_ASSEMBLY__
void set_bit(int nr, volatile void * addr);
void clear_bit(int nr, volatile void * addr);
int test_bit(int nr, const volatile void *addr);
#endif

/*
 * Boot next stage
 */
#ifndef __LANGUAGE_ASSEMBLY__
void boot_next_stage(void);

#define assert(x) if (!(x)) {hang_reason("Assert failed: "#x"\n");}

#endif

/*
 * Console
 */
#ifndef __LANGUAGE_ASSEMBLY__

#ifdef CONFIG_CONSOLE_ENABLE
void __attribute__((long_call)) serial_printf(const char *fmt, ...);
void panic(const char *fmt, ...);
#define puts(s) do{uart_puts(s);uart_puts("\n");}while(0)
#define printf(fmt, args...) serial_printf(fmt, ##args)
#else
#define puts(fmt, args...) do{}while(0)
#define panic(fmt, args...) do{}while(0)
#define printf(fmt, args...) do{}while(0)
#endif

#ifdef XLOADER_DEBUG
#define debug(fmt, args...) serial_printf(fmt, ##args)
#else
#define debug(fmt, args...) do{}while(0)
#endif
#endif

/*
 * Delay
 */
#ifndef __LANGUAGE_ASSEMBLY__
void udelay(uint64_t us);
void mdelay(uint32_t ms);
#endif

#ifndef __LANGUAGE_ASSEMBLY__
__attribute__((noreturn)) void hang(void);
__attribute__((noreturn)) void hang_reason(const char* reason);
#endif

/*
 * Recovery
 */
#ifndef __LANGUAGE_ASSEMBLY__
#define NORMAL_BOOT                     1
#define RECOVERY_BOOT                   2

enum {
    KEY_UNPRESS = 0,
    KEY_PRESS,
};
int get_boot_sel(void);
#endif

/*
 * Cache ops
 */
#ifndef __LANGUAGE_ASSEMBLY__
void local_irq_disable(void);
void local_irq_enable(void);
void flush_dcache_all(void);
void flush_icache_all(void);
void flush_cache_all(void);
#endif


#ifndef __LANGUAGE_ASSEMBLY__
#ifndef __HOST__
static inline uint16_t __get_unaligned_le16(const uint8_t *p) {
    return p[0] | p[1] << 8;
}

static inline uint32_t __get_unaligned_le32(const uint8_t *p) {
    return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

static inline uint64_t __get_unaligned_le64(const uint8_t *p) {
    return (uint64_t)__get_unaligned_le32(p + 4) << 32 |
           __get_unaligned_le32(p);
}

static inline void __put_unaligned_le16(uint16_t val, uint8_t *p) {
    *p++ = val;
    *p++ = val >> 8;
}

static inline void __put_unaligned_le32(uint32_t val, uint8_t *p) {
    __put_unaligned_le16(val >> 16, p + 2);
    __put_unaligned_le16(val, p);
}

static inline void __put_unaligned_le64(uint64_t val, uint8_t *p) {
    __put_unaligned_le32(val >> 32, p + 4);
    __put_unaligned_le32(val, p);
}

static inline uint16_t get_unaligned_le16(const void *p) {
    return __get_unaligned_le16((const uint8_t *)p);
}

static inline uint32_t get_unaligned_le32(const void *p) {
    return __get_unaligned_le32((const uint8_t *)p);
}

static inline uint64_t get_unaligned_le64(const void *p) {
    return __get_unaligned_le64((const uint8_t *)p);
}

static inline void put_unaligned_le16(uint16_t val, void *p) {
    __put_unaligned_le16(val, p);
}

static inline void put_unaligned_le32(uint32_t val, void *p) {
    __put_unaligned_le32(val, p);
}

static inline void put_unaligned_le64(uint32_t val, void *p) {
    __put_unaligned_le64(val, p);
}

uint32_t __div64_32(uint64_t *dividend, uint32_t divisor);

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({              \
    uint32_t __base = (base);           \
    uint32_t __rem;                 \
    (void)(((typeof((n)) *)0) == ((uint64_t *)0));  \
    if (((n) >> 32) == 0) {         \
        __rem = (uint32_t)(n) % __base;     \
        (n) = (uint32_t)(n) / __base;       \
    } else                      \
        __rem = __div64_32(&(n), __base);   \
    __rem;                      \
 })

/* Wrapper for do_div(). Doesn't modify dividend and returns
 * the result, not reminder.
 */
static inline uint64_t lldiv(uint64_t dividend, uint32_t divisor)
{
    uint64_t __res = dividend;
    do_div(__res, divisor);
    return(__res);
}
#endif
#endif

#ifndef __LANGUAGE_ASSEMBLY__
#ifndef __HOST__
void *memcpy(void *dst, const void *src, unsigned int len);
int memcmp(const void * cs,const void * ct, size_t count);
void *memset(void *s, int c, size_t n);
char * strstr(const char * s1,const char * s2);
size_t strlen(const char * s);
#endif
#endif

#ifndef __LANGUAGE_ASSEMBLY__
void ddr_access_test(void);
void dump_mem_content(uint32_t *src, uint32_t len);
#endif

#ifndef __LANGUAGE_ASSEMBLY__
extern int ddr_autosr;
void check_socid(void);
void pass_params_to_uboot(void);
void check_jump_to_usb_boot(void);
void set_jump_to_usb_boot(void);
#endif

#ifndef __LANGUAGE_ASSEMBLY__
void suspend_enter(int state);
int enter_sleep(int state);
void enter_idle(void);

void cache_init(void);

struct sleep_context {
    unsigned int gpr_s[8];
    unsigned int gpr_gp;
    unsigned int gpr_sp;
    unsigned int gpr_fp;
    unsigned int gpr_ra;

    unsigned int cp0_pagemask;
    unsigned int cp0_tlb_spec;
    unsigned int cp0_status;
    unsigned int cp0_intctl;
    unsigned int cp0_cause;
    unsigned int cp0_ebase;
    unsigned int cp0_config;
    unsigned int cp0_config1;
    unsigned int cp0_config2;
    unsigned int cp0_config3;
    unsigned int cp0_config7;
    unsigned int cp0_lladdr;

    unsigned int pmon_csr;
    unsigned int pmon_high;
    unsigned int pmon_lc;
    unsigned int pmon_rc;

    unsigned int cp0_watchlo;
    unsigned int cp0_watchhi;

    unsigned int cp0_ecc;

    unsigned int cpm_lcr;
    unsigned int cpm_opcr;
    unsigned int cpm_clkgr;
    unsigned int cpm_usbpcr;
};

struct sleep_lib_entry {
    void (*restore_context)(void);
    int (*enter_sleep)(int state);
    void (*enter_idle)(void);
};

void sleep_lib_init_clk(void);
void sleep_lib_reset_clk_tree(void);
#endif

#endif /* COMMON_H */
