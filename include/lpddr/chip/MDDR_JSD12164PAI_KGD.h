// Jeju Semi JSD12164PAI - Used in the X1000E

#pragma once

#define DDRC_CFG_VALUE                  0x0a668a40
#define DDRC_CTRL_VALUE                 0x0000c91a
#define DDRC_DLMR_VALUE                 0x00000000
#define DDRC_DDLP_VALUE                 0x00000000
#define DDRC_MMAP0_VALUE                0x000020fc
#define DDRC_MMAP1_VALUE                0x00002400
#define DDRC_REFCNT_VALUE               0x002f0001
#define DDRC_TIMING1_VALUE              0x04070301
#define DDRC_TIMING2_VALUE              0x04080303
#define DDRC_TIMING3_VALUE              0x2003020b
#define DDRC_TIMING4_VALUE              0x07630121
#define DDRC_TIMING5_VALUE              0xff070100
#define DDRC_TIMING6_VALUE              0x06000505
#define DDRC_AUTOSR_EN_VALUE            0x00000000
#define DDRP_DCR_VALUE                  0x00000000
#define DDRP_MR0_VALUE                  0x00000033
#define DDRP_MR1_VALUE                  0x00000000
#define DDRP_MR2_VALUE                  0x00000098
#define DDRP_MR3_VALUE                  0x00000000
#define DDRP_PTR0_VALUE                 0x0021000a
#define DDRP_PTR1_VALUE                 0x06009c40
#define DDRP_PTR2_VALUE                 0x042da072
#define DDRP_DTPR0_VALUE                0x16483352
#define DDRP_DTPR1_VALUE                0x000f0090
#define DDRP_DTPR2_VALUE                0x10010818
#define DDRP_PGCR_VALUE                 0x01042e03
#define DDRP_ODTCR_VALUE                0x00000000
#define DDRP_DX0GCR_VALUE              0x00090881
#define DDRP_DX1GCR_VALUE              0x00090881
#define DDRP_DX2GCR_VALUE              0x00090e80
#define DDRP_DX3GCR_VALUE              0x00090e80
#define DDRP_ZQNCR1_VALUE               0x0000007b
#define DDRP_IMPANDCE_ARRAY             {0x00000000,0x00000000} //0-cal_value 1-req_value
#define DDRP_ODT_IMPANDCE_ARRAY         {0x00000000,0x00000000} //0-cal_value 1-req_value
#define DDRP_RZQ_TABLE  {0x00,0x01,0x02,0x03,0x06,0x07,0x04,0x05,0x0c,0x0d,0x0e,0x0f,0x0a,0x0b,0x08,0x09,0x18,0x19,0x1a,0x1b,0x1e,0x1f,0x1c,0x1d,0x14,0x15,0x16,0x17,0x12,0x13,0x10,0x11}
#define DDR_CHIP_0_SIZE                 67108864
#define DDR_CHIP_1_SIZE                 0
#define REMMAP_ARRAY {\
        0x03020d0c,\
        0x07060504,\
        0x0b0a0908,\
        0x0f0e0100,\
        0x13121110,\
}
