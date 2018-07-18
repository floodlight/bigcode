/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *        Copyright 2014, 2015 Big Switch Networks, Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#ifndef __SFF_DOM_H__
#define __SFF_DOM_H__

#include <sff/sff.h>

#define SFF_DOM_CHANNEL_COUNT_MAX 4

/* <auto.start.enum(tag:dom).define> */
/** sff_dom_field_flag */
typedef enum sff_dom_field_flag_e {
    SFF_DOM_FIELD_FLAG_TEMP = (1 << 0),
    SFF_DOM_FIELD_FLAG_VOLTAGE = (1 << 1),
    SFF_DOM_FIELD_FLAG_BIAS_CUR = (1 << 2),
    SFF_DOM_FIELD_FLAG_RX_POWER = (1 << 3),
    SFF_DOM_FIELD_FLAG_RX_POWER_OMA = (1 << 4),
    SFF_DOM_FIELD_FLAG_TX_POWER = (1 << 5),
} sff_dom_field_flag_t;

/** sff_dom_spec */
typedef enum sff_dom_spec_e {
    SFF_DOM_SPEC_UNSUPPORTED,
    SFF_DOM_SPEC_SFF8436,
    SFF_DOM_SPEC_SFF8472,
    SFF_DOM_SPEC_SFF8636,
    SFF_DOM_SPEC_LAST = SFF_DOM_SPEC_SFF8636,
    SFF_DOM_SPEC_COUNT,
    SFF_DOM_SPEC_INVALID = -1,
} sff_dom_spec_t;
/* <auto.end.enum(tag:dom).define> */


/**
 * The structure represents the abstract
 * diagnostic values for a given channel
 * across all specifications.
 */
typedef struct sff_dom_channel_info_s {
    /** Valid Field Flags - a bitfield of sff_dom_field_flag_t */
    uint32_t fields;

    /** Measured bias current in 2uA units */
    uint16_t bias_cur;

    /** Measured Rx Power (Avg Optical Power) */
    uint16_t rx_power;

    /** Measured RX Power (OMA) */
    uint16_t rx_power_oma;

    /** Measured TX Power (Avg Optical Power) */
    uint16_t tx_power;

} sff_dom_channel_info_t;


/**
 * All available module diagnostic information.
 */
typedef struct sff_dom_info_s {

    /** The SFF Specification from which this information was derived. */
    sff_dom_spec_t spec;

    /** Valid Field Flags - a bitfield of sff_domf_field_flag_t */
    uint32_t fields;

    /** Temp in 16-bit signed 1/256 Celsius */
    int16_t temp;

    /** Voltage in 0.1mV units */
    uint16_t voltage;

    /** Whether external calibration was enabled. */
    int extcal;

    /** Number of reporting channels. */
    int nchannels;

    /** Channel information. */
    sff_dom_channel_info_t channels[SFF_DOM_CHANNEL_COUNT_MAX];

} sff_dom_info_t;


/**
 * @brief Determine the applicable DOM specification
 * for the given module.
 * @param si The sff info structure for the given module.
 * @param a0 The A0 idprom data.
 * @param rspec Receives the specification identifier.
 */
int sff_dom_spec_get(sff_info_t* si, uint8_t* a0, sff_dom_spec_t* rspec);


/**
 * @brief Generate the DOM information from the given data.
 * @param sdi [out] Receives the DOM information.
 * @param info The SFF information structure for the module.
 * @param a0 The A0 IDPROM data.
 * @param a2 The A2 DOM data (for SFP ONLY).
 */
int sff_dom_info_get(sff_dom_info_t* sdi, sff_info_t* info, uint8_t* a0, uint8_t* a2);

int sff_dom_info_to_json(sff_dom_info_t* sdi, cJSON** cjp);

/**
 * Determines whether the DOM information is meaningful.
 */
#define SFF_DOM_INFO_VALID(_infop)              \
    ((_infop)->fields || (_infop)->channels)

#endif /* __SFF_DOM_H__ */
