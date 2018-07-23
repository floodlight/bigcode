/**************************************************************************//**
 *
 *
 *
 *
 *****************************************************************************/
#include <sff/8472.h>
#include <sff/8436.h>
#include <sff/8636.h>
#include <math.h>
#include <arpa/inet.h>
#include <sff/sff.h>
#include <sff/dom.h>
#include <cjson_util/cjson_util_format.h>
#include "sff_log.h"

/**
 * SFF-8472 DOM Structure
 */
typedef struct sff8472_dom_s {
    uint16_t bias_cur;      /* Measured bias current in 2uA units */
    uint16_t tx_power;      /* Measured TX Power in 0.1uW units */
    uint16_t rx_power;      /* Measured RX Power */
    int16_t sfp_temp;      /* SFP Temp in 16-bit signed 1/256 Celsius */
    uint16_t sfp_voltage;   /* SFP voltage in 0.1mV units */
} sff8472_dom_t;

/**
 * SFF-8436 DOM Structure
 */
struct sff8436_channel_diags {
    uint16_t bias_cur;      /* Measured bias current in 2uA units */
    uint16_t rx_power;      /* Measured RX Power */
};
typedef struct sff8436_dom_s {
#define MAX_CHANNEL_NUM 4
    int16_t sfp_temp;       /* SFP Temp in 16-bit signed 1/256 Celsius */
    uint16_t sfp_voltage;   /* SFP voltage in 0.1mV units */
    struct sff8436_channel_diags scd[MAX_CHANNEL_NUM];
} sff8436_dom_t;

/**
 * SFF-8636 DOM Structure
 */
struct sff8636_channel_diags {
    uint16_t bias_cur;      /* Measured bias current in 2uA units */
    uint16_t rx_power;      /* Measured RX Power */
    uint16_t tx_power;      /* Measured TX Power */
};
typedef struct sff8636_dom_s {
#define MAX_CHANNEL_NUM 4
    int16_t sfp_temp;       /* SFP Temp in 16-bit signed 1/256 Celsius */
    uint16_t sfp_voltage;   /* SFP voltage in 0.1mV units */
    struct sff8636_channel_diags scd[MAX_CHANNEL_NUM];
} sff8636_dom_t;


double convert_mw_to_dbm(double mw)
{
    return (10. * log10(mw / 1000.)) + 30.;
}

/* Converts to a float from a big-endian 4-byte source buffer. */
static float befloattoh(const uint32_t *source)
{
    union {
        uint32_t src;
        float dst;
    } converter;

    converter.src = ntohl(*source);
    return converter.dst;
}

#define SFF_DOM_GET_RXPWRx(offset)    \
        (befloattoh((uint32_t *)(dom + (offset))))
#define SFF_DOM_GET_SLP(offset)       \
        (dom[offset] + dom[offset + 1] / 256.)
#define SFF_DOM_GET_OFF(offset)       \
        (int16_t)(dom[offset] << 8 | dom[offset + 1])


static void sff8472_dom_calibration(uint8_t* dom, sff8472_dom_t* sd)
{
    uint16_t rx_reading;

    /*
     * Apply calibration formula 1 (Temp., Voltage, Bias, Tx Power)
     */
    sd->bias_cur    *= SFF_DOM_GET_SLP(SFF8472_CAL_TXI_SLP);
    sd->tx_power    *= SFF_DOM_GET_SLP(SFF8472_CAL_TXPWR_SLP);
    sd->sfp_voltage *= SFF_DOM_GET_SLP(SFF8472_CAL_V_SLP);
    sd->sfp_temp    *= SFF_DOM_GET_SLP(SFF8472_CAL_T_SLP);

    sd->bias_cur    += SFF_DOM_GET_OFF(SFF8472_CAL_TXI_OFF);
    sd->tx_power    += SFF_DOM_GET_OFF(SFF8472_CAL_TXPWR_OFF);
    sd->sfp_voltage += SFF_DOM_GET_OFF(SFF8472_CAL_V_OFF);
    sd->sfp_temp    += SFF_DOM_GET_OFF(SFF8472_CAL_T_OFF);

    /*
     * Apply calibration formula 2 (Rx Power only)
     */
    rx_reading = sd->rx_power;
    sd->rx_power    = SFF_DOM_GET_RXPWRx(SFF8472_CAL_RXPWR0);
    sd->rx_power    += rx_reading *
                          SFF_DOM_GET_RXPWRx(SFF8472_CAL_RXPWR1);
    sd->rx_power    += rx_reading *
                          SFF_DOM_GET_RXPWRx(SFF8472_CAL_RXPWR2);
    sd->rx_power    += rx_reading *
                          SFF_DOM_GET_RXPWRx(SFF8472_CAL_RXPWR3);
}



