#ifndef _SYS_H_
#define _SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

#if (defined(CHIP_GM6601))
/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Configuration Status Register: 0x10 */
    __IO uint32_t CFGMISC;                   /**< System Miscellaneous Configuration Register: 0x14 */
    __IO uint32_t RSVCFG[2];                 /**< System Configuration Reserved Register: 0x18~0x1c */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[3];                   /**< Reserved System Clock Gating Register: 0x34~0x3c */
    __IO uint32_t SCS;                       /**< System Clock Source Register: 0x40 */
    __IO uint32_t MCS;                       /**< Module Clock Source Register: 0x44 */
    __IO uint32_t RCS[10];                   /**< Reserved Clock Source  Register: 0x48~0x6c */
    __IO uint32_t SPS;                       /**< System Prescaler Register: 0x70 */
    __IO uint32_t RESERVED[3];               /**< System Configuration Reserved Register: 0x74~0x7c */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t RESERVED1[9];              /**< Reserved: 0x9C ~ 0xBC */
    __IO uint32_t ESS;                       /**< ETM Sync Source Register: 0xC0 */
    __IO uint32_t RESERVED2[15];             /**< Reserved: 0xC4 ~ 0xFC */
    __IO uint32_t FLTCPS;                    /**< Pad Filter Clock Prescaler Setting : 0x100 */
    __IO uint32_t FLTCS[4];                  /**< Pad Filter Clock Source Selection: 0x104~0x110 */
    __IO uint32_t FLTBP[2];                  /**< Pad Filter Bypass: 0x114~0x118 */
    __IO uint32_t RESERVED3;                 /**< Reserved: 0x11C */
    __IO uint32_t PINMUX[2];                 /**< Pad Multiplexer Control Register: 0x120~0x124 */
    __IO uint32_t RESERVED4[2];              /**< Reserved: 0x128 ~ 0x12C */
    __IO uint32_t PUR[4];                    /**< GPIO Port Pull-up Register: 0x130~0x13C */
} SYS_Device_T;
#elif (defined(CHIP_GM6601B))
/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Configuration Status Register: 0x10 */
    __IO uint32_t CFGMISC;                   /**< System Miscellaneous Configuration Register: 0x14 */
    __IO uint32_t RSVCFG;                    /**< System Configuration Reserved Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[3];                   /**< Reserved System Clock Gating Register: 0x34~0x3c */
    __IO uint32_t SCS;                       /**< System Clock Source Register: 0x40 */
    __IO uint32_t MCS;                       /**< Module Clock Source Register: 0x44 */
    __IO uint32_t FLTCPS;                    /**< Pad Filter Clock Prescaler Setting : 0x48 */
    __IO uint32_t RCS[3];                    /**< Reserved Clock Source  Register: 0x4c~0x54 */
    __IO uint32_t FLTCS[4];                  /**< Pad Filter Clock Source Selection: 0x58~0x64 */
    __IO uint32_t RESERVED0[2];              /**< System Configuration Reserved Register: 0x68 ~ 0x6c */
    __IO uint32_t SPS;                       /**< System Prescaler Register: 0x70 */
    __IO uint32_t RESERVED[3];               /**< System Configuration Reserved Register: 0x74~0x7c */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9c*/
    __IO uint32_t ANASEL;                    /**< System Analog Function Selection Register 0xA0*/
    __IO uint32_t RESERVED00;                /**< Reserved: 0xA4 */
    __IO uint32_t VD3CR;                     /**< Voltage Detector3 Control Register: 0xA8 */
    __IO uint32_t VD3SR;                     /**< Voltage Detector3 Status Register: 0xAC */
    __IO uint32_t RESERVED1[4];              /**< Reserved: 0xB0 ~ 0xBC */
    __IO uint32_t ESS;                       /**< ETM Sync Source Register: 0xC0 */
    __IO uint32_t RESERVED2[15];             /**< Reserved: 0xC4 ~ 0xFC */
    __IO uint32_t FLTBP[2];                  /**< Pad Filter Bypass: 0x100~0x104 */
    __IO uint32_t RESERVED3[6];              /**< Reserved: 0x108 ~ 0x11C */
    __IO uint32_t PINMUX[3];                 /**< Pad Multiplexer Control Register: 0x120~0x128 */
    __IO uint32_t RESERVED4;                 /**< Reserved: 0x12C */
    __IO uint32_t PUR[4];                    /**< GPIO Port Pull-up Register: 0x130~0x13C */
} SYS_Device_T;
#elif (defined(CHIP_GM6611))
/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Information Value Register: 0x10 */
    __IO uint32_t CFGCTL;                    /**< System Configuration Control Register: 0x14 */
    __IO uint32_t WSC;                       /**< Work Source Configuration Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[2];                   /**< Reserved System Clock Gating Register: 0x34~0x38 */
    __IO uint32_t STPMD;                     /**< Stop Mode Register: 0x3C */
    __IO uint32_t CLK0;                      /**< System Clock Selection Register 0: 0x40 */
    __IO uint32_t CLK1;                      /**< System Clock Selection Register 1: 0x44 */
    __IO uint32_t CLK2;                      /**< System Clock Selection Register 2: 0x48 */
    __IO uint32_t RCS[3];                    /**< Reserved Clock Source  Register: 0x4c~0x54 */
    __IO uint32_t DBCS[5];                   /**< Debounce Clock Selection Register: 0x58~0x68 */
    __IO uint32_t RESERVED0;                 /**< Reserved 0x6C */
    __IO uint32_t LIRCM;                     /**< LIRC Monitor Register: 0x70 */
    __IO uint32_t HIRCM0;                    /**< HIRC Monitor Register 1: 0x74 */
    __IO uint32_t HIRCM1;                    /**< HIRC Monitor Register 2: 0x78 */
    __IO uint32_t RESERVED1;                 /**< Reserved: 0x7C */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9c*/
    __IO uint32_t ANASEL;                    /**< System Analog Function Selection Register 0xA0*/
    __IO uint32_t RESERVED2[7];              /**< Reserved: 0xA4 ~ 0xBC */
    __IO uint32_t DIPS;                      /**< Digital IP System Register: 0xC0 */
    __IO uint32_t RESERVED3[14];             /**< Reserved: 0xC4 ~ 0xF8 */
    __IO uint32_t APBTIMEOUTVAL;             /**< APB interface time-out counter value Register: 0xFC*/
    __IO uint32_t FLTBP[3];                  /**< Pad Filter Bypass: 0x100~0x108 */
    __IO uint32_t RESERVED4;                 /**< Reserved: 0x10C */
    __IO uint32_t PADE[3];                   /**< Pad Input Enable: 0x110 ~ 0x118 */
    __IO uint32_t RESERVED5;                 /**< Reserved: 0x11C */
    __IO uint32_t PINMUX[3];                 /**< Pad Multiplexer Control Register: 0x120~0x128 */
    __IO uint32_t RESERVED6;                 /**< Reserved: 0x12C */
    __IO uint32_t PUR[3];                    /**< GPIO Port Pull-up Register: 0x130~0x138 */
    __IO uint32_t RESERVED7;                 /**< Reserved: 0x13C */
    __IO uint32_t PMERR[3];                  /**< Pin Multiplexer Error Register: 0x140~0x148 */
    __IO uint32_t RESERVED8[3];              /**< Reserved: 0x14C ~ 0x154*/
} SYS_Device_T;



#elif defined(CHIP_F6601C)
/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Configuration Status Register: 0x10 */
    __IO uint32_t CFGMISC;                   /**< System Miscellaneous Configuration Register: 0x14 */
    __IO uint32_t RSVCFG;                    /**< System Configuration Reserved Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[3];                   /**< Reserved System Clock Gating Register: 0x34~0x3c */
    __IO uint32_t SCS;                       /**< System Clock Source Register: 0x40 */
    __IO uint32_t MCS;                       /**< Module Clock Source Register: 0x44 */
    __IO uint32_t FLTCPS;                    /**< Pad Filter Clock Prescaler Setting : 0x48 */
    __IO uint32_t RCS[3];                    /**< Reserved Clock Source  Register: 0x4c~0x54 */
    __IO uint32_t FLTCS[4];                  /**< Pad Filter Clock Source Selection: 0x58~0x64 */
    __IO uint32_t RESERVED0[2];              /**< System Configuration Reserved Register: 0x68 ~ 0x6c */
    __IO uint32_t IRCEN;                     /**< IRC Monitor Enable Register: 0x70 */
    __IO uint32_t HOSCMEN;                   /**< HOSC Monitor Enable Register: 0x74 */
    __IO uint32_t HOSCMTHD;                  /**< HOSC Monitor Enable Register: 0x78 */
    __IO uint32_t STCSLIB;                   /**< HOSC Monitor Enable Register: 0x7C */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9c*/
    __IO uint32_t ANASEL;                    /**< System Analog Function Selection Register 0xA0*/
    __IO uint32_t RESERVED00;                /**< Reserved: 0xA4 */
    __IO uint32_t VD3CR;                     /**< Voltage Detector3 Control Register: 0xA8 */
    __IO uint32_t VD3SR;                     /**< Voltage Detector3 Status Register: 0xAC */
    __IO uint32_t FTDL;                     /**< Voltage Detector3 Status Register: 0xB0 */
    __IO uint32_t RESERVED1[3];              /**< Reserved: 0xB4 ~ 0xBC */
    __IO uint32_t ESS;                       /**< ETM Sync Source Register: 0xC0 */
    __IO uint32_t RESERVED2[7];              /**< Reserved: 0xC4 ~ 0xDC */
    __IO uint32_t DSC[2];                    /**< Driven Strength Control Register: 0xE0 ~0xE4 */
    __IO uint32_t RESERVED3[2];              /**< Reserved: 0xE8 ~ 0xEC */
    __IO uint32_t ODMC[2];                   /**< Open Drain Mode Control: Register: 0xF0 ~0xF4 */
    __IO uint32_t RESERVED4;                 /**< Reserved: 0xF8 */
    __IO uint32_t TOCV;                      /**< Timeout Counter Valuee Register: 0xFC */
    __IO uint32_t FLTBP[2];                  /**< Pad Filter Bypass: 0x100~0x104 */
    __IO uint32_t RESERVED5[2];              /**< Reserved: 0x108 ~ 0x10C */
    __IO uint32_t IEC[2];                    /**< Pad Filter Bypass: 0x110~0x114 */
    __IO uint32_t RESERVED6[2];              /**< Reserved: 0x118 ~ 0x11C */
    __IO uint32_t PINMUX[3];                 /**< PIN Multiplexer Control Register: 0x120~0x128 */
		__IO uint32_t RESERVED7;                 /**< Reserved: 0x12c */
    __IO uint32_t PUR[4];                    /**< GPIO Port Pull-up Register: 0x130~0x13C */
    __IO uint32_t PINSELERR[2];              /**PIN Mux Conflict Error Register: 0x140~0x144 */
} SYS_Device_T;


