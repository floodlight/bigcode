#include <sff/cmis.h>

int
sff_cmis_cable_assembly_length(const uint8_t* eeprom)
{
    /* Bit 7~6 is the multiplier */
    int multiplier = (eeprom[202] >> 6) & 0x3;
    int value = eeprom[202] & 0x3f;

    switch(multiplier)
        {
        case 0: return value / 10;
        case 1: return value * 1;
        case 2: return value * 10;
        case 3: return value * 100;
        }

    return -1;
}

static sff_module_type_t
mmf_module_type_get__(const uint8_t* eeprom)
{
    /* First Advertised Module Media Interface Code */
    switch(eeprom[87])
        {
        case SFF_CMIS_MMF_CODE_400G_SR8:
            return SFF_MODULE_TYPE_400G_SR8;

        case SFF_CMIS_MMF_CODE_400G_SR4:
            return SFF_MODULE_TYPE_400G_SR4;

        case SFF_CMIS_MMF_CODE_400GE_BIDI:
            return SFF_MODULE_TYPE_400G_BIDI;

        default:
            return SFF_MODULE_TYPE_INVALID;
        }
}

static sff_module_type_t
smf_module_type_get__(const uint8_t* eeprom)
{
    /* First Advertised Module Media Interface Code */
    switch(eeprom[87])
        {
        case SFF_CMIS_SMF_CODE_400GBASE_FR8:
            return SFF_MODULE_TYPE_400G_BASE_FR8;

        case SFF_CMIS_SMF_CODE_400GBASE_LR8:
            return SFF_MODULE_TYPE_400G_BASE_LR8;

        case SFF_CMIS_SMF_CODE_400GBASE_DR4:
            return SFF_MODULE_TYPE_400G_BASE_DR4;

        case SFF_CMIS_SMF_CODE_400G_FR4:
            return SFF_MODULE_TYPE_400G_FR4;

        case SFF_CMIS_SMF_CODE_400G_LR4:
            return SFF_MODULE_TYPE_400G_LR4;

        default:
            return SFF_MODULE_TYPE_INVALID;
        }

}

static sff_module_type_t
passive_cu_module_type_get__(const uint8_t* eeprom)
{
    /* Fixme */
    return SFF_MODULE_TYPE_400G_CR8;
}

static sff_module_type_t
active_cable_type_get__(const uint8_t* eeprom)
{
    /* Fixme */
    return SFF_MODULE_TYPE_400G_AOC;
}

static sff_module_type_t
base_t_type_get__(const uint8_t* eeprom)
{
    /* Fixme */
    return SFF_MODULE_TYPE_INVALID;
}

sff_module_type_t
sff_cmis_module_type_get(const uint8_t* eeprom)
{
    if(!SFF_CMIS_MODULE_IS_QSFP_DD(eeprom)) {
        return -1;
    }

    /*
     * Module Type Byte 85
     */
    switch(eeprom[85])
        {
        case SFF_CMIS_MODULE_TYPE_UNDEFINED:
            return SFF_MODULE_TYPE_INVALID;

        case SFF_CMIS_MODULE_TYPE_OPTICAL_MMF:
            return mmf_module_type_get__(eeprom);

        case SFF_CMIS_MODULE_TYPE_OPTICAL_SMF:
            return smf_module_type_get__(eeprom);

        case SFF_CMIS_MODULE_TYPE_PASSIVE_CU:
            return passive_cu_module_type_get__(eeprom);

        case SFF_CMIS_MODULE_TYPE_ACTIVE_CABLE:
            return active_cable_type_get__(eeprom);

        case SFF_CMIS_MODULE_TYPE_BASE_T:
            return base_t_type_get__(eeprom);

        default:
            return SFF_MODULE_TYPE_INVALID;
        }
}
