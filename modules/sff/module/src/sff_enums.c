/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <sff/sff_config.h>
#include <sff/sff.h>
#include <sff/dom.h>
#include <sff/cmis.h>

/* <auto.start.enum(ALL).source> */
aim_map_si_t sff_cmis_active_cable_code_map[] =
{
    { "UNDEFINED", SFF_CMIS_ACTIVE_CABLE_CODE_UNDEFINED },
    { "BER_LESS_THAN_1X10_MINUS_12", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_1X10_MINUS_12 },
    { "BER_LESS_THAN_5X10_MINUS_5", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_5X10_MINUS_5 },
    { "BER_LESS_THAN_2P4X10_MINUS_4", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_2P4X10_MINUS_4 },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_active_cable_code_desc_map[] =
{
    { "None", SFF_CMIS_ACTIVE_CABLE_CODE_UNDEFINED },
    { "None", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_1X10_MINUS_12 },
    { "None", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_5X10_MINUS_5 },
    { "None", SFF_CMIS_ACTIVE_CABLE_CODE_BER_LESS_THAN_2P4X10_MINUS_4 },
    { NULL, 0 }
};

const char*
sff_cmis_active_cable_code_name(sff_cmis_active_cable_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_active_cable_code_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_active_cable_code'";
    }
}

int
sff_cmis_active_cable_code_value(const char* str, sff_cmis_active_cable_code_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_active_cable_code_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_active_cable_code_desc(sff_cmis_active_cable_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_active_cable_code_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_active_cable_code'";
    }
}

int
sff_cmis_active_cable_code_valid(sff_cmis_active_cable_code_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_active_cable_code_map, 0) ? 1 : 0;
}


aim_map_si_t sff_cmis_base_t_code_map[] =
{
    { "UNDEFINED", SFF_CMIS_BASE_T_CODE_UNDEFINED },
    { "1000BASE_T", SFF_CMIS_BASE_T_CODE_1000BASE_T },
    { "2_5GBASE_T", SFF_CMIS_BASE_T_CODE_2_5GBASE_T },
    { "5GBASE_T", SFF_CMIS_BASE_T_CODE_5GBASE_T },
    { "10GBASE_T", SFF_CMIS_BASE_T_CODE_10GBASE_T },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_base_t_code_desc_map[] =
{
    { "None", SFF_CMIS_BASE_T_CODE_UNDEFINED },
    { "None", SFF_CMIS_BASE_T_CODE_1000BASE_T },
    { "None", SFF_CMIS_BASE_T_CODE_2_5GBASE_T },
    { "None", SFF_CMIS_BASE_T_CODE_5GBASE_T },
    { "None", SFF_CMIS_BASE_T_CODE_10GBASE_T },
    { NULL, 0 }
};

const char*
sff_cmis_base_t_code_name(sff_cmis_base_t_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_base_t_code_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_base_t_code'";
    }
}

int
sff_cmis_base_t_code_value(const char* str, sff_cmis_base_t_code_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_base_t_code_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_base_t_code_desc(sff_cmis_base_t_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_base_t_code_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_base_t_code'";
    }
}

int
sff_cmis_base_t_code_valid(sff_cmis_base_t_code_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_base_t_code_map, 0) ? 1 : 0;
}