#elif defined(CHIP_GM6611B)

/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< Strap Trigger Control Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< Strap Configuration Value Register: 0xC */
    __IO uint32_t CFGSTS;                    /**< Strap Information Value Register: 0x10 */
    __IO uint32_t CFGCTL;                    /**< System Configuration Control Register: 0x14 */
    __IO uint32_t WSC;                       /**< Work Source Configuration Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2C */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[2];                   /**< Reserved: 0x34~0x38 */
    __IO uint32_t STPMD;                     /**< Stop Mode Register: 0x3C */
    __IO uint32_t CLK0;                      /**< System Clock Selection Register 0: 0x40 */
    __IO uint32_t CLK1;                      /**< System Clock Selection Register 1: 0x44 */
    __IO uint32_t CLK2;                      /**< System Clock Selection Register 2: 0x48 */
    __IO uint32_t RCS[3];                    /**< Reserved Clock Source Register: 0x4C~0x54 */
    __IO uint32_t DBCS[6];                   /**< Debounce Clock Selection Register: 0x58~0x6C */
    __IO uint32_t LIRCM;                     /**< LIRC Monitor Register: 0x70 */
    __IO uint32_t HIRCME;                    /**< HIRC Monitor Enable Register: 0x74 */
    __IO uint32_t HIRCMTHR;                  /**< HIRC Monitor Threshold Register: 0x78 */
    __IO uint32_t STCALIB;                   /**< Clibration Register: 0x7C */
    __IO uint32_t ACR;                       /**< Analog PMU Control Register: 0x80 */
    __IO uint32_t VDCR;                      /**< Voltage Detector Control Register: 0x84 */
    __IO uint32_t VDSR;                      /**< Voltage Detector Status Register: 0x88 */
    __IO uint32_t RSVREG;                    /**< Reserved: 0x8C */
    __IO uint32_t IRC;                       /**< System IRC Control Register 0x90*/
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9C*/
    __IO uint32_t ANASEL;                    /**< System Analog Function Selection Register 0xA0*/
    __IO uint32_t RESERVED0;                     /**< Reserved: 0xA4 */
    __IO uint32_t LVD2CR;                    /**< LVD2 Control Register: 0xA8 */
    __IO uint32_t LVD2SR;                    /**< LVD2 Status Register: 0xAC */
    __IO uint32_t LVD3CR;                    /**< LVD2 Status Register: 0xB0 */
    __IO uint32_t LVD3SR;                    /**< LVD2 Status Register: 0xB4 */
    __IO uint32_t RESERVED1[2];              /**< Reserved: 0xB8 ~ 0xBC */
    __IO uint32_t DIPS;                      /**< Digital IP System Register: 0xC0 */
    __IO uint32_t RESERVED2[7];              /**< Reserved: 0xC4 ~ 0xDC */
    __IO uint32_t IO_DS_EN[3];               /**< IO DS ENABLE Register: 0xE0 ~ 0xE8 */
    __IO uint32_t RESERVED;                  /**< Reserved: 0xEC */
    __IO uint32_t IO_OD_EN[3];               /**< IO OD ENABLE Register: 0xF0 ~ 0xF8 */
    __IO uint32_t APBTIMEOUTVAL;             /**< APB interface time-out counter value Register: 0xFC*/
    __IO uint32_t FLTBP[3];                  /**< Pad Filter Control Register: 0x100~0x108 */
    __IO uint32_t RESERVED3;                 /**< Reserved: 0x10C */
    __IO uint32_t PADE[3];                   /**< Pad Input Enable: 0x110 ~ 0x118 */
    __IO uint32_t RESERVED4;                 /**< Reserved: 0x11C */
    __IO uint32_t PINMUX[3];                 /**< Pad Multiplexer Control Register: 0x120~0x128 */
    __IO uint32_t RESERVED5;                 /**< Reserved: 0x12C */
    __IO uint32_t PUR[3];                    /**< GPIO Port Pull-up Register: 0x130~0x138 */
    __IO uint32_t RESERVED6;                 /**< Reserved: 0x13C */
    __IO uint32_t PMERR[3];                  /**< Pin Multiplexer Error Register: 0x140~0x148 */
    __IO uint32_t RESERVED8[3];              /**< Reserved: 0x14C ~ 0x154*/
} SYS_Device_T;

#elif (defined(CHIP_GM6621))
/* System Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CID;                       /**< Chip ID Register: 0x0 */
    __IO uint32_t SRS;                       /**< System Reset Status Register: 0x4 */
    __IO uint32_t CFGTRG;                    /**< System Configuration Trigger Register: 0x8 */
    __IO uint32_t CFGVAL;                    /**< System Configuration Value Register: 0xc */
    __IO uint32_t CFGSTS;                    /**< System Configuration Status Register: 0x10 */
    __IO uint32_t CFGMISC;                   /**< System Miscellaneous Configuration Register: 0x14 */
    __IO uint32_t RSVCFG;                    /**< System Configuration Reserved Register: 0x18 */
    __IO uint32_t DBGCTRL;                   /**< Debug Ctrl Register: 0x1C */
    __IO uint32_t RST;                       /**< Reset Control Register: 0x20 */
    __IO uint32_t RSVRST[3];                 /**< Reserved Reset Control Register: 0x24~0x2c */
    __IO uint32_t SCG;                       /**< System Clock Gating Register: 0x30 */
    __IO uint32_t RSCG[3];                   /**< Reserved System Clock Gating Register: 0x34~0x3c */
    __IO uint32_t CLK0;                      /**< Clock Selection Register 0 : 0x40 */
    __IO uint32_t CLK1;                      /**< Clock Selection Register 1 : 0x44 */
    __IO uint32_t CLK2;                      /**< Clock Selection Register 2 : 0x48 */
    __IO uint32_t CLK3;                      /**< Clock Selection Register 3 : 0x4c */
    __IO uint32_t RCS[2];                    /**< Reserved Clock Source Register: 0x50~0x54 */
    __IO uint32_t DBCS[4];                   /**< Debounced Clock Selection Regsiters: 0x58~0x64 */
    __IO uint32_t RESERVED0[3];              /**< System Configuration Reserved Register: 0x68~0x70 */
    __IO uint32_t OSCM;                      /**< OSC Monitor Regsiter: 0x74 */
    __IO uint32_t HOSCM;                     /**< OSC Monitor Regsiter: 0x78 */
    __IO uint32_t RESERVED1[6];              /**< Reserved Register: 0x7C~0x90 */
    __IO uint32_t OSC;                       /**< System OSC Control Register 0x94*/
    __IO uint32_t PLL;                       /**< System PLL Control Register 0x98*/
    __IO uint32_t TSC;                       /**< System TS Control Register 0x9C*/
    __IO uint32_t RESERVED2[8];              /**< Reserved Register: 0xA0~0xBC */
    __IO uint32_t DIPS;                      /**< Digital IP System Register: 0xC0 */
    __IO uint32_t WDTC;                      /**< WDT Clock Setting Lock Register: C4 */
    __IO uint32_t RESERVED3[13];             /**< Reserved Register: 0xC8~0xF8 */
    __IO uint32_t APBTMO;                    /**< APB Timeout Value Register: 0xFC */
    __IO uint32_t FLTBP[2];                  /**< Pad Filter Bypass: 0x100~0x104 */
    __IO uint32_t RESERVED4[2];              /**< Reserved Register: 0x108~0x10C */
    __IO uint32_t PADE[2];                   /**< Pad Input Enable Regiser: 0x110~0x114 */
    __IO uint32_t RESERVED5[2];              /**< Reserved: 0x118 ~ 0x11C */
    __IO uint32_t DS[3];                     /**< Pad Driving Strength Register: 0x120~0x128 */
    __IO uint32_t RESERVED6;                 /**< Reserved: 0x12C */
    __IO uint32_t PUR[4];                    /**< GPIO Port Pull-up Register: 0x130~0x13c */
    __IO uint32_t MODE[4];                   /**< GPIO Port Mode Register: 0x140~0x14c */
    __IO uint32_t AFR[8];                    /**< GPIO Port alternate function Register: 0x150~0x16c */
    __IO uint32_t PODC0;                     /**< IO Pad Open Drain Controller Register 0: 0x170 */
    __IO uint32_t PODC1;                     /**< IO Pad Open Drain Controller Register 1: 0x174 */
} SYS_Device_T;