#define SFF_DOM_INFO_OBJ_SET_FIELD(_obj, _field, _FIELD, _value)        \
    do {                                                                \
        (_obj) -> fields |= SFF_DOM_FIELD_FLAG_##_FIELD;                \
        (_obj) -> _field = _value;                                      \
    } while(0)

#define SFF_DOM_CHANNEL_INFO_SET_BIAS_CUR(_ch, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_ch, bias_cur, BIAS_CUR, _value)
#define SFF_DOM_CHANNEL_INFO_SET_RX_POWER(_ch, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_ch, rx_power, RX_POWER, _value)
#define SFF_DOM_CHANNEL_INFO_SET_RX_POWER_OMA(_ch, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_ch, rx_power_oma, RX_POWER_OMA, _value)
#define SFF_DOM_CHANNEL_INFO_SET_TX_POWER(_ch, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_ch, tx_power, TX_POWER, _value)

#define SFF_DOM_INFO_SET_TEMP(_p, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_p, temp, TEMP, _value)
#define SFF_DOM_INFO_SET_VOLTAGE(_p, _value) \
    SFF_DOM_INFO_OBJ_SET_FIELD(_p, voltage, VOLTAGE, _value)

/**
 * @brief Parse and display interested DOM field.
 * @param info sff info structure.
 * @param dom: Raw DOM data.
 */
void
sff8472_dom_info_get(sff_dom_info_t* dinfo, uint8_t* a0, uint8_t* a2)
{
    if (!SFF8472_DOM_SUPPORTED(a0)) {
        dinfo->spec = SFF_DOM_SPEC_UNSUPPORTED;
        return;
    }

    sff8472_dom_t sd;

    sd.rx_power = SFF8472_RX_PWR(a2);
    sd.bias_cur = SFF8472_BIAS_CUR(a2);
    sd.sfp_voltage = SFF8472_SFP_VOLT(a2);
    sd.tx_power = SFF8472_TX_PWR(a2);
    sd.sfp_temp = SFF8472_SFP_TEMP(a2);


    dinfo->spec = SFF_DOM_SPEC_SFF8472;
    dinfo->nchannels = 1;

    /* if SFP is externally calibrated, do calibration */
    if(SFF8472_DOM_USE_EXTCAL(a0)) {
        sff8472_dom_calibration(a2, &sd);
        dinfo->extcal = 1;
    }

    /* convert format to display */
    if(!SFF8472_DOM_GET_RXPWR_TYPE(a0)) {
        SFF_DOM_CHANNEL_INFO_SET_RX_POWER_OMA(dinfo->channels, sd.rx_power);
    } else {
        SFF_DOM_CHANNEL_INFO_SET_RX_POWER(dinfo->channels, sd.rx_power);
    }

    SFF_DOM_CHANNEL_INFO_SET_BIAS_CUR(dinfo->channels, sd.bias_cur);
    SFF_DOM_CHANNEL_INFO_SET_TX_POWER(dinfo->channels, sd.tx_power);
    SFF_DOM_INFO_SET_TEMP(dinfo, sd.sfp_temp);
    SFF_DOM_INFO_SET_VOLTAGE(dinfo, sd.sfp_voltage);
}

/**
 * @brief Parse and display interested DOM field.
 * @param info sff info structure.
 */
void
sff8436_dom_info_get(sff_dom_info_t* dinfo, uint8_t* a0)
{
    sff8436_dom_t sd = {0};
    int i;

    for (i = 0; i < MAX_CHANNEL_NUM; i++) {
        switch (i)
            {
            case 0:
                sd.scd[i].bias_cur = SFF8436_TX1_BIAS(a0);
                sd.scd[i].rx_power = SFF8436_RX1_PWR(a0);
                break;
            case 1:
                sd.scd[i].bias_cur = SFF8436_TX2_BIAS(a0);
                sd.scd[i].rx_power = SFF8436_RX2_PWR(a0);
                break;
            case 2:
                sd.scd[i].bias_cur = SFF8436_TX3_BIAS(a0);
                sd.scd[i].rx_power = SFF8436_RX3_PWR(a0);
                break;
            case 3:
                sd.scd[i].bias_cur = SFF8436_TX4_BIAS(a0);
                sd.scd[i].rx_power = SFF8436_RX4_PWR(a0);
                break;
            default:
                AIM_LOG_ERROR("invalid channel number %d", i);
                break;
            }
    }
    sd.sfp_voltage = SFF8436_SFP_VOLT(a0);
    sd.sfp_temp = SFF8436_SFP_TEMP(a0);

    dinfo->spec = SFF_DOM_SPEC_SFF8436;
    dinfo->nchannels = 4;

    for (i = 0; i < MAX_CHANNEL_NUM; i++) {
        if(!SFF8436_DOM_GET_RXPWR_TYPE(a0)) {
            SFF_DOM_CHANNEL_INFO_SET_RX_POWER_OMA(dinfo->channels + i, sd.scd[i].rx_power);
        } else {
            SFF_DOM_CHANNEL_INFO_SET_RX_POWER(dinfo->channels + i, sd.scd[i].rx_power);
        }
    }

    for (i = 0; i < MAX_CHANNEL_NUM; i++) {
        SFF_DOM_CHANNEL_INFO_SET_BIAS_CUR(dinfo->channels + i, sd.scd[i].bias_cur);
    }

    SFF_DOM_INFO_SET_TEMP(dinfo, sd.sfp_temp);
    SFF_DOM_INFO_SET_VOLTAGE(dinfo, sd.sfp_voltage);
}