aim_map_si_t sff_cmis_mmf_code_map[] =
{
    { "UNDEFINED", SFF_CMIS_MMF_CODE_UNDEFINED },
    { "10GBASE_SW", SFF_CMIS_MMF_CODE_10GBASE_SW },
    { "10GBASE_SR", SFF_CMIS_MMF_CODE_10GBASE_SR },
    { "25GBASE_SR", SFF_CMIS_MMF_CODE_25GBASE_SR },
    { "40GBASE_SR4", SFF_CMIS_MMF_CODE_40GBASE_SR4 },
    { "40GE_SWDM4", SFF_CMIS_MMF_CODE_40GE_SWDM4 },
    { "40GE_BiDi", SFF_CMIS_MMF_CODE_40GE_BIDI },
    { "50GBASE_SR", SFF_CMIS_MMF_CODE_50GBASE_SR },
    { "100GBASE_SR10", SFF_CMIS_MMF_CODE_100GBASE_SR10 },
    { "100GBASE_SR4", SFF_CMIS_MMF_CODE_100GBASE_SR4 },
    { "100GE_SWDM4", SFF_CMIS_MMF_CODE_100GE_SWDM4 },
    { "100GE_BIDI", SFF_CMIS_MMF_CODE_100GE_BIDI },
    { "100GBASE_SR2", SFF_CMIS_MMF_CODE_100GBASE_SR2 },
    { "100G_SR", SFF_CMIS_MMF_CODE_100G_SR },
    { "200GBASE_SR4", SFF_CMIS_MMF_CODE_200GBASE_SR4 },
    { "400GBASE_SR16", SFF_CMIS_MMF_CODE_400GBASE_SR16 },
    { "400G_SR8", SFF_CMIS_MMF_CODE_400G_SR8 },
    { "400G_SR4", SFF_CMIS_MMF_CODE_400G_SR4 },
    { "800G-SR8", SFF_CMIS_MMF_CODE_800G_SR8 },
    { "400GE-BIDI", SFF_CMIS_MMF_CODE_400GE_BIDI },
    { "8GFC-MM", SFF_CMIS_MMF_CODE_8GFC_MM },
    { "10GFC-MM", SFF_CMIS_MMF_CODE_10GFC_MM },
    { "16GFC-MM", SFF_CMIS_MMF_CODE_16GFC_MM },
    { "32GFC-MM", SFF_CMIS_MMF_CODE_32GFC_MM },
    { "64GFC-MM", SFF_CMIS_MMF_CODE_64GFC_MM },
    { "128GFC-MM4", SFF_CMIS_MMF_CODE_128GFC_MM4 },
    { "256GFC-MM4", SFF_CMIS_MMF_CODE_256GFC_MM4 },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_mmf_code_desc_map[] =
{
    { "None", SFF_CMIS_MMF_CODE_UNDEFINED },
    { "None", SFF_CMIS_MMF_CODE_10GBASE_SW },
    { "None", SFF_CMIS_MMF_CODE_10GBASE_SR },
    { "None", SFF_CMIS_MMF_CODE_25GBASE_SR },
    { "None", SFF_CMIS_MMF_CODE_40GBASE_SR4 },
    { "None", SFF_CMIS_MMF_CODE_40GE_SWDM4 },
    { "None", SFF_CMIS_MMF_CODE_40GE_BIDI },
    { "None", SFF_CMIS_MMF_CODE_50GBASE_SR },
    { "None", SFF_CMIS_MMF_CODE_100GBASE_SR10 },
    { "None", SFF_CMIS_MMF_CODE_100GBASE_SR4 },
    { "None", SFF_CMIS_MMF_CODE_100GE_SWDM4 },
    { "None", SFF_CMIS_MMF_CODE_100GE_BIDI },
    { "None", SFF_CMIS_MMF_CODE_100GBASE_SR2 },
    { "None", SFF_CMIS_MMF_CODE_100G_SR },
    { "None", SFF_CMIS_MMF_CODE_200GBASE_SR4 },
    { "None", SFF_CMIS_MMF_CODE_400GBASE_SR16 },
    { "None", SFF_CMIS_MMF_CODE_400G_SR8 },
    { "None", SFF_CMIS_MMF_CODE_400G_SR4 },
    { "None", SFF_CMIS_MMF_CODE_800G_SR8 },
    { "None", SFF_CMIS_MMF_CODE_400GE_BIDI },
    { "None", SFF_CMIS_MMF_CODE_8GFC_MM },
    { "None", SFF_CMIS_MMF_CODE_10GFC_MM },
    { "None", SFF_CMIS_MMF_CODE_16GFC_MM },
    { "None", SFF_CMIS_MMF_CODE_32GFC_MM },
    { "None", SFF_CMIS_MMF_CODE_64GFC_MM },
    { "None", SFF_CMIS_MMF_CODE_128GFC_MM4 },
    { "None", SFF_CMIS_MMF_CODE_256GFC_MM4 },
    { NULL, 0 }
};

const char*
sff_cmis_mmf_code_name(sff_cmis_mmf_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_mmf_code_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_mmf_code'";
    }
}

int
sff_cmis_mmf_code_value(const char* str, sff_cmis_mmf_code_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_mmf_code_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_mmf_code_desc(sff_cmis_mmf_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_mmf_code_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_mmf_code'";
    }
}

int
sff_cmis_mmf_code_valid(sff_cmis_mmf_code_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_mmf_code_map, 0) ? 1 : 0;
}


aim_map_si_t sff_cmis_module_type_map[] =
{
    { "UNDEFINED", SFF_CMIS_MODULE_TYPE_UNDEFINED },
    { "OPTICAL_MMF", SFF_CMIS_MODULE_TYPE_OPTICAL_MMF },
    { "OPTICAL_SMF", SFF_CMIS_MODULE_TYPE_OPTICAL_SMF },
    { "PASSIVE_CU", SFF_CMIS_MODULE_TYPE_PASSIVE_CU },
    { "ACTIVE_CABLE", SFF_CMIS_MODULE_TYPE_ACTIVE_CABLE },
    { "BASE_T", SFF_CMIS_MODULE_TYPE_BASE_T },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_module_type_desc_map[] =
{
    { "None", SFF_CMIS_MODULE_TYPE_UNDEFINED },
    { "None", SFF_CMIS_MODULE_TYPE_OPTICAL_MMF },
    { "None", SFF_CMIS_MODULE_TYPE_OPTICAL_SMF },
    { "None", SFF_CMIS_MODULE_TYPE_PASSIVE_CU },
    { "None", SFF_CMIS_MODULE_TYPE_ACTIVE_CABLE },
    { "None", SFF_CMIS_MODULE_TYPE_BASE_T },
    { NULL, 0 }
};

const char*
sff_cmis_module_type_name(sff_cmis_module_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_module_type_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_module_type'";
    }
}