/* AOP (Always On Power) Control Register Layout Type Definition */
typedef struct
{
    uint32_t RESERVED0;                      /**< Reserved: 0x0 */
    __IO uint32_t SRS;                       /**< AOP Reset Status Register: 0x4 */
    uint32_t RESERVED1[4];                   /**< Reserved: 0x08 ~ 0x14 */
    __IO uint32_t PMC;                       /**< Power Mode Register: 0x18 */
    uint32_t RESERVED2;                      /**< Reserved: 0x1C */
    __IO uint32_t RSTC;                      /**< AOP Reset Control Register: 0x20 */
    uint32_t RESERVED3[3];                   /**< Reserved: 0x24 ~ 0x2C */
    __IO uint32_t CG;                        /**< Clock Gate Register: 0x30 */
    uint32_t RESERVED4[4];                   /**< Reserved: 0x34 ~ 0x40 */
    __IO uint32_t CSR1;                      /**< Clock Selection Register 1: 0x44 */
    __IO uint32_t CSR2;                      /**< Clock Selection Register 2: 0x48 */
    uint32_t RESERVED5[13];                  /**< Reserved : 0x4C~0x7C */
    __IO uint32_t PMU;                       /**< PMU Control Register: 0x80 */
    __IO uint32_t VD1C;                      /**< Voltage Detector 1 Control Register: 0x84 */
    __IO uint32_t VD1S;                      /**< Voltage Detector 1 Status Register: 0x88 */
    uint32_t RESERVED6;                      /**< Reserved : 0x8C */
    __IO uint32_t IRC;                       /**< IRC Control Register: 0x90 */
    uint32_t RESERVED7[3];                   /**< Reserved : 0x94 ~ 0x9C */
    __IO uint32_t LSCCTRL;                   /**< LOSC Control Register: 0xA0 */
    uint32_t RESERVED8;                      /**< Reserved : 0xA4 */
    __IO uint32_t VD2C;                      /**< Voltage Detector 2 Control Register: 0xA8 */
    __IO uint32_t VD2S;                      /**< Voltage Detector 2 Status Register: 0xAC */
    __IO uint32_t VD3C;                      /**< Voltage Detector 2 Control Register: 0xB0 */
    __IO uint32_t VD3S;                      /**< Voltage Detector 2 Status Register: 0xB4*/
    uint32_t RESERVED9[2];                   /**< Reserved: 0xB8 ~ 0xBC */
    __IO uint32_t LRCM;                      /**< LIRC Monitor Register: 0xC0 */
    __IO uint32_t LRCMS;                     /**< LIRC Monitor Status Register: 0xC4*/
    __IO uint32_t LSCM;                      /**< LOSC Monitor Register: 0xC8 */
    uint32_t RESERVED10[41];                 /**< Reserved : 0xCC ~ 0x16C */
    __IO uint32_t BAK[6];                    /**< Backup Register : 0x170 ~ 0x18C */
    __IO uint32_t PADRSTE;                   /**< Pad Reset Enable Register: 0x190 */
    __IO uint32_t RCD[4];                    /**< CPU Access Address Record Register: 0x1A0 ~ 0x1AC */
    __IO uint32_t WDTRCD;                    /**< WDT Record Register: 0x1B0 */
    __IO uint32_t PMDBG;                     /**< Power Mode FSM Debug Register: 0x1B4 */
} AOP_Device_T;

#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
#define  SYS_SRS_LVD                         ((uint32_t)0x00000001)            /* System Reset by Low Voltage Detect(LVD) Circuit or Power On Reset(POR) Circuit  */
#define  SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */
#define  SYS_SRS_LOCKUP                      ((uint32_t)0x00000004)            /* System Reset by Cortex-M Lockup Event */
#define  SYS_SRS_SW                          ((uint32_t)0x00000008)            /* System Reset by Software Request */
#if defined(CHIP_GM6601B)
#define  SYS_SRS_HOSC_LSRSTPD                ((uint32_t)0x00000010)            /* System Reset by HOSC Loss */
#define  SYS_SRS_HOSC_LSPD                   ((uint32_t)0x00000080)            /* HOSC Loss Pending */
#endif

#define  SYS_CFGVAL_MODE                     ((uint32_t)0x00000001)            /* Selection of Function Mode */
#define  SYS_CFGVAL_SUBMODE                  ((uint32_t)0x0000000E)            /* Selection of Test SubMode */
#define  SYS_CFGVAL_SUBMODE_SHIFT            (1)
#define  SYS_CFGVAL_SUBMODE_FIELD(x)         (((uint32_t)(((uint32_t)(x))<<SYS_CFGVAL_SUBMODE_SHIFT))&SYS_CFGVAL_SUBMODE)
#define  SYS_CFGVAL_PLL_CS                   ((uint32_t)0x00000010)            /* PLL Clock Source, 0: Internal RC OSC 1: External Crystal OSC */

#define  SYS_CFGTRG_MODE                     ((uint32_t)0x00000001)            /* System Function Mode Trigger */
#define  SYS_CFGTRG_SUBMODE                  ((uint32_t)0x00000002)            /* System Test SubMode Trigger */
#define  SYS_CFGTRG_PLL_CS                   ((uint32_t)0x00000004)            /* PLL Clock Source Trigger */

#define  SYS_CFGMISC_BOOTSEQ_MASK            ((uint32_t)0x00000003)
#define  SYS_CFGMISC_BOOTSEQ_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_CFGMISC_BOOTSEQ_MASK))
#define  SYS_CFGMISC_BOOTSEQ(reg)            ((reg)&SYS_CFGMISC_BOOTSEQ_MASK)
#define  SYS_CFGMISC_PMU_EN                  ((uint32_t)0x00000008)
#define  SYS_CFGMISC_LOCKUP_RESET            ((uint32_t)0x00000010)
//#define  SYS_CFGMISC_PLL_BYPASS            ((uint32_t)0x00000100)            /* Bypass PLL */

#define  SYS_SCG_ACMP                        ((uint32_t)(1 << 13))              /* ACMP peripheral clock gate */
#define  SYS_SCG_ADC                         ((uint32_t)(1 << 12))              /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)(1 << 11))              /* ETM peripheral clock gate */
#define  SYS_SCG_DMA                         ((uint32_t)(1 << 10))              /* DMA peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)(1 << 9))               /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)(1 << 8))               /* CRC peripheral clock gate */
#define  SYS_SCG_HWIR                        ((uint32_t)(1 << 7))               /* HWIR peripheral clock gate */
#define  SYS_SCG_GPIO                        ((uint32_t)(1 << 6))               /* GPIO peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)(1 << 5))               /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)(1 << 4))               /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)(1 << 3))               /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C2                        ((uint32_t)(1 << 2))               /* I2C2 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)(1 << 1))               /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)(1 << 0))               /* I2C0 peripheral clock gate */

#define  SYS_MCS_WDT_SWITCH                  ((uint32_t)0x00040000)            /* Watchdog multiplex switch */
#define  SYS_MCS_RTC2_SWITCH                 ((uint32_t)0x00004000)            /* RTC2 multiplex switch */
#define  SYS_MCS_RTC1_SWITCH                 ((uint32_t)0x00000400)            /* RTC1 multiplex switch */
#define  SYS_MCS_RTC0_SWITCH                 ((uint32_t)0x00000040)            /* RTC0 multiplex switch */

#define  SYS_SCS_CS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SCS_CS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SCS_CS_MASK))
#define  SYS_SCS_CS(reg)                     ((reg)&SYS_SCS_CS_MASK)
#define  SYS_SCS_PLL_RC                      0                                 /* System Clock Source is PLL Referrence Clock (Internal RC Oscillator or External High Frequency Oscillator */
#define  SYS_SCS_PLL_6X                      1                                 /* System Clock Source is PLL output with Multiplication Factor 6 */
#define  SYS_SCS_PLL_3X                      2                                 /* System Clock Source is PLL output with Multiplication Factor 3 */
#define  SYS_SCS_LF_OSC                      3                                 /* System Clock Source is Low Frequency Oscillator cowork with external crystal or resonator */
#define  SYS_SCS_PLL_BYPASS                  ((uint32_t)0x00000100)            /* Bypass PLL */

#if defined(CHIP_GM6601)
#define  SYS_SCS_CORE_SWITCH                 ((uint32_t)0x00000004)            /* FCLK multiplex switch */
#endif

#define  SYS_SCS_PLLCLK_REF                  ((uint32_t)0x00000010)            /**< PLL reference clock */

#define  SYS_SCS_SWT_EN                      ((uint32_t)0x00000004)            /* Clock Switch Module Enabled */

#define  SYS_MCS_UART0_CLKSRC                ((uint32_t)0x00000001)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART1_CLKSRC                ((uint32_t)0x00000002)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART2_CLKSRC                ((uint32_t)0x00000004)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */

#define  SYS_SPS_PS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SPS_PS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SPS_PS_MASK))
#define  SYS_SPS_PS(reg)                     ((reg)&SYS_SPS_PS_MASK)
#define  SYS_SPS_1                           0                                 /* System Clock Prescaler Factor is 1 */
#define  SYS_SPS_2                           1                                 /* System Clock Prescaler Factor is 2 */
#define  SYS_SPS_4                           2                                 /* System Clock Prescaler Factor is 4 */
#define  SYS_SPS_8                           3                                 /* System Clock Prescaler Factor is 8 */
#define  SYS_SPS_16                          4                                 /* System Clock Prescaler Factor is 16 */
#define  SYS_SPS_32                          5                                 /* System Clock Prescaler Factor is 32 */
#define  SYS_SPS_64                          6                                 /* System Clock Prescaler Factor is 64 */
#define  SYS_SPS_128                         7                                 /* System Clock Prescaler Factor is 128 */

#define  SYS_IRC_CRY8M                       ((uint32_t)0x00000001)            /* IRC 8MHz power */

