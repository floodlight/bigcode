/**********************************************************************
 *
 * inf_8628.h
 *
 * idprom defintions for QSFP-DD modules.
 *
 * INF-8628 QSFP-DD 8X Transceiver (QSFP Double Density)
 **********************************************************************/

#ifndef __INF_8628_H__
#define __INF_8628_H__

#include <sff/sff_config.h>

/* identifier, bytes 128 page 0 (also byte 0) -  Module ID from SFF-8024 (TABLE 4-1) list. */
#define SFF8024_IDENT_UNKNOWN			0x00 /* Unknown or unspecified */
#define SFF8024_IDENT_GBIC				0x01 /* GBIC */
#define SFF8024_IDENT_BASEBOARD			0x02 /* Module/connector soldered to motherboard (using SFF-8472) */
#define SFF8024_IDENT_SFP				0x03 /* SFP/SFP+/SFP28 */
#define SFF8024_IDENT_XBI				0x04 /* 300 pin XBI */
#define SFF8024_IDENT_XENPAK			0x05 /* XENPAK */
#define SFF8024_IDENT_XFP				0x06 /* XFP */
#define SFF8024_IDENT_XFF				0x07 /* XFF */
#define SFF8024_IDENT_XFPE				0x08 /* XFP-E */
#define SFF8024_IDENT_XPAK				0x09 /* XPAK */
#define SFF8024_IDENT_X2				0x0A /* X2 */
#define SFF8024_IDENT_DWDM_SFP			0x0B /* DWDM-SFP/SFP+ (not using SFF-8472) */
#define SFF8024_IDENT_QSFP				0x0C /* QSFP (INF-8438) */
#define SFF8024_IDENT_QSFP_PLUS			0x0D /* QSFP+ or later (SFF-8436, SFF-8635, SFF-8665, SFF-8685 et al) *1 */
#define SFF8024_IDENT_CXP				0x0E /* CXP or later */
#define SFF8024_IDENT_SHIELD_4X			0x0F /* Shielded Mini Multilane HD 4X */
#define SFF8024_IDENT_SHIELD_8X			0x10 /* Shielded Mini Multilane HD 8X */
#define SFF8024_IDENT_QSFP28			0x11 /* QSFP28 or later (SFF-8665 et al) *2 */
#define SFF8024_IDENT_CXP2				0x12 /* CXP2 (aka CXP28) or later */
#define SFF8024_IDENT_CDFP				0x13 /* CDFP (Style 1/Style2) */
#define SFF8024_IDENT_SHIELD_4X_FANOUT	0x14 /* Shielded Mini Multilane HD 4X Fanout Cable */
#define SFF8024_IDENT_SHIELD_8X_FANOUT	0x15 /* Shielded Mini Multilane HD 8X Fanout Cable */
#define SFF8024_IDENT_CDFP3				0x16 /* CDFP (Style 3) */
#define SFF8024_IDENT_MICRO_QSFP		0x17 /* microQSFP */
#define SFF8024_IDENT_QSFP_DD			0x18 /* QSFP-DD Double Density 8X Pluggable Transceiver (INF-8628) */


/* additional values from this spec */
#define INF8628_IDENT_QSFP_DD          SFF8024_IDENT_QSFP_DD

#define INF8628_MODULE_QSFP_DD(idprom)           \
    (idprom[0] == INF8628_IDENT_QSFP_DD)

/* 
*  Media Connector Type - Upper Memory Page 00h, Byte 203
*  See SFF-8024 (TABLE 4-3 CONNECTOR TYPES) for codes.
*/
#define SFF8024_CONN_UNKNOWN   		0x00 /* Unknown or unspecified */
#define SFF8024_CONN_SC        		0x01 /* SC (Subscriber Connector) */
#define SFF8024_CONN_FC1_CU    		0x02 /* Fibre Channel Style 1 copper connector */
#define SFF8024_CONN_FC2_CU    		0x03 /* Fibre Channel Style 2 copper connector */
#define SFF8024_CONN_BNC       		0x04 /* BNC/TNC (Bayonet/Threaded Neill-Concelman) */
#define SFF8024_CONN_FC_COAX   		0x05 /* Fibre Channel coax headers */
#define SFF8024_CONN_FJ        		0x06 /* Fiber Jack */
#define SFF8024_CONN_LC        		0x07 /* LC (Lucent Connector) */
#define SFF8024_CONN_MT_RJ     		0x08 /* MT-RJ (Mechanical Transfer - Registered Jack) */
#define SFF8024_CONN_MU        		0x09 /* MU (Multiple Optical) */
#define SFF8024_CONN_SG        		0x0A /* SG */
#define SFF8024_CONN_SI_PIGTAIL		0x0B /* Optical Pigtail */
#define SFF8024_CONN_MPO_1X2   		0x0C /* MPO 1x12 (Multifiber Parallel Optic) */
#define SFF8024_CONN_MPO_2X16  		0x0D /* MPO 2x16 */
#define SFF8024_CONN_HSSDC_II  		0x20 /* HSSDC II (High Speed Serial Data Connector) */
#define SFF8024_CONN_CU_PIGTAIL		0x21 /* Copper pigtail */
#define SFF8024_CONN_RJ45      		0x22 /* RJ45 (Registered Jack) */
#define SFF8024_CONN_NOSEP     		0x23 /* No separable connector */
#define SFF8024_CONN_MXC 			0x24 /* MXC 2x16 */

/* addtional values from this spec */
#define INF8628_CONN_NONE           SFF8024_CONN_NOSEP

/* LEONI QSFP-DD media connecter type - 0x23 */
#define INF8628_MEDIA_400GE_CR(idprom)                    \
  (idprom[203] == INF8628_CONN_NONE)

/* 
*  QSFP-DD - Common management specification Rev 3.0 
*  Table 78 - Module Host Electrical Interfaces Codes (Page 00h byte 86)
*   
*/
#define INF8628_400GAUI_16_C2M			0X10 /* 400GAUI-16 C2M (Annex 120C) */
#define INF8628_400GAUI_8_C2M			0X11 /* 400GAUI-8 C2M (Annex 120C) */
#define INF8628_400G_CR8				0X1D /* 400G CR8 */

#define INF8628_MEDIA_400GAUI_16_C2M(idprom)                   \
  ((idprom[86] & INF8628_400GAUI_16_C2M) != 0)
#define INF8628_MEDIA_400GAUI_8_C2M(idprom)                   \
  ((idprom[86] & INF8628_400GAUI_8_C2M) != 0)
#define INF8628_MEDIA_400G_CR8(idprom)                    \
  ((idprom[86] & INF8628_400G_CR8) != 0)

/*
 * Cable Assembly length - Uper page 00h byte 202
 */
static inline int
_inf8628_qsfp_dd_cable_length(const uint8_t *idprom)
{
    /* module should be qsfp-dd */
    if (!INF8628_MODULE_QSFP_DD(idprom))
        return -1;

    /* Bit 7~6 is the multiplier */
    if (((idprom[202] >> 6) & 0xff) == 0x00) return ((idprom[202] & 0x3f) * 0.1);
    else if (((idprom[202] >> 6) & 0xff) == 0x01) return ((idprom[202] & 0x3f) * 1);
    else if (((idprom[202] >> 6) & 0xff) == 0x10) return ((idprom[202] & 0x3f) * 10);
    else if (((idprom[202] >> 6) & 0xff) == 0x11) return ((idprom[202] & 0x3f) * 100);
    else return -1; 
}

#endif /* __INF_8628_H__ */