int
sff_cmis_module_type_value(const char* str, sff_cmis_module_type_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_module_type_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_module_type_desc(sff_cmis_module_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_module_type_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_module_type'";
    }
}

int
sff_cmis_module_type_valid(sff_cmis_module_type_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_module_type_map, 0) ? 1 : 0;
}


aim_map_si_t sff_cmis_passive_copper_code_map[] =
{
    { "UNDEFINED", SFF_CMIS_PASSIVE_COPPER_CODE_UNDEFINED },
    { "COPPER_CABLE", SFF_CMIS_PASSIVE_COPPER_CODE_COPPER_CABLE },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_passive_copper_code_desc_map[] =
{
    { "None", SFF_CMIS_PASSIVE_COPPER_CODE_UNDEFINED },
    { "None", SFF_CMIS_PASSIVE_COPPER_CODE_COPPER_CABLE },
    { NULL, 0 }
};

const char*
sff_cmis_passive_copper_code_name(sff_cmis_passive_copper_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_passive_copper_code_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_passive_copper_code'";
    }
}

int
sff_cmis_passive_copper_code_value(const char* str, sff_cmis_passive_copper_code_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_passive_copper_code_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_passive_copper_code_desc(sff_cmis_passive_copper_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_passive_copper_code_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_passive_copper_code'";
    }
}

int
sff_cmis_passive_copper_code_valid(sff_cmis_passive_copper_code_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_passive_copper_code_map, 0) ? 1 : 0;
}


aim_map_si_t sff_cmis_smf_code_map[] =
{
    { "UNDEFINED", SFF_CMIS_SMF_CODE_UNDEFINED },
    { "10GBASE_LW", SFF_CMIS_SMF_CODE_10GBASE_LW },
    { "10GBASE_EW", SFF_CMIS_SMF_CODE_10GBASE_EW },
    { "10G_ZW", SFF_CMIS_SMF_CODE_10G_ZW },
    { "10GBASE_LR", SFF_CMIS_SMF_CODE_10GBASE_LR },
    { "10GBASE_ER", SFF_CMIS_SMF_CODE_10GBASE_ER },
    { "10G_ZR", SFF_CMIS_SMF_CODE_10G_ZR },
    { "25GBASE_LR", SFF_CMIS_SMF_CODE_25GBASE_LR },
    { "25GBASE_ER", SFF_CMIS_SMF_CODE_25GBASE_ER },
    { "40GBASE_LR4", SFF_CMIS_SMF_CODE_40GBASE_LR4 },
    { "40GBASE_FR", SFF_CMIS_SMF_CODE_40GBASE_FR },
    { "50GBASE_FR", SFF_CMIS_SMF_CODE_50GBASE_FR },
    { "50GBASE_LR", SFF_CMIS_SMF_CODE_50GBASE_LR },
    { "100GBASE_LR4", SFF_CMIS_SMF_CODE_100GBASE_LR4 },
    { "100GBASE_ER4", SFF_CMIS_SMF_CODE_100GBASE_ER4 },
    { "100G_PSM4", SFF_CMIS_SMF_CODE_100G_PSM4 },
    { "100G_CWDM4_OCP", SFF_CMIS_SMF_CODE_100G_CWDM4_OCP },
    { "100G_CWDM4", SFF_CMIS_SMF_CODE_100G_CWDM4 },
    { "100G_4WDM_10", SFF_CMIS_SMF_CODE_100G_4WDM_10 },
    { "100G_4WDM_20", SFF_CMIS_SMF_CODE_100G_4WDM_20 },
    { "100G_4WDM_40", SFF_CMIS_SMF_CODE_100G_4WDM_40 },
    { "100GBASE_DR", SFF_CMIS_SMF_CODE_100GBASE_DR },
    { "100G_FR", SFF_CMIS_SMF_CODE_100G_FR },
    { "100G_LR", SFF_CMIS_SMF_CODE_100G_LR },
    { "200GBASE_DR4", SFF_CMIS_SMF_CODE_200GBASE_DR4 },
    { "200GBASE_FR4", SFF_CMIS_SMF_CODE_200GBASE_FR4 },
    { "200GBASE_LR4", SFF_CMIS_SMF_CODE_200GBASE_LR4 },
    { "400GBASE_FR8", SFF_CMIS_SMF_CODE_400GBASE_FR8 },
    { "400GBASE_LR8", SFF_CMIS_SMF_CODE_400GBASE_LR8 },
    { "400GBASE_DR4", SFF_CMIS_SMF_CODE_400GBASE_DR4 },
    { "400G_FR4", SFF_CMIS_SMF_CODE_400G_FR4 },
    { "400G_LR4", SFF_CMIS_SMF_CODE_400G_LR4 },
    { "8GFC_SM", SFF_CMIS_SMF_CODE_8GFC_SM },
    { "10GFC_SM", SFF_CMIS_SMF_CODE_10GFC_SM },
    { "16GFC_SM", SFF_CMIS_SMF_CODE_16GFC_SM },
    { "32GFC_SM", SFF_CMIS_SMF_CODE_32GFC_SM },
    { "64GFC_SM", SFF_CMIS_SMF_CODE_64GFC_SM },
    { "128GFC_PSM4", SFF_CMIS_SMF_CODE_128GFC_PSM4 },
    { "256GFC_PSM4", SFF_CMIS_SMF_CODE_256GFC_PSM4 },
    { "128GFC_CWDM4", SFF_CMIS_SMF_CODE_128GFC_CWDM4 },
    { "256GFC_CWDM4", SFF_CMIS_SMF_CODE_256GFC_CWDM4 },
    { "4I1_9D1F", SFF_CMIS_SMF_CODE_4I1_9D1F },
    { "4L1_9C1F", SFF_CMIS_SMF_CODE_4L1_9C1F },
    { "4L1_9D1F", SFF_CMIS_SMF_CODE_4L1_9D1F },
    { "C4S1_9D1F", SFF_CMIS_SMF_CODE_C4S1_9D1F },
    { "C4S1_4D1F", SFF_CMIS_SMF_CODE_C4S1_4D1F },
    { "4I1_4D1F", SFF_CMIS_SMF_CODE_4I1_4D1F },
    { "8R1_4D1F", SFF_CMIS_SMF_CODE_8R1_4D1F },
    { "8I1_4D1F", SFF_CMIS_SMF_CODE_8I1_4D1F },
    { "10G_SR", SFF_CMIS_SMF_CODE_10G_SR },
    { "10G_LR", SFF_CMIS_SMF_CODE_10G_LR },
    { "25G_SR", SFF_CMIS_SMF_CODE_25G_SR },
    { "25G_LR", SFF_CMIS_SMF_CODE_25G_LR },
    { "10G_LR_BIDI", SFF_CMIS_SMF_CODE_10G_LR_BIDI },
    { "25G_LR_BIDI", SFF_CMIS_SMF_CODE_25G_LR_BIDI },
    { NULL, 0 }
};