#define  SYS_OSC_CRY8M                       ((uint32_t)0x00000001)            /* OSC 8MHz power */
#define  SYS_OSC_CRY32K                      ((uint32_t)0x00000100)            /* OSC 32KHz power  */

#define  SYS_ANASEL_HOSCIN                   ((uint32_t)0x00000001)            /* OSC 8MHz selection in */
#define  SYS_ANASEL_HOSCOUT                  ((uint32_t)0x00000002)            /* OSC 8MHz selection out */
#define  SYS_ANASEL_LOSCIN                   ((uint32_t)0x00000004)            /* OSC 32KHz selection in */
#define  SYS_ANASEL_LOSCOUT                  ((uint32_t)0x00000008)            /* OSC 32KHz selection out */

#define  SYS_PLL_PDB                         ((uint32_t)0x00000001)            /* PLL power down  */

#if defined(CHIP_GM6601B)
#define  SYS_PLL_PLLCLK_REF                  ((uint32_t)0x00000002)            /**< PLL reference clock */
#endif
#endif


#if defined(CHIP_F6601C)
#define SYS_RTC0_WRITE_DONE_FLAG            ((uint32_t)(1 << 17))               /* 0: write RTC0 is ready , 1: write RTC0 is busy */
#define SYS_RTC1_WRITE_DONE_FLAG            ((uint32_t)(1 << 18))               /* 0: write RTC1 is ready , 1: write RTC1 is busy */
#define SYS_RTC2_WRITE_DONE_FLAG            ((uint32_t)(1 << 19))               /* 0: write RTC2 is ready , 1: write RTC2 is busy */

#define  SYS_SRS_LVD                         ((uint32_t)0x00000001)            /* System Reset by Low Voltage Detect(LVD) Circuit or Power On Reset(POR) Circuit  */
#define  SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */
#define  SYS_SRS_LOCKUP                      ((uint32_t)0x00000004)            /* System Reset by Cortex-M Lockup Event */
#define  SYS_SRS_SW                          ((uint32_t)0x00000008)            /* System Reset by Software Request */
#define  SYS_SRS_HOSC_LSRSTPD                ((uint32_t)0x00000010)            /* System Reset by HOSC Loss */
#define  SYS_SRS_HOSC_LSPD                   ((uint32_t)0x00000080)            /* HOSC Loss Pending */

#define  SYS_CFGVAL_MODE                     ((uint32_t)0x00000001)            /* Selection of Function Mode */
#define  SYS_CFGVAL_SUBMODE                  ((uint32_t)0x0000000E)            /* Selection of Test SubMode */
#define  SYS_CFGVAL_SUBMODE_SHIFT            (1)
#define  SYS_CFGVAL_SUBMODE_FIELD(x)         (((uint32_t)(((uint32_t)(x))<<SYS_CFGVAL_SUBMODE_SHIFT))&SYS_CFGVAL_SUBMODE)
#define  SYS_CFGVAL_PLL_CS                   ((uint32_t)0x00000010)            /* PLL Clock Source, 0: Internal RC OSC 1: External Crystal OSC */

#define  SYS_CFGTRG_MODE                     ((uint32_t)0x00000001)            /* System Function Mode Trigger */
#define  SYS_CFGTRG_SUBMODE                  ((uint32_t)0x00000002)            /* System Test SubMode Trigger */
#define  SYS_CFGTRG_PLL_CS                   ((uint32_t)0x00000004)            /* PLL Clock Source Trigger */

#define  SYS_CFGMISC_BOOTSEQ_MASK            ((uint32_t)0x00000003)
#define  SYS_CFGMISC_BOOTSEQ_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_CFGMISC_BOOTSEQ_MASK))
#define  SYS_CFGMISC_BOOTSEQ(reg)            ((reg)&SYS_CFGMISC_BOOTSEQ_MASK)
#define  SYS_CFGMISC_PMU_EN                  ((uint32_t)0x00000008)
#define  SYS_CFGMISC_LOCKUP_RESET            ((uint32_t)0x00000010)
//#define  SYS_CFGMISC_PLL_BYPASS            ((uint32_t)0x00000100)            /* Bypass PLL */
#define  SYS_RSV_WDOG_WRITEDONE              ((uint32_t)0x00010000)            /* WDOG wirte done flag*/

#define  SYS_SCG_ACMP                        ((uint32_t)(1 << 13))              /* ACMP peripheral clock gate */
#define  SYS_SCG_ADC                         ((uint32_t)(1 << 12))              /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)(1 << 11))              /* ETM peripheral clock gate */
#define  SYS_SCG_DMA                         ((uint32_t)(1 << 10))              /* DMA peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)(1 << 9))               /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)(1 << 8))               /* CRC peripheral clock gate */
#define  SYS_SCG_HWIR                        ((uint32_t)(1 << 7))               /* HWIR peripheral clock gate */
#define  SYS_SCG_GPIO                        ((uint32_t)(1 << 6))               /* GPIO peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)(1 << 5))               /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)(1 << 4))               /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)(1 << 3))               /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C2                        ((uint32_t)(1 << 2))               /* I2C2 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)(1 << 1))               /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)(1 << 0))               /* I2C0 peripheral clock gate */

#define  SYS_MCS_WDT_SWITCH                  ((uint32_t)0x00040000)            /* Watchdog multiplex switch */
#define  SYS_MCS_RTC2_SWITCH                 ((uint32_t)0x00004000)            /* RTC2 multiplex switch */
#define  SYS_MCS_RTC1_SWITCH                 ((uint32_t)0x00000400)            /* RTC1 multiplex switch */
#define  SYS_MCS_RTC0_SWITCH                 ((uint32_t)0x00000040)            /* RTC0 multiplex switch */

#define  SYS_SCS_CS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SCS_CS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SCS_CS_MASK))
#define  SYS_SCS_CS(reg)                     ((reg)&SYS_SCS_CS_MASK)
#define  SYS_SCS_PLL_RC                      0                                 /* System Clock Source is PLL Referrence Clock (Internal RC Oscillator or External High Frequency Oscillator */
#define  SYS_SCS_PLL_6X                      1                                 /* System Clock Source is PLL output with Multiplication Factor 6 */
#define  SYS_SCS_PLL_3X                      2                                 /* System Clock Source is PLL output with Multiplication Factor 3 */
#define  SYS_SCS_LF_OSC                      3                                 /* System Clock Source is Low Frequency Oscillator cowork with external crystal or resonator */
#define  SYS_SCS_PLL_BYPASS                  ((uint32_t)0x00000100)            /* Bypass PLL */

#define  SYS_SCS_PLLCLK_REF                  ((uint32_t)0x00000010)            /**< PLL reference clock */

#define  SYS_SCS_SWT_EN                      ((uint32_t)0x00000004)            /* Clock Switch Module Enabled */

#define  SYS_MCS_UART0_CLKSRC                ((uint32_t)0x00000001)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART1_CLKSRC                ((uint32_t)0x00000002)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART2_CLKSRC                ((uint32_t)0x00000004)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */

#define  SYS_SPS_PS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SPS_PS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SPS_PS_MASK))
#define  SYS_SPS_PS(reg)                     ((reg)&SYS_SPS_PS_MASK)
#define  SYS_SPS_1                           0                                 /* System Clock Prescaler Factor is 1 */
#define  SYS_SPS_2                           1                                 /* System Clock Prescaler Factor is 2 */
#define  SYS_SPS_4                           2                                 /* System Clock Prescaler Factor is 4 */
#define  SYS_SPS_8                           3                                 /* System Clock Prescaler Factor is 8 */
#define  SYS_SPS_16                          4                                 /* System Clock Prescaler Factor is 16 */
#define  SYS_SPS_32                          5                                 /* System Clock Prescaler Factor is 32 */
#define  SYS_SPS_64                          6                                 /* System Clock Prescaler Factor is 64 */
#define  SYS_SPS_128                         7                                 /* System Clock Prescaler Factor is 128 */

#define  SYS_IRC_CRY8M                       ((uint32_t)0x00000001)            /* IRC 8MHz power */

#define  SYS_OSC_CRY8M                       ((uint32_t)0x00000001)            /* OSC 8MHz power */
#define  SYS_OSC_CRY32K                      ((uint32_t)0x00000100)            /* OSC 32KHz power  */

#define  SYS_ANASEL_HOSCIN                   ((uint32_t)0x00000001)            /* OSC 8MHz selection in */
#define  SYS_ANASEL_HOSCOUT                  ((uint32_t)0x00000002)            /* OSC 8MHz selection out */
#define  SYS_ANASEL_LOSCIN                   ((uint32_t)0x00000004)            /* OSC 32KHz selection in */
#define  SYS_ANASEL_LOSCOUT                  ((uint32_t)0x00000008)            /* OSC 32KHz selection out */


#define  SYS_PLL_PDB                         ((uint32_t)0x00000001)            /* PLL power down  */

#define  SYS_PLL_PLLCLK_REF                  ((uint32_t)0x00000002)            /**< PLL reference clock */