/**
 * @brief Parse and display SFF8636 DOM field.
 * @param info sff info structure.
 */
void
sff8636_dom_info_get(sff_dom_info_t* dinfo, uint8_t* a0)
{
    sff8636_dom_t sd = {0};
    int i;

    for (i = 0; i < MAX_CHANNEL_NUM; i++) {
        switch (i) {
            case 0:
                sd.scd[i].bias_cur = SFF8636_TX1_BIAS(a0);
                sd.scd[i].rx_power = SFF8636_RX1_PWR(a0);
                sd.scd[i].tx_power = SFF8636_TX1_PWR(a0);
                break;
            case 1:
                sd.scd[i].bias_cur = SFF8636_TX2_BIAS(a0);
                sd.scd[i].rx_power = SFF8636_RX2_PWR(a0);
                sd.scd[i].tx_power = SFF8636_TX2_PWR(a0);
                break;
            case 2:
                sd.scd[i].bias_cur = SFF8636_TX3_BIAS(a0);
                sd.scd[i].rx_power = SFF8636_RX3_PWR(a0);
                sd.scd[i].tx_power = SFF8636_TX3_PWR(a0);
                break;
            case 3:
                sd.scd[i].bias_cur = SFF8636_TX4_BIAS(a0);
                sd.scd[i].rx_power = SFF8636_RX4_PWR(a0);
                sd.scd[i].tx_power = SFF8636_TX4_PWR(a0);
                break;
            default:
                AIM_LOG_ERROR("invalid channel number %d", i);
                break;
        }
    }
    sd.sfp_voltage = SFF8636_SFP_VOLT(a0);
    sd.sfp_temp = SFF8636_SFP_TEMP(a0);

    dinfo->spec = SFF_DOM_SPEC_SFF8636;
    dinfo->nchannels = 4;

    for (i = 0; i < MAX_CHANNEL_NUM; i++) {

        if(!SFF8636_DOM_GET_RXPWR_TYPE(a0)) {
            SFF_DOM_CHANNEL_INFO_SET_RX_POWER_OMA(dinfo->channels+i, sd.scd[i].rx_power);
        } else {
            SFF_DOM_CHANNEL_INFO_SET_RX_POWER(dinfo->channels+i, sd.scd[i].rx_power);
        }

        SFF_DOM_CHANNEL_INFO_SET_BIAS_CUR(dinfo->channels+i, sd.scd[i].bias_cur);

        if (SFF8636_DOM_GET_TXPWR_SUPPORT(a0)){
            SFF_DOM_CHANNEL_INFO_SET_TX_POWER(dinfo->channels + i, sd.scd[i].tx_power);
        }
    }

    SFF_DOM_INFO_SET_TEMP(dinfo, sd.sfp_temp);
    SFF_DOM_INFO_SET_VOLTAGE(dinfo, sd.sfp_voltage);

}

static int
dom_spec_get_QSFP28__(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    *rspec = SFF_DOM_SPEC_SFF8636;
    return 0;
}
static int
dom_info_get_QSFP28__(sff_dom_info_t* sdi, sff_info_t* si,
                      uint8_t* a0, uint8_t* a2)
{
    sff8636_dom_info_get(sdi, a0);
    return 0;
}


static int
dom_spec_get_QSFP_PLUS__(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    *rspec = SFF_DOM_SPEC_SFF8436;
    return 0;
}
static int
dom_info_get_QSFP_PLUS__(sff_dom_info_t* sdi, sff_info_t* si,
                         uint8_t* a0, uint8_t* a2)
{
    sff8436_dom_info_get(sdi, a0);
    return 0;
}