aim_map_si_t sff_cmis_smf_code_desc_map[] =
{
    { "None", SFF_CMIS_SMF_CODE_UNDEFINED },
    { "None", SFF_CMIS_SMF_CODE_10GBASE_LW },
    { "None", SFF_CMIS_SMF_CODE_10GBASE_EW },
    { "None", SFF_CMIS_SMF_CODE_10G_ZW },
    { "None", SFF_CMIS_SMF_CODE_10GBASE_LR },
    { "None", SFF_CMIS_SMF_CODE_10GBASE_ER },
    { "None", SFF_CMIS_SMF_CODE_10G_ZR },
    { "None", SFF_CMIS_SMF_CODE_25GBASE_LR },
    { "None", SFF_CMIS_SMF_CODE_25GBASE_ER },
    { "None", SFF_CMIS_SMF_CODE_40GBASE_LR4 },
    { "None", SFF_CMIS_SMF_CODE_40GBASE_FR },
    { "None", SFF_CMIS_SMF_CODE_50GBASE_FR },
    { "None", SFF_CMIS_SMF_CODE_50GBASE_LR },
    { "None", SFF_CMIS_SMF_CODE_100GBASE_LR4 },
    { "None", SFF_CMIS_SMF_CODE_100GBASE_ER4 },
    { "None", SFF_CMIS_SMF_CODE_100G_PSM4 },
    { "None", SFF_CMIS_SMF_CODE_100G_CWDM4_OCP },
    { "None", SFF_CMIS_SMF_CODE_100G_CWDM4 },
    { "None", SFF_CMIS_SMF_CODE_100G_4WDM_10 },
    { "None", SFF_CMIS_SMF_CODE_100G_4WDM_20 },
    { "None", SFF_CMIS_SMF_CODE_100G_4WDM_40 },
    { "None", SFF_CMIS_SMF_CODE_100GBASE_DR },
    { "None", SFF_CMIS_SMF_CODE_100G_FR },
    { "None", SFF_CMIS_SMF_CODE_100G_LR },
    { "None", SFF_CMIS_SMF_CODE_200GBASE_DR4 },
    { "None", SFF_CMIS_SMF_CODE_200GBASE_FR4 },
    { "None", SFF_CMIS_SMF_CODE_200GBASE_LR4 },
    { "None", SFF_CMIS_SMF_CODE_400GBASE_FR8 },
    { "None", SFF_CMIS_SMF_CODE_400GBASE_LR8 },
    { "None", SFF_CMIS_SMF_CODE_400GBASE_DR4 },
    { "None", SFF_CMIS_SMF_CODE_400G_FR4 },
    { "None", SFF_CMIS_SMF_CODE_400G_LR4 },
    { "None", SFF_CMIS_SMF_CODE_8GFC_SM },
    { "None", SFF_CMIS_SMF_CODE_10GFC_SM },
    { "None", SFF_CMIS_SMF_CODE_16GFC_SM },
    { "None", SFF_CMIS_SMF_CODE_32GFC_SM },
    { "None", SFF_CMIS_SMF_CODE_64GFC_SM },
    { "None", SFF_CMIS_SMF_CODE_128GFC_PSM4 },
    { "None", SFF_CMIS_SMF_CODE_256GFC_PSM4 },
    { "None", SFF_CMIS_SMF_CODE_128GFC_CWDM4 },
    { "None", SFF_CMIS_SMF_CODE_256GFC_CWDM4 },
    { "None", SFF_CMIS_SMF_CODE_4I1_9D1F },
    { "None", SFF_CMIS_SMF_CODE_4L1_9C1F },
    { "None", SFF_CMIS_SMF_CODE_4L1_9D1F },
    { "None", SFF_CMIS_SMF_CODE_C4S1_9D1F },
    { "None", SFF_CMIS_SMF_CODE_C4S1_4D1F },
    { "None", SFF_CMIS_SMF_CODE_4I1_4D1F },
    { "None", SFF_CMIS_SMF_CODE_8R1_4D1F },
    { "None", SFF_CMIS_SMF_CODE_8I1_4D1F },
    { "None", SFF_CMIS_SMF_CODE_10G_SR },
    { "None", SFF_CMIS_SMF_CODE_10G_LR },
    { "None", SFF_CMIS_SMF_CODE_25G_SR },
    { "None", SFF_CMIS_SMF_CODE_25G_LR },
    { "None", SFF_CMIS_SMF_CODE_10G_LR_BIDI },
    { "None", SFF_CMIS_SMF_CODE_25G_LR_BIDI },
    { NULL, 0 }
};