/* IO_DS_EN0 */
#define SYS_XIN_DS_EN                        ((uint32_t)0x00000001)            /* XIN driven strength enable */
#define SYS_XOUT_DS_EN                       ((uint32_t)0x00000002)            /* XOUT driven strength enable */
#define SYS_XTXD2_DS_EN                      ((uint32_t)0x00000004)            /* XTXD2 driven strength enable */
#define SYS_XRXD2_DS_EN                      ((uint32_t)0x00000008)            /* XRXD2 driven strength enable */
#define SYS_XTIN_DS_EN                       ((uint32_t)0x00000020)            /* XTIN driven strength enable */
#define SYS_XTOIUT_DS_EN                     ((uint32_t)0x00000040)            /* XTOUT driven strength enable */
#define SYS_XGPIO_7_DS_EN                    ((uint32_t)0x00000080)            /* XGPIO_7 driven strength enable */
#define SYS_XNMI_DS_EN                       ((uint32_t)0x00000100)            /* XNMI driven strength enable */
#define SYS_XGPIO_9_DS_EN                    ((uint32_t)0x00000200)            /* XGPIO_9 driven strength enable */
#define SYS_XSDA1_DS_EN                      ((uint32_t)0x00000400)            /* XSDA1 driven strength enable */
#define SYS_XSCL1_DS_EN                      ((uint32_t)0x00000800)            /* XSCL1 driven strength enable */
#define SYS_XSWD_CLK_DS_EN                   ((uint32_t)0x00001000)            /* XSWD_CLK driven strength enable */
#define SYS_XSWD_DIO_DS_EN                   ((uint32_t)0x00002000)            /* XSWD_DIO driven strength enable */
#define SYS_XSPI_CLK_DS_EN                   ((uint32_t)0x00004000)            /* XSPI_CLK driven strength enable */
#define SYS_XSDA0_CLK_DS_EN                  ((uint32_t)0x00008000)            /* XSDA0 driven strength enable */
#define SYS_XSCL0_DS_EN                      ((uint32_t)0x00010000)            /* XSCL0 driven strength enable */
#define SYS_XETM1_EXT_CLK_DS_EN              ((uint32_t)0x00020000)            /* XETM1 ext clk driven strength enable */
#define SYS_XETM1_TC_IN_DS_EN                ((uint32_t)0x00040000)            /* XETM1 Tc In driven strength enable */
#define SYS_XETM1_CH1_IN_DS_EN               ((uint32_t)0x00080000)            /* XETM1 Ch1 driven strength enable */
#define SYS_XETM1_CH2_IN_DS_EN               ((uint32_t)0x00100000)            /* XETM1 Ch2 driven strength enable */
#define SYS_XAIN12_DS_EN                     ((uint32_t)0x00200000)            /* XAIN12 driven strength enable */
#define SYS_XAIN13_DS_EN                     ((uint32_t)0x00400000)            /* XAIN13 driven strength enable */
#define SYS_XAIN14_DS_EN                     ((uint32_t)0x00800000)            /* XAIN14 driven strength enable */
#define SYS_XAIN15_DS_EN                     ((uint32_t)0x01000000)            /* XAIN15 driven strength enable */
#define SYS_XAIN0_DS_EN                      ((uint32_t)0x02000000)            /* XAIN0 driven strength enable */
#define SYS_XAIN1_DS_EN                      ((uint32_t)0x04000000)            /* XAIN1 driven strength enable */
#define SYS_XAIN2_DS_EN                      ((uint32_t)0x08000000)            /* XAIN2 driven strength enable */
#define SYS_XAIN3_DS_EN                      ((uint32_t)0x10000000)            /* XAIN3 driven strength enable */
#define SYS_XAIN4_DS_EN                      ((uint32_t)0x20000000)            /* XAIN4 driven strength enable */
#define SYS_XAIN5_DS_EN                      ((uint32_t)0x40000000)            /* XAIN5 driven strength enable */
#define SYS_XAIN6_DS_EN                      ((uint32_t)0x80000000)            /* XAIN6 driven strength enable */

/* IO_DS_EN1 */
#define SYS_XAIN7_DS_EN                      ((uint32_t)0x00000001)            /* XAIN7 driven strength enable */
#define SYS_XGPIO_33_DS_EN                   ((uint32_t)0x00000002)            /* XGPIO_33 driven strength enable */
#define SYS_XGPIO_34_DS_EN                   ((uint32_t)0x00000004)            /* XGPIO_34 driven strength enable */
#define SYS_XGPIO_35_DS_EN                   ((uint32_t)0x00000008)            /* XGPIO_35 driven strength enable */
#define SYS_XGPIO_36_DS_EN                   ((uint32_t)0x00000010)            /* XGPIO_36 driven strength enable */
#define SYS_XETM0_EXT_CLK_DS_EN              ((uint32_t)0x00000020)            /* XETM0_EXT_CLK driven strength enable */
#define SYS_XETM0_TC_IN_DS_EN                ((uint32_t)0x00000040)            /* XETM0_TC_IN driven strength enable */
#define SYS_XETM0_CH1_DS_EN                  ((uint32_t)0x00000080)            /* XETM0_CH1 driven strength enable */
#define SYS_XETM0_CH2_DS_EN                  ((uint32_t)0x00000100)            /* XETM0_CH2 driven strength enable */
#define SYS_XTXD1_DS_EN                      ((uint32_t)0x00000200)            /* XTXD1 driven strength enable */
#define SYS_XRXD1_DS_EN                      ((uint32_t)0x00000400)            /* XRXD1 driven strength enable */
#define SYS_XPWM8_1_DS_EN                    ((uint32_t)0x00000800)            /* XPWM8_1 driven strength enable */
#define SYS_XPWM8_2_DS_EN                    ((uint32_t)0x00001000)            /* XPWM8_2 driven strength enable */
#define SYS_XETM2_EXT_CLK_DS_EN              ((uint32_t)0x00002000)            /* XETM2_EXT_CLK driven strength enable */
#define SYS_XETM2_TC_IN_DS_EN                ((uint32_t)0x00004000)            /* XETM2_TC_IN driven strength enable */
#define SYS_XETM2_CH1_DS_EN                  ((uint32_t)0x00008000)            /* XETM2_CH1 driven strength enable */
#define SYS_XETM2_CH2_DS_EN                  ((uint32_t)0x00010000)            /* XETM2_CH2 driven strength enable */
#define SYS_XACMP_0_DS_EN                    ((uint32_t)0x00020000)            /* XACMP_0 driven strength enable */
#define SYS_XACMP_1_DS_EN                    ((uint32_t)0x00040000)            /* XACMP_1 driven strength enable */
#define SYS_XACMP_2_DS_EN                    ((uint32_t)0x00080000)            /* XACMP_2 driven strength enable */
#define SYS_XSDA2_DS_EN                      ((uint32_t)0x00100000)            /* XSDA2 driven strength enable */
#define SYS_XSCL2_DS_EN                      ((uint32_t)0x00200000)            /* XSCL2 driven strength enable */
#define SYS_XTXD0_DS_EN                      ((uint32_t)0x00400000)            /* XTXD0 driven strength enable */
#define SYS_XRXD0_DS_EN                      ((uint32_t)0x00800000)            /* XRXD0 driven strength enable */
#define SYS_XGPIO_56_DS_EN                   ((uint32_t)0x01000000)            /* XGPIO_56 driven strength enable */
#define SYS_XGPIO_57_DS_EN                   ((uint32_t)0x02000000)            /* XGPIO_57 driven strength enable */

/* IO_OD_EN0 */
#define SYS_XIN_ODMC_EN                      ((uint32_t)0x00000001)            /* XIN IO open drain enable */
#define SYS_XOUT_ODMC_EN                     ((uint32_t)0x00000002)            /* XOUT IO open drain enable */
#define SYS_XTXD2_ODMC_EN                    ((uint32_t)0x00000004)            /* XTXD2 IO open drain enable */
#define SYS_XRXD2_ODMC_EN                    ((uint32_t)0x00000008)            /* XRXD2 IO open drain enable */
#define SYS_XTIN_ODMC_EN                     ((uint32_t)0x00000020)            /* XTIN IO open drain enable */
#define SYS_XTOIUT_ODMC_EN                   ((uint32_t)0x00000040)            /* XTOUT IO open drain enable */
#define SYS_XGPIO_7_ODMC_EN                  ((uint32_t)0x00000080)            /* XGPIO_7 IO open drain enable */
#define SYS_XNMI_ODMC_EN                     ((uint32_t)0x00000100)            /* XNMI IO open drain enable */
#define SYS_XGPIO_9_ODMC_EN                  ((uint32_t)0x00000200)            /* XGPIO_9 IO open drain enable */
#define SYS_XSDA1_ODMC_EN                    ((uint32_t)0x00000400)            /* XSDA1 IO open drain enable */
#define SYS_XSCL1_ODMC_EN                    ((uint32_t)0x00000800)            /* XSCL1 IO open drain enable */
#define SYS_XSWD_CLK_ODMC_EN                 ((uint32_t)0x00001000)            /* XSWD_CLK IO open drain enable */
#define SYS_XSWD_DIO_ODMC_EN                 ((uint32_t)0x00002000)            /* XSWD_DIO IO open drain enable */
#define SYS_XSPI_CLK_ODMC_EN                 ((uint32_t)0x00004000)            /* XSPI_CLK IO open drain enable */
#define SYS_XSDA0_CLK_ODMC_EN                ((uint32_t)0x00008000)            /* XSDA0 IO open drain enable */
#define SYS_XSCL0_ODMC_EN                    ((uint32_t)0x00010000)            /* XSCL0 IO open drain enable */
#define SYS_XETM1_EXT_CLK_ODMC_EN            ((uint32_t)0x00020000)            /* XETM1 ext clk IO open drain enable */
#define SYS_XETM1_TC_IN_ODMC_EN              ((uint32_t)0x00040000)            /* XETM1 Tc In IO open drain enable */
#define SYS_XETM1_CH1_IN_ODMC_EN             ((uint32_t)0x00080000)            /* XETM1 Ch1 IO open drain enable */
#define SYS_XETM1_CH2_IN_ODMC_EN             ((uint32_t)0x00100000)            /* XETM1 Ch2 IO open drain enable */
#define SYS_XAIN12_ODMC_EN                   ((uint32_t)0x00200000)            /* XAIN12 IO open drain enable */
#define SYS_XAIN13_ODMC_EN                   ((uint32_t)0x00400000)            /* XAIN13 IO open drain enable */
#define SYS_XAIN14_ODMC_EN                   ((uint32_t)0x00800000)            /* XAIN14 IO open drain enable */
#define SYS_XAIN15_ODMC_EN                   ((uint32_t)0x01000000)            /* XAIN15 IO open drain enable */
#define SYS_XAIN0_ODMC_EN                    ((uint32_t)0x02000000)            /* XAIN0 IO open drain enable */
#define SYS_XAIN1_ODMC_EN                    ((uint32_t)0x04000000)            /* XAIN1 IO open drain enable */
#define SYS_XAIN2_ODMC_EN                    ((uint32_t)0x08000000)            /* XAIN2 IO open drain enable */
#define SYS_XAIN3_ODMC_EN                    ((uint32_t)0x10000000)            /* XAIN3 IO open drain enable */
#define SYS_XAIN4_ODMC_EN                    ((uint32_t)0x20000000)            /* XAIN4 IO open drain enable */
#define SYS_XAIN5_ODMC_EN                    ((uint32_t)0x40000000)            /* XAIN5 IO open drain enable */
#define SYS_XAIN6_ODMC_EN                    ((uint32_t)0x80000000)            /* XAIN6 IO open drain enable */