static int
dom_spec_get_QSFP__(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    *rspec = SFF_DOM_SPEC_UNSUPPORTED;
    return 0;
}
static int
dom_info_get_QSFP__(sff_dom_info_t* sdi, sff_info_t* si,
                    uint8_t* a0, uint8_t* a2)
{
    sdi->spec = SFF_DOM_SPEC_UNSUPPORTED;
    return 0;
}

static int
dom_spec_get_SFP__(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    if (SFF8472_DOM_SUPPORTED(a0)) {
        *rspec = SFF_DOM_SPEC_SFF8472;
    }
    else {
        *rspec = SFF_DOM_SPEC_UNSUPPORTED;
    }
    return 0;
}

static int
dom_spec_get_SFP28__(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    if (SFF8472_DOM_SUPPORTED(a0)) {
        *rspec = SFF_DOM_SPEC_SFF8472;
    }
    else {
        *rspec = SFF_DOM_SPEC_UNSUPPORTED;
    }
    return 0;
}



static int
dom_info_get_SFP__(sff_dom_info_t* sdi, sff_info_t* si,
                   uint8_t* a0, uint8_t* a2)
{
    sff8472_dom_info_get(sdi, a0, a2);
    return 0;
}

static int
dom_info_get_SFP28__(sff_dom_info_t* sdi, sff_info_t* si,
                   uint8_t* a0, uint8_t* a2)
{
    sff8472_dom_info_get(sdi, a0, a2);
    return 0;
}

int
sff_dom_spec_get(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec)
{
    if(si->media_type == SFF_MEDIA_TYPE_COPPER) {
        *rspec = SFF_DOM_SPEC_UNSUPPORTED;
        return 0;
    }

    switch(si->sfp_type)
        {
#define SFF_SFP_TYPE_ENTRY(_id, _desc)          \
            case SFF_SFP_TYPE_##_id: return dom_spec_get_##_id##__(si, a0, rspec);
#include <sff/sff.x>
        default:
            return AIM_ERROR_PARAM;
        }
}


int
sff_dom_info_get(sff_dom_info_t* sdi, sff_info_t* si, uint8_t* a0, uint8_t* a2)
{
    memset(sdi, 0, sizeof(*sdi));

    AIM_SUCCESS_OR_RETURN(sff_dom_spec_get(si, a0, &sdi->spec));
    if(sdi->spec == SFF_DOM_SPEC_UNSUPPORTED) {
        return 0;
    }

    switch(si->sfp_type)
        {
#define SFF_SFP_TYPE_ENTRY(_id, _desc) \
            case SFF_SFP_TYPE_##_id: return dom_info_get_##_id##__(sdi, si, a0, a2);
#include <sff/sff.x>
        default:
            return AIM_ERROR_PARAM;
        }
}

int
sff_dom_info_to_json(sff_dom_info_t* sdi, cJSON** cjp)
{
    if(sdi == NULL || cjp == NULL) {
        return AIM_ERROR_PARAM;
    }

    cJSON* cj = cJSON_CreateObject();
    cjson_util_add_string_to_object(cj, "spec", "%{sff_dom_spec}", sdi->spec);
    if(sdi->spec != SFF_DOM_SPEC_UNSUPPORTED) {
        if(sdi->fields & SFF_DOM_FIELD_FLAG_TEMP) {
            cJSON_AddNumberToObject(cj, "temp", sdi->temp);
        }
        if(sdi->fields & SFF_DOM_FIELD_FLAG_VOLTAGE) {
            cJSON_AddNumberToObject(cj, "voltage", sdi->voltage);
        }
        int c;
        cJSON* carray = cJSON_CreateArray();
        sff_dom_channel_info_t* chp;
        for(c = 0, chp = sdi->channels; c < sdi->nchannels; c++, chp++) {
            cJSON* jc = cJSON_CreateObject();
            if(chp->fields & SFF_DOM_FIELD_FLAG_BIAS_CUR) {
                cJSON_AddNumberToObject(jc, "bias-cur", chp->bias_cur);
            }
            if(chp->fields & SFF_DOM_FIELD_FLAG_RX_POWER) {
                cJSON_AddNumberToObject(jc, "rx-power", chp->rx_power);
            }
            if(chp->fields & SFF_DOM_FIELD_FLAG_RX_POWER_OMA) {
                cJSON_AddNumberToObject(jc, "rx-power-oma", chp->rx_power_oma);
            }
            if(chp->fields & SFF_DOM_FIELD_FLAG_TX_POWER) {
                cJSON_AddNumberToObject(jc, "tx-power", chp->tx_power);
            }
            cJSON_AddItemToArray(carray, jc);
        }
        cJSON_AddItemToObject(cj, "channels", carray);
    }

    if(*cjp) {
        cJSON_AddItemToObject(*cjp, "dom", cj);
    }
    else {
        *cjp = cj;
    }
    return 0;
}