const char*
sff_cmis_smf_code_name(sff_cmis_smf_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_smf_code_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_smf_code'";
    }
}

int
sff_cmis_smf_code_value(const char* str, sff_cmis_smf_code_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_cmis_smf_code_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_cmis_smf_code_desc(sff_cmis_smf_code_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_cmis_smf_code_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_cmis_smf_code'";
    }
}

int
sff_cmis_smf_code_valid(sff_cmis_smf_code_t e)
{
    return aim_map_si_i(NULL, e, sff_cmis_smf_code_map, 0) ? 1 : 0;
}


aim_map_si_t sff_dom_field_flag_map[] =
{
    { "TEMP", SFF_DOM_FIELD_FLAG_TEMP },
    { "VOLTAGE", SFF_DOM_FIELD_FLAG_VOLTAGE },
    { "BIAS_CUR", SFF_DOM_FIELD_FLAG_BIAS_CUR },
    { "RX_POWER", SFF_DOM_FIELD_FLAG_RX_POWER },
    { "RX_POWER_OMA", SFF_DOM_FIELD_FLAG_RX_POWER_OMA },
    { "TX_POWER", SFF_DOM_FIELD_FLAG_TX_POWER },
    { NULL, 0 }
};

aim_map_si_t sff_dom_field_flag_desc_map[] =
{
    { "None", SFF_DOM_FIELD_FLAG_TEMP },
    { "None", SFF_DOM_FIELD_FLAG_VOLTAGE },
    { "None", SFF_DOM_FIELD_FLAG_BIAS_CUR },
    { "None", SFF_DOM_FIELD_FLAG_RX_POWER },
    { "None", SFF_DOM_FIELD_FLAG_RX_POWER_OMA },
    { "None", SFF_DOM_FIELD_FLAG_TX_POWER },
    { NULL, 0 }
};

const char*
sff_dom_field_flag_name(sff_dom_field_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_dom_field_flag_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_dom_field_flag'";
    }
}

int
sff_dom_field_flag_value(const char* str, sff_dom_field_flag_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_dom_field_flag_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_dom_field_flag_desc(sff_dom_field_flag_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_dom_field_flag_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_dom_field_flag'";
    }
}

int
sff_dom_field_flag_valid(sff_dom_field_flag_t e)
{
    return aim_map_si_i(NULL, e, sff_dom_field_flag_map, 0) ? 1 : 0;
}


aim_map_si_t sff_dom_spec_map[] =
{
    { "UNSUPPORTED", SFF_DOM_SPEC_UNSUPPORTED },
    { "SFF8436", SFF_DOM_SPEC_SFF8436 },
    { "SFF8472", SFF_DOM_SPEC_SFF8472 },
    { "SFF8636", SFF_DOM_SPEC_SFF8636 },
    { NULL, 0 }
};

aim_map_si_t sff_dom_spec_desc_map[] =
{
    { "None", SFF_DOM_SPEC_UNSUPPORTED },
    { "None", SFF_DOM_SPEC_SFF8436 },
    { "None", SFF_DOM_SPEC_SFF8472 },
    { "None", SFF_DOM_SPEC_SFF8636 },
    { NULL, 0 }
};

const char*
sff_dom_spec_name(sff_dom_spec_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_dom_spec_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_dom_spec'";
    }
}