/* IO_OD_EN1 */
#define SYS_XAIN7_ODMC_EN                    ((uint32_t)0x00000001)            /* XAIN7 IO open drain enable */
#define SYS_XGPIO_33_ODMC_EN                 ((uint32_t)0x00000002)            /* XGPIO_33 IO open drain enable */
#define SYS_XGPIO_34_ODMC_EN                 ((uint32_t)0x00000004)            /* XGPIO_34 IO open drain enable */
#define SYS_XGPIO_35_ODMC_EN                 ((uint32_t)0x00000008)            /* XGPIO_35 IO open drain enable */
#define SYS_XGPIO_36_ODMC_EN                 ((uint32_t)0x00000010)            /* XGPIO_36 IO open drain enable */
#define SYS_XETM0_EXT_CLK_ODMC_EN            ((uint32_t)0x00000020)            /* XETM0_EXT_CLK IO open drain enable */
#define SYS_XETM0_TC_IN_ODMC_EN              ((uint32_t)0x00000040)            /* XETM0_TC_IN IO open drain enable */
#define SYS_XETM0_CH1_ODMC_EN                ((uint32_t)0x00000080)            /* XETM0_CH1 IO open drain enable */
#define SYS_XETM0_CH2_ODMC_EN                ((uint32_t)0x00000100)            /* XETM0_CH2 IO open drain enable */
#define SYS_XTXD1_ODMC_EN                    ((uint32_t)0x00000200)            /* XTXD1 IO open drain enable */
#define SYS_XRXD1_ODMC_EN                    ((uint32_t)0x00000400)            /* XRXD1 IO open drain enable */
#define SYS_XPWM8_1_ODMC_EN                  ((uint32_t)0x00000800)            /* XPWM8_1 IO open drain enable */
#define SYS_XPWM8_2_ODMC_EN                  ((uint32_t)0x00001000)            /* XPWM8_2 IO open drain enable */
#define SYS_XETM2_EXT_CLK_ODMC_EN            ((uint32_t)0x00002000)            /* XETM2_EXT_CLK IO open drain enable */
#define SYS_XETM2_TC_IN_ODMC_EN              ((uint32_t)0x00004000)            /* XETM2_TC_IN IO open drain enable */
#define SYS_XETM2_CH1_ODMC_EN                ((uint32_t)0x00008000)            /* XETM2_CH1 IO open drain enable */
#define SYS_XETM2_CH2_ODMC_EN                ((uint32_t)0x00010000)            /* XETM2_CH2 IO open drain enable */
#define SYS_XACMP_0_ODMC_EN                  ((uint32_t)0x00020000)            /* XACMP_0 IO open drain enable */
#define SYS_XACMP_1_ODMC_EN                  ((uint32_t)0x00040000)            /* XACMP_1 IO open drain enable */
#define SYS_XACMP_2_ODMC_EN                  ((uint32_t)0x00080000)            /* XACMP_2 IO open drain enable */
#define SYS_XSDA2_ODMC_EN                    ((uint32_t)0x00100000)            /* XSDA2 IO open drain enable */
#define SYS_XSCL2_ODMC_EN                    ((uint32_t)0x00200000)            /* XSCL2 IO open drain enable */
#define SYS_XTXD0_ODMC_EN                    ((uint32_t)0x00400000)            /* XTXD0 IO open drain enable */
#define SYS_XRXD0_ODMC_EN                    ((uint32_t)0x00800000)            /* XRXD0 IO open drain enable */
#define SYS_XGPIO_56_ODMC_EN                 ((uint32_t)0x01000000)            /* XGPIO_56 IO open drain enable */
#define SYS_XGPIO_57_ODMC_EN                 ((uint32_t)0x02000000)            /* XGPIO_57 IO open drain enable */

/* IO_IE_CTRL0 */
#define SYS_XIN_IE_IN_EN                     ((uint32_t)0x00000001)            /* XIN IE pad input enable */
#define SYS_XOUT_IE_IN_EN                    ((uint32_t)0x00000002)            /* XOUT IE pad input enable */
#define SYS_XTXD2_IE_IN_EN                   ((uint32_t)0x00000004)            /* XTXD2 IE pad input enable */
#define SYS_XRXD2_IE_IN_EN                   ((uint32_t)0x00000008)            /* XRXD2 IE pad input enable */
#define SYS_XTIN_IE_IN_EN                    ((uint32_t)0x00000020)            /* XTIN IE pad input enable */
#define SYS_XTOIUT_IE_IN_EN                  ((uint32_t)0x00000040)            /* XTOUT IE pad input enable */
#define SYS_XGPIO_7_IE_IN_EN                 ((uint32_t)0x00000080)            /* XGPIO_7 IE pad input enable */
#define SYS_XNMI_IE_IN_EN                    ((uint32_t)0x00000100)            /* XNMI IO IE pad input enable */
#define SYS_XGPIO_9_IE_IN_EN                 ((uint32_t)0x00000200)            /* XGPIO_9 IE pad input enable */
#define SYS_XSDA1_IE_IN_EN                   ((uint32_t)0x00000400)            /* XSDA1 IE pad input enable */
#define SYS_XSCL1_IE_IN_EN                   ((uint32_t)0x00000800)            /* XSCL1 IE pad input enable */
#define SYS_XSWD_CLK_IE_IN_EN                ((uint32_t)0x00001000)            /* XSWD_CLK IE pad input enable */
#define SYS_XSWD_DIO_IE_IN_EN                ((uint32_t)0x00002000)            /* XSWD_DIO IE pad input enable */
#define SYS_XSPI_CLK_IE_IN_EN                ((uint32_t)0x00004000)            /* XSPI_CLK IE pad input enable */
#define SYS_XSDA0_CLK_IE_IN_EN               ((uint32_t)0x00008000)            /* XSDA0 IE pad input enable */
#define SYS_XSCL0_IE_IN_EN                   ((uint32_t)0x00010000)            /* XSCL0 IE pad input enable */
#define SYS_XETM1_EXT_CLK_IE_IN_EN           ((uint32_t)0x00020000)            /* XETM1 ext clk IE pad input enable */
#define SYS_XETM1_TC_IE_IN_EN                ((uint32_t)0x00040000)            /* XETM1 Tc In IE pad input enable */
#define SYS_XETM1_CH1_IE_IN_EN               ((uint32_t)0x00080000)            /* XETM1 Ch1 IE pad input enable */
#define SYS_XETM1_CH2_IE_IN_EN               ((uint32_t)0x00100000)            /* XETM1 Ch2 IE pad input enable */
#define SYS_XAIN12_IE_IN_EN                  ((uint32_t)0x00200000)            /* XAIN12 IE pad input enable */
#define SYS_XAIN13_IE_IN_EN                  ((uint32_t)0x00400000)            /* XAIN13 IE pad input enable */
#define SYS_XAIN14_IE_IN_EN                  ((uint32_t)0x00800000)            /* XAIN14 IE pad input enable*/
#define SYS_XAIN15_IE_IN_EN                  ((uint32_t)0x01000000)            /* XAIN15 IE pad input enable */
#define SYS_XAIN0_IE_IN_EN                   ((uint32_t)0x02000000)            /* XAIN0 IE pad input enable */
#define SYS_XAIN1_IE_IN_EN                   ((uint32_t)0x04000000)            /* XAIN1 IE pad input enable */
#define SYS_XAIN2_IE_IN_EN                   ((uint32_t)0x08000000)            /* XAIN2 IE pad input enable*/
#define SYS_XAIN3_IE_IN_EN                   ((uint32_t)0x10000000)            /* XAIN3 IE pad input enable */
#define SYS_XAIN4_IE_IN_EN                   ((uint32_t)0x20000000)            /* XAIN4 IE pad input enable */
#define SYS_XAIN5_IE_IN_EN                   ((uint32_t)0x40000000)            /* XAIN5 IE pad input enable */
#define SYS_XAIN6_IE_IN_EN                   ((uint32_t)0x80000000)            /* XAIN6 IE pad input enable */

/* IO_IE_CTRL1 */
#define SYS_XAIN7_IE_IN_EN                   ((uint32_t)0x00000001)            /* XAIN7 IO IE pad input enable */
#define SYS_XGPIO_33_IE_IN_EN                ((uint32_t)0x00000002)            /* XGPIO_33 IE pad input enable */
#define SYS_XGPIO_34_IE_IN_EN                ((uint32_t)0x00000004)            /* XGPIO_34 IE pad input enable */
#define SYS_XGPIO_35_IE_IN_EN                ((uint32_t)0x00000008)            /* XGPIO_35 IE pad input enable */
#define SYS_XGPIO_36_IE_IN_EN                ((uint32_t)0x00000010)            /* XGPIO_36 IE pad input enable */
#define SYS_XETM0_EXT_CLK_IE_IN_EN           ((uint32_t)0x00000020)            /* XETM0_EXT_CLK IE pad input enable */
#define SYS_XETM0_TC_IN_IE_IN_EN             ((uint32_t)0x00000040)            /* XETM0_TC_IN IE pad input enable */
#define SYS_XETM0_CH1_IE_IN_EN               ((uint32_t)0x00000080)            /* XETM0_CH1 IE pad input enable */
#define SYS_XETM0_CH2_IE_IN_EN               ((uint32_t)0x00000100)            /* XETM0_CH2 IE pad input enable */
#define SYS_XTXD1_IE_IN_EN                   ((uint32_t)0x00000200)            /* XTXD1 IE pad input enable */
#define SYS_XRXD1_IE_IN_EN                   ((uint32_t)0x00000400)            /* XRXD1 IE pad input enable */
#define SYS_XPWM8_1_IE_IN_EN                 ((uint32_t)0x00000800)            /* XPWM8_1 IE pad input enable */
#define SYS_XPWM8_2_IE_IN_EN                 ((uint32_t)0x00001000)            /* XPWM8_2 IE pad input enable */
#define SYS_XETM2_EXT_CLK_IE_IN_EN           ((uint32_t)0x00002000)            /* XETM2_EXT_CLK IE pad input enable */
#define SYS_XETM2_TC_IN_IE_IN_EN             ((uint32_t)0x00004000)            /* XETM2_TC_IN IE pad input enable */
#define SYS_XETM2_CH1_IE_IN_EN               ((uint32_t)0x00008000)            /* XETM2_CH1 IE pad input enable */
#define SYS_XETM2_CH2_IE_IN_EN               ((uint32_t)0x00010000)            /* XETM2_CH2 IE pad input enable */
#define SYS_XACMP_0_IE_IN_EN                 ((uint32_t)0x00020000)            /* XACMP_0 IE pad input enable */
#define SYS_XACMP_1_IE_IN_EN                 ((uint32_t)0x00040000)            /* XACMP_1 IE pad input enable */
#define SYS_XACMP_2_IE_IN_EN                 ((uint32_t)0x00080000)            /* XACMP_2 IE pad input enable */
#define SYS_XSDA2_IE_IN_EN                   ((uint32_t)0x00100000)            /* XSDA2 IE pad input enable */
#define SYS_XSCL2_IE_IN_EN                   ((uint32_t)0x00200000)            /* XSCL2 IE pad input enable */
#define SYS_XTXD0_IE_IN_EN                   ((uint32_t)0x00400000)            /* XTXD0 IE pad input enable */
#define SYS_XRXD0_IE_IN_EN                   ((uint32_t)0x00800000)            /* XRXD0 IE pad input enable */
#define SYS_XGPIO_56_IE_IN_EN                ((uint32_t)0x01000000)            /* XGPIO_56 IE pad input enable */
#define SYS_XGPIO_57_IE_IN_EN                ((uint32_t)0x02000000)            /* XGPIO_57 IE pad input enable */
#endif

#if ((defined(CHIP_GM6611)) || (defined(CHIP_GM6621)) || (defined(CHIP_GM6611B)))

#if defined(CHIP_GM6611B)
#define SYS_SRS_STOPDEEP_PENDING             ((uint32_t)0x00000020)
#define SYS_SRS_STOP_PENDING                 ((uint32_t)0x00000040)
#define SYS_SRS_GPIOWKUP_PENDING             ((uint32_t)0x00000100)
#endif

#define  SYS_SRS_LVD                         ((uint32_t)0x00000001)            /* System Reset by Low Voltage Detect(LVD) Circuit or Power On Reset(POR) Circuit  */
#define  SYS_SRS_WDOG                        ((uint32_t)0x00000002)            /* System Reset by Watchdog Module */
#define  SYS_SRS_LOCKUP                      ((uint32_t)0x00000004)            /* System Reset by Cortex-M Lockup Event */
#define  SYS_SRS_SW                          ((uint32_t)0x00000008)            /* System Reset by Software Request */
#define  SYS_SRS_HOSC_LSRSTPD                ((uint32_t)0x00000010)            /* System Reset by HOSC Loss */
#define  SYS_SRS_HOSC_LSPD                   ((uint32_t)0x00000080)            /* HOSC Loss Pending */

#define  SYS_CFGVAL_MODE                     ((uint32_t)0x00000001)            /* Selection of Function Mode */
#define  SYS_CFGVAL_SUBMODE                  ((uint32_t)0x0000000E)            /* Selection of Test SubMode */
#define  SYS_CFGVAL_SUBMODE_SHIFT            (1)
#define  SYS_CFGVAL_SUBMODE_FIELD(x)         (((uint32_t)(((uint32_t)(x))<<SYS_CFGVAL_SUBMODE_SHIFT))&SYS_CFGVAL_SUBMODE)
#define  SYS_CFGVAL_PLL_CS                   ((uint32_t)0x00000010)            /* PLL Clock Source, 0: Internal RC OSC 1: External Crystal OSC */

#define  SYS_CFGTRG_MODE                     ((uint32_t)0x00000001)            /* System Function Mode Trigger */
#define  SYS_CFGTRG_SUBMODE                  ((uint32_t)0x00000002)            /* System Test SubMode Trigger */
#define  SYS_CFGTRG_PLL_CS                   ((uint32_t)0x00000004)            /* PLL Clock Source Trigger */

#define  SYS_CFGMISC_BOOTSEQ_MASK            ((uint32_t)0x00000003)
#define  SYS_CFGMISC_BOOTSEQ_FIELD(x)        ((uint32_t)(((uint32_t)(x))&SYS_CFGMISC_BOOTSEQ_MASK))
#define  SYS_CFGMISC_BOOTSEQ(reg)            ((reg)&SYS_CFGMISC_BOOTSEQ_MASK)
#define  SYS_CFGMISC_PMU_EN                  ((uint32_t)0x00000008)
#define  SYS_CFGMISC_LOCKUP_RESET            ((uint32_t)0x00000010)
//#define  SYS_CFGMISC_PLL_BYPASS            ((uint32_t)0x00000100)            /* Bypass PLL */

#if (defined(CHIP_GM6611B) || defined(CHIP_GM6611))
#define  SYS_SCG_CAN                         ((uint32_t)(1 << 16))              /* CAN peripheral clock gate */
#define  SYS_SCG_SPI1                        ((uint32_t)(1 << 15))              /* SPI1 peripheral clock gate */
#define  SYS_SCG_LCD                         ((uint32_t)(1 << 14))              /* LCD peripheral clock gate */
#define  SYS_SCG_ACMP                        ((uint32_t)(1 << 13))              /* ACMP peripheral clock gate */
#define  SYS_SCG_ADC                         ((uint32_t)(1 << 12))              /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)(1 << 11))              /* ETM peripheral clock gate */
#define  SYS_SCG_DMA                         ((uint32_t)(1 << 10))              /* DMA peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)(1 << 9))               /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)(1 << 8))               /* CRC peripheral clock gate */
#define  SYS_SCG_HWIR                        ((uint32_t)(1 << 7))               /* HWIR peripheral clock gate */
#define  SYS_SCG_GPIO                        ((uint32_t)(1 << 6))               /* GPIO peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)(1 << 5))               /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)(1 << 4))               /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)(1 << 3))               /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C2                        ((uint32_t)(1 << 2))               /* I2C2 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)(1 << 1))               /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)(1 << 0))               /* I2C0 peripheral clock gate */
#endif

#if defined(CHIP_GM6621)
#define  SYS_SCG_WDT                         ((uint32_t)(1UL << 31))            /* WDT peripheral clock gate */
#define  SYS_SCG_LPSTM                       ((uint32_t)(1 << 22))              /* LPSTM peripheral clock gate */
#define  SYS_SCG_BTM                         ((uint32_t)(1 << 21))              /* BTM peripheral clock gate */
#define  SYS_SCG_IRTX                        ((uint32_t)(1 << 20))              /* IRTX peripheral clock gate */
#define  SYS_SCG_SPI1                        ((uint32_t)(1 << 15))              /* SPI1 peripheral clock gate */
#define  SYS_SCG_ADC                         ((uint32_t)(1 << 12))              /* ADC peripheral clock gate */
#define  SYS_SCG_ETM                         ((uint32_t)(1 << 11))              /* ETM peripheral clock gate */
#define  SYS_SCG_DMA                         ((uint32_t)(1 << 10))              /* DMA peripheral clock gate */
#define  SYS_SCG_SPI                         ((uint32_t)(1 << 9))               /* SPI peripheral clock gate */
#define  SYS_SCG_CRC                         ((uint32_t)(1 << 8))               /* CRC peripheral clock gate */
#define  SYS_SCG_GPIO                        ((uint32_t)(1 << 6))               /* GPIO peripheral clock gate */
#define  SYS_SCG_UART2                       ((uint32_t)(1 << 5))               /* UART2 peripheral clock gate */
#define  SYS_SCG_UART1                       ((uint32_t)(1 << 4))               /* UART1 peripheral clock gate */
#define  SYS_SCG_UART0                       ((uint32_t)(1 << 3))               /* UART0 peripheral clock gate */
#define  SYS_SCG_I2C1                        ((uint32_t)(1 << 1))               /* I2C1 peripheral clock gate */
#define  SYS_SCG_I2C0                        ((uint32_t)(1 << 0))               /* I2C0 peripheral clock gate */
#endif