int
sff_dom_spec_value(const char* str, sff_dom_spec_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_dom_spec_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_dom_spec_desc(sff_dom_spec_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_dom_spec_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_dom_spec'";
    }
}


aim_map_si_t sff_media_type_map[] =
{
    { "COPPER", SFF_MEDIA_TYPE_COPPER },
    { "FIBER", SFF_MEDIA_TYPE_FIBER },
    { NULL, 0 }
};

aim_map_si_t sff_media_type_desc_map[] =
{
    { "Copper", SFF_MEDIA_TYPE_COPPER },
    { "Fiber", SFF_MEDIA_TYPE_FIBER },
    { NULL, 0 }
};

const char*
sff_media_type_name(sff_media_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_media_type_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_media_type'";
    }
}

int
sff_media_type_value(const char* str, sff_media_type_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_media_type_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_media_type_desc(sff_media_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_media_type_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_media_type'";
    }
}


aim_map_si_t sff_module_caps_map[] =
{
    { "F_100", SFF_MODULE_CAPS_F_100 },
    { "F_1G", SFF_MODULE_CAPS_F_1G },
    { "F_10G", SFF_MODULE_CAPS_F_10G },
    { "F_25G", SFF_MODULE_CAPS_F_25G },
    { "F_40G", SFF_MODULE_CAPS_F_40G },
    { "F_100G", SFF_MODULE_CAPS_F_100G },
    { "F_400G", SFF_MODULE_CAPS_F_400G },
    { NULL, 0 }
};

aim_map_si_t sff_module_caps_desc_map[] =
{
    { "None", SFF_MODULE_CAPS_F_100 },
    { "None", SFF_MODULE_CAPS_F_1G },
    { "None", SFF_MODULE_CAPS_F_10G },
    { "None", SFF_MODULE_CAPS_F_25G },
    { "None", SFF_MODULE_CAPS_F_40G },
    { "None", SFF_MODULE_CAPS_F_100G },
    { "None", SFF_MODULE_CAPS_F_400G },
    { NULL, 0 }
};

const char*
sff_module_caps_name(sff_module_caps_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_module_caps_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_module_caps'";
    }
}

int
sff_module_caps_value(const char* str, sff_module_caps_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_module_caps_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_module_caps_desc(sff_module_caps_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_module_caps_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_module_caps'";
    }
}

int
sff_module_caps_valid(sff_module_caps_t e)
{
    return aim_map_si_i(NULL, e, sff_module_caps_map, 0) ? 1 : 0;
}


aim_map_si_t sff_module_type_map[] =
{
    { "400G_CR8", SFF_MODULE_TYPE_400G_CR8 },
    { "400G_SR8", SFF_MODULE_TYPE_400G_SR8 },
    { "400G_SR4", SFF_MODULE_TYPE_400G_SR4 },
    { "400G_BASE_FR8", SFF_MODULE_TYPE_400G_BASE_FR8 },
    { "400G_FR4", SFF_MODULE_TYPE_400G_FR4 },
    { "400G_BASE_LR8", SFF_MODULE_TYPE_400G_BASE_LR8 },
    { "400G_LR4", SFF_MODULE_TYPE_400G_LR4 },
    { "400G_BASE_DR4", SFF_MODULE_TYPE_400G_BASE_DR4 },
    { "400G_AOC", SFF_MODULE_TYPE_400G_AOC },
    { "400G_BIDI", SFF_MODULE_TYPE_400G_BIDI },
    { "100G_AOC", SFF_MODULE_TYPE_100G_AOC },
    { "100G_BASE_CR4", SFF_MODULE_TYPE_100G_BASE_CR4 },
    { "100G_BASE_SR4", SFF_MODULE_TYPE_100G_BASE_SR4 },
    { "100G_BASE_LR4", SFF_MODULE_TYPE_100G_BASE_LR4 },
    { "100G_BASE_ER4", SFF_MODULE_TYPE_100G_BASE_ER4 },
    { "100G_CWDM4", SFF_MODULE_TYPE_100G_CWDM4 },
    { "100G_PSM4", SFF_MODULE_TYPE_100G_PSM4 },
    { "100G_SWDM4", SFF_MODULE_TYPE_100G_SWDM4 },
    { "100G_PAM4_BIDI", SFF_MODULE_TYPE_100G_PAM4_BIDI },
    { "100G_AOC_LBER", SFF_MODULE_TYPE_100G_AOC_LBER },
    { "100G_ACC_LBER", SFF_MODULE_TYPE_100G_ACC_LBER },
    { "40G_BASE_CR4", SFF_MODULE_TYPE_40G_BASE_CR4 },
    { "40G_BASE_SR4", SFF_MODULE_TYPE_40G_BASE_SR4 },
    { "40G_BASE_LR4", SFF_MODULE_TYPE_40G_BASE_LR4 },
    { "40G_BASE_LM4", SFF_MODULE_TYPE_40G_BASE_LM4 },
    { "40G_BASE_ACTIVE", SFF_MODULE_TYPE_40G_BASE_ACTIVE },
    { "40G_BASE_CR", SFF_MODULE_TYPE_40G_BASE_CR },
    { "40G_BASE_SR2", SFF_MODULE_TYPE_40G_BASE_SR2 },
    { "40G_BASE_SM4", SFF_MODULE_TYPE_40G_BASE_SM4 },
    { "40G_BASE_ER4", SFF_MODULE_TYPE_40G_BASE_ER4 },
    { "40G_BASE_SWDM4", SFF_MODULE_TYPE_40G_BASE_SWDM4 },
    { "25G_BASE_CR", SFF_MODULE_TYPE_25G_BASE_CR },
    { "25G_BASE_SR", SFF_MODULE_TYPE_25G_BASE_SR },
    { "25G_BASE_LR", SFF_MODULE_TYPE_25G_BASE_LR },
    { "25G_BASE_AOC", SFF_MODULE_TYPE_25G_BASE_AOC },
    { "10G_BASE_SR", SFF_MODULE_TYPE_10G_BASE_SR },
    { "10G_BASE_LR", SFF_MODULE_TYPE_10G_BASE_LR },
    { "10G_BASE_LRM", SFF_MODULE_TYPE_10G_BASE_LRM },
    { "10G_BASE_ER", SFF_MODULE_TYPE_10G_BASE_ER },
    { "10G_BASE_CR", SFF_MODULE_TYPE_10G_BASE_CR },
    { "10G_BASE_SX", SFF_MODULE_TYPE_10G_BASE_SX },
    { "10G_BASE_LX", SFF_MODULE_TYPE_10G_BASE_LX },
    { "10G_BASE_ZR", SFF_MODULE_TYPE_10G_BASE_ZR },
    { "10G_BASE_SRL", SFF_MODULE_TYPE_10G_BASE_SRL },
    { "10G_BASE_T", SFF_MODULE_TYPE_10G_BASE_T },
    { "1G_BASE_SX", SFF_MODULE_TYPE_1G_BASE_SX },
    { "1G_BASE_LX", SFF_MODULE_TYPE_1G_BASE_LX },
    { "1G_BASE_ZX", SFF_MODULE_TYPE_1G_BASE_ZX },
    { "1G_BASE_CX", SFF_MODULE_TYPE_1G_BASE_CX },
    { "1G_BASE_T", SFF_MODULE_TYPE_1G_BASE_T },
    { "100_BASE_LX", SFF_MODULE_TYPE_100_BASE_LX },
    { "100_BASE_FX", SFF_MODULE_TYPE_100_BASE_FX },
    { "4X_MUX", SFF_MODULE_TYPE_4X_MUX },
    { NULL, 0 }
};