#define  SYS_MCS_WDT_SWITCH                  ((uint32_t)0x00040000)            /* Watchdog multiplex switch */
#define  SYS_MCS_RTC2_SWITCH                 ((uint32_t)0x00004000)            /* RTC2 multiplex switch */
#define  SYS_MCS_RTC1_SWITCH                 ((uint32_t)0x00000400)            /* RTC1 multiplex switch */
#define  SYS_MCS_RTC0_SWITCH                 ((uint32_t)0x00000040)            /* RTC0 multiplex switch */

#define  SYS_SCS_CS_MASK                     ((uint32_t)0x00000007)
#define  SYS_SCS_CS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SCS_CS_MASK))
#define  SYS_SCS_CS(reg)                     ((reg)&SYS_SCS_CS_MASK)
#define  SYS_SCS_PLL_RC                      0                                 /* System Clock Source is PLL Referrence Clock (Internal RC Oscillator or External High Frequency Oscillator */
#define  SYS_SCS_PLL_6X                      1                                 /* System Clock Source is PLL output with Multiplication Factor 6 */
#define  SYS_SCS_PLL_3X                      2                                 /* System Clock Source is PLL output with Multiplication Factor 3 */
#define  SYS_SCS_LF_OSC                      3                                 /* System Clock Source is Low Frequency Oscillator cowork with external crystal or resonator */
#define  SYS_SCS_PLL_BYPASS                  ((uint32_t)0x00000100)            /* Bypass PLL */
#define  SYS_SCS_CORE_SWITCH                 ((uint32_t)0x00000004)            /* FCLK multiplex switch */
#define  SYS_PLL_PLLCLK_REF                  ((uint32_t)0x00000002)            /**< PLL reference clock */

#define  SYS_SCS_SWT_EN                      ((uint32_t)0x00000004)            /* Clock Switch Module Enabled */

#define  SYS_MCS_UART0_CLKSRC                ((uint32_t)0x00000001)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART1_CLKSRC                ((uint32_t)0x00000002)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */
#define  SYS_MCS_UART2_CLKSRC                ((uint32_t)0x00000004)            /* UART0 Clock Source 0:1.834MHZ 1:PLL_CLK  */

#define  SYS_SPS_PS_MASK                     ((uint32_t)0x00000003)
#define  SYS_SPS_PS_FIELD(x)                 ((uint32_t)(((uint32_t)(x))&SYS_SPS_PS_MASK))
#define  SYS_SPS_PS(reg)                     ((reg)&SYS_SPS_PS_MASK)
#define  SYS_SPS_1                           0                                 /* System Clock Prescaler Factor is 1 */
#define  SYS_SPS_2                           1                                 /* System Clock Prescaler Factor is 2 */
#define  SYS_SPS_4                           2                                 /* System Clock Prescaler Factor is 4 */
#define  SYS_SPS_8                           3                                 /* System Clock Prescaler Factor is 8 */
#define  SYS_SPS_16                          4                                 /* System Clock Prescaler Factor is 16 */
#define  SYS_SPS_32                          5                                 /* System Clock Prescaler Factor is 32 */
#define  SYS_SPS_64                          6                                 /* System Clock Prescaler Factor is 64 */
#define  SYS_SPS_128                         7                                 /* System Clock Prescaler Factor is 128 */

#define  SYS_IRC_CRY8M                       ((uint32_t)0x00000001)            /* IRC 8MHz power */

#define  SYS_OSC_CRY8M                       ((uint32_t)0x00000001)            /* OSC 8MHz power */
#define  SYS_OSC_CRY32K                      ((uint32_t)0x00000100)            /* OSC 32KHz power */

#if defined(CHIP_GM6621)
#define  SYS_OSC_OUTSEL                      ((uint32_t)0x00010000)            /* OSC out select */
#define  SYS_AOP_CRY32K                      ((uint32_t)0x00000001)            /* OSC 32K power */
#define  SYS_AOP_SRS_STANDBYFLG              ((uint32_t)0x80000000)            /* Reset from Standby Flag*/
#define  SYS_AOP_LSCCTRL_OSCPINSEL           ((uint32_t)0x80000000)            /* LOSC use HIN/OUT pin*/

#define SYS_AOP_PMC_PMUEN_POS                (0)
#define SYS_AOP_PMC_PMUEN_MASK               ((uint32_t)(1 << SYS_AOP_PMC_PMUEN_POS))
#define SYS_AOP_PMC_STOPEN_POS               (1)
#define SYS_AOP_PMC_STOPEN_MASK              ((uint32_t)(1 << SYS_AOP_PMC_STOPEN_POS))
#define SYS_AOP_PMC_STBYEN_POS               (2)
#define SYS_AOP_PMC_STBYEN_MASK              ((uint32_t)(1 << SYS_AOP_PMC_STBYEN_POS))
#define SYS_AOP_PMC_EFLASHSTBYEN_POS         (4)
#define SYS_AOP_PMC_EFLASHSTBYEN_MASK        ((uint32_t)(1 << SYS_AOP_PMC_EFLASHSTBYEN_POS))
#define SYS_AOP_PMC_LDOTRIMEN_POS            (5)
#define SYS_AOP_PMC_LDOTRIMEN_MASK           ((uint32_t)(1 << SYS_AOP_PMC_LDOTRIMEN_POS))
#define SYS_AOP_PMC_LCDACCESSEN_POS          (6)
#define SYS_AOP_PMC_LCDACCESSEN_MASK         ((uint32_t)(1 << SYS_AOP_PMC_LCDACCESSEN_POS))

#define SYS_AOP_PMU_LOADEN_POS               (0)
#define SYS_AOP_PMU_LOADEN_MASK              ((uint32_t)(1 << SYS_AOP_PMU_LOADEN_POS))
#define SYS_AOP_PMU_LDOMODE_POS              (1)
#define SYS_AOP_PMU_LDOMODE_MASK             ((uint32_t)(1 << SYS_AOP_PMU_LDOMODE_POS))

#define SYS_AOP_SYS_STOPMODEFLAG_POS                 (30)
#define SYS_AOP_SYS_STOPMODEFLAG_MASK            ((uint32_t)(1 << SYS_AOP_SYS_STOPMODEFLAG_POS))

#define  SYS_PLL_CLKREF_MASK                 ((uint32_t)0x00000006)            /* PLL reference clock mask */
#define  SYS_PLL_CLKREF_HIRC                 ((uint32_t)0x00000000)            /* PLL reference from HIRC */
#define  SYS_PLL_CLKREF_HOSC                 ((uint32_t)0x00000002)            /* PLL reference from HOSC */
#define  SYS_PLL_CLKREF_LOSC                 ((uint32_t)0x00000004)            /* PLL reference from LOSC */
#define  SYS_PLL_DIVSEL_LFEN                 ((uint32_t)0x00001000)            /* Divider selection valid only for LOSC*/
#endif

#define  SYS_ANASEL_HOSCIN                   ((uint32_t)0x00000001)            /* OSC 8MHz selection in */
#define  SYS_ANASEL_HOSCOUT                  ((uint32_t)0x00000002)            /* OSC 8MHz selection out */
#define  SYS_ANASEL_LOSCIN                   ((uint32_t)0x00000004)            /* OSC 32KHz selection in */
#define  SYS_ANASEL_LOSCOUT                  ((uint32_t)0x00000008)            /* OSC 32KHz selection out */

#define  SYS_PLL_PWRON                       ((uint32_t)0x00000001)            /* PLL power on */
#define  SYS_DBG_WDOG_STOP                   ((uint32_t)0x00000001)            /* DBG_CFG_CTRL: bit0  WDOG will be stoped*/
#define  SYS_DGB_WDOG_WRITEDONE              ((uint32_t)0x00010000)            /* DBG_CFG_CTRL: bit16 WDOG wirte done flag*/
#endif




/* ETM_Sys_Reg Bit Fields */
#define SYS_ESS_EN                           ((uint32_t)0x00000004)            /* Enable ETM sync */
#define SYS_ESS_SEL_MASK                     ((uint32_t)0x00000003)            /* ETM Sync source selection */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6621) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C) )
#define  SYS_FLTCPS_CLK0_MASK               ((uint32_t)0x00000600)
#define  SYS_FLTCPS_CLK0_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<9)&SYS_FLTCPS_CLK0_MASK))
#define  SYS_FLTCPS_CLK1_MASK               ((uint32_t)0x00003800)
#define  SYS_FLTCPS_CLK1_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<11)&SYS_FLTCPS_CLK1_MASK))
#define  SYS_FLTCPS_CLK2_MASK               ((uint32_t)0x0000c000)
#define  SYS_FLTCPS_CLK2_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<14)&SYS_FLTCPS_CLK2_MASK))
#else
#define  SYS_FLTCPS_CLK0_MASK               ((uint32_t)0x00000003)
#define  SYS_FLTCPS_CLK0_FIELD(x)           ((uint32_t)(((uint32_t)(x))&SYS_FLTCPS_CLK0_MASK))
#define  SYS_FLTCPS_CLK1_MASK               ((uint32_t)0x00000070)
#define  SYS_FLTCPS_CLK1_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<4)&SYS_FLTCPS_CLK1_MASK))
#define  SYS_FLTCPS_CLK2_MASK               ((uint32_t)0x00000300)
#define  SYS_FLTCPS_CLK2_FIELD(x)           ((uint32_t)((((uint32_t)(x))<<8)&SYS_FLTCPS_CLK2_MASK))
#endif

#define  SYSDEV                               ((SYS_Device_T *)(0x40000000u))

#if defined(CHIP_GM6621)
#define  AOPDEV                               ((AOP_Device_T *)(0x40039000u))
#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _SYS_H_ */