aim_map_si_t sff_module_type_desc_map[] =
{
    { "400G-CR8", SFF_MODULE_TYPE_400G_CR8 },
    { "400G-SR8", SFF_MODULE_TYPE_400G_SR8 },
    { "400G-SR4", SFF_MODULE_TYPE_400G_SR4 },
    { "400GBASE-FR8", SFF_MODULE_TYPE_400G_BASE_FR8 },
    { "400G-FR4", SFF_MODULE_TYPE_400G_FR4 },
    { "400GBASE-LR8", SFF_MODULE_TYPE_400G_BASE_LR8 },
    { "400G-LR4", SFF_MODULE_TYPE_400G_LR4 },
    { "400GBASE-DR4", SFF_MODULE_TYPE_400G_BASE_DR4 },
    { "400G-AOC", SFF_MODULE_TYPE_400G_AOC },
    { "400G-BiDi", SFF_MODULE_TYPE_400G_BIDI },
    { "100G-AOC", SFF_MODULE_TYPE_100G_AOC },
    { "100GBASE-CR4", SFF_MODULE_TYPE_100G_BASE_CR4 },
    { "100GBASE-SR4", SFF_MODULE_TYPE_100G_BASE_SR4 },
    { "100GBASE-LR4", SFF_MODULE_TYPE_100G_BASE_LR4 },
    { "100GBASE-ER4", SFF_MODULE_TYPE_100G_BASE_ER4 },
    { "100G-CWDM4", SFF_MODULE_TYPE_100G_CWDM4 },
    { "100G-PSM4", SFF_MODULE_TYPE_100G_PSM4 },
    { "100G-SWDM4", SFF_MODULE_TYPE_100G_SWDM4 },
    { "100G-PAM4-BIDI", SFF_MODULE_TYPE_100G_PAM4_BIDI },
    { "100G-AOC-LBER", SFF_MODULE_TYPE_100G_AOC_LBER },
    { "100G-ACC-LBER", SFF_MODULE_TYPE_100G_ACC_LBER },
    { "40GBASE-CR4", SFF_MODULE_TYPE_40G_BASE_CR4 },
    { "40GBASE-SR4", SFF_MODULE_TYPE_40G_BASE_SR4 },
    { "40GBASE-LR4", SFF_MODULE_TYPE_40G_BASE_LR4 },
    { "40GBASE-LM4", SFF_MODULE_TYPE_40G_BASE_LM4 },
    { "40GBASE-ACTIVE", SFF_MODULE_TYPE_40G_BASE_ACTIVE },
    { "40GBASE-CR", SFF_MODULE_TYPE_40G_BASE_CR },
    { "40GBASE-SR2", SFF_MODULE_TYPE_40G_BASE_SR2 },
    { "40GBASE-SM4", SFF_MODULE_TYPE_40G_BASE_SM4 },
    { "40GBASE-ER4", SFF_MODULE_TYPE_40G_BASE_ER4 },
    { "40GBASE-SWDM4", SFF_MODULE_TYPE_40G_BASE_SWDM4 },
    { "25GBASE-CR", SFF_MODULE_TYPE_25G_BASE_CR },
    { "25GBASE-SR", SFF_MODULE_TYPE_25G_BASE_SR },
    { "25GBASE-LR", SFF_MODULE_TYPE_25G_BASE_LR },
    { "25GBASE-AOC", SFF_MODULE_TYPE_25G_BASE_AOC },
    { "10GBASE-SR", SFF_MODULE_TYPE_10G_BASE_SR },
    { "10GBASE-LR", SFF_MODULE_TYPE_10G_BASE_LR },
    { "10GBASE-LRM", SFF_MODULE_TYPE_10G_BASE_LRM },
    { "10GBASE-ER", SFF_MODULE_TYPE_10G_BASE_ER },
    { "10GBASE-CR", SFF_MODULE_TYPE_10G_BASE_CR },
    { "10GBASE-SX", SFF_MODULE_TYPE_10G_BASE_SX },
    { "10GBASE-LX", SFF_MODULE_TYPE_10G_BASE_LX },
    { "10GBASE-ZR", SFF_MODULE_TYPE_10G_BASE_ZR },
    { "10GBASE-SRL", SFF_MODULE_TYPE_10G_BASE_SRL },
    { "10GBASE-T", SFF_MODULE_TYPE_10G_BASE_T },
    { "1GBASE-SX", SFF_MODULE_TYPE_1G_BASE_SX },
    { "1GBASE-LX", SFF_MODULE_TYPE_1G_BASE_LX },
    { "1GBASE-ZX", SFF_MODULE_TYPE_1G_BASE_ZX },
    { "1GBASE-CX", SFF_MODULE_TYPE_1G_BASE_CX },
    { "1GBASE-T", SFF_MODULE_TYPE_1G_BASE_T },
    { "100BASE-LX", SFF_MODULE_TYPE_100_BASE_LX },
    { "100BASE-FX", SFF_MODULE_TYPE_100_BASE_FX },
    { "4X-MUX", SFF_MODULE_TYPE_4X_MUX },
    { NULL, 0 }
};

const char*
sff_module_type_name(sff_module_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_module_type_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_module_type'";
    }
}

int
sff_module_type_value(const char* str, sff_module_type_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_module_type_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_module_type_desc(sff_module_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_module_type_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_module_type'";
    }
}


aim_map_si_t sff_sfp_type_map[] =
{
    { "SFP", SFF_SFP_TYPE_SFP },
    { "QSFP", SFF_SFP_TYPE_QSFP },
    { "QSFP_PLUS", SFF_SFP_TYPE_QSFP_PLUS },
    { "QSFP28", SFF_SFP_TYPE_QSFP28 },
    { "SFP28", SFF_SFP_TYPE_SFP28 },
    { "QSFP_DD", SFF_SFP_TYPE_QSFP_DD },
    { NULL, 0 }
};

aim_map_si_t sff_sfp_type_desc_map[] =
{
    { "SFP", SFF_SFP_TYPE_SFP },
    { "QSFP", SFF_SFP_TYPE_QSFP },
    { "QSFP+", SFF_SFP_TYPE_QSFP_PLUS },
    { "QSFP28", SFF_SFP_TYPE_QSFP28 },
    { "SFP28", SFF_SFP_TYPE_SFP28 },
    { "QSFP-DD", SFF_SFP_TYPE_QSFP_DD },
    { NULL, 0 }
};

const char*
sff_sfp_type_name(sff_sfp_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_sfp_type_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_sfp_type'";
    }
}

int
sff_sfp_type_value(const char* str, sff_sfp_type_t* e, int substr)
{
    int i;
    AIM_REFERENCE(substr);
    if(aim_map_si_s(&i, str, sff_sfp_type_map, 0)) {
        /* Enum Found */
        *e = i;
        return 0;
    }
    else {
        return -1;
    }
}

const char*
sff_sfp_type_desc(sff_sfp_type_t e)
{
    const char* name;
    if(aim_map_si_i(&name, e, sff_sfp_type_desc_map, 0)) {
        return name;
    }
    else {
        return "-invalid value for enum type 'sff_sfp_type'";
    }
}

/* <auto.end.enum(ALL).source> */
