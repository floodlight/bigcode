"""enums.py

Enumerations from the SFF auto.yml.
"""

class Enumeration(object):
    @classmethod
    def name(klass, value):
        for (k, v) in klass.__dict__.iteritems():
            if v == value:
                return k
        return None

# <auto.start.pyenum(ALL).define>
class SFF_CMIS_ACTIVE_CABLE_CODE(Enumeration):
    UNDEFINED = 0
    BER_LESS_THAN_1X10_MINUS_12 = 1
    BER_LESS_THAN_5X10_MINUS_5 = 2
    BER_LESS_THAN_2P4X10_MINUS_4 = 3


class SFF_CMIS_BASE_T_CODE(Enumeration):
    UNDEFINED = 0
    _1000BASE_T = 1
    _2_5GBASE_T = 2
    _5GBASE_T = 3
    _10GBASE_T = 4


class SFF_CMIS_MMF_CODE(Enumeration):
    UNDEFINED = 0
    _10GBASE_SW = 1
    _10GBASE_SR = 2
    _25GBASE_SR = 3
    _40GBASE_SR4 = 4
    _40GE_SWDM4 = 5
    _40GE_BIDI = 6
    _50GBASE_SR = 7
    _100GBASE_SR10 = 8
    _100GBASE_SR4 = 9
    _100GE_SWDM4 = 10
    _100GE_BIDI = 11
    _100GBASE_SR2 = 12
    _100G_SR = 13
    _200GBASE_SR4 = 14
    _400GBASE_SR16 = 15
    _400G_SR8 = 16
    _400G_SR4 = 17
    _800G_SR8 = 18
    _400GE_BIDI = 26
    _8GFC_MM = 19
    _10GFC_MM = 20
    _16GFC_MM = 21
    _32GFC_MM = 22
    _64GFC_MM = 23
    _128GFC_MM4 = 24
    _256GFC_MM4 = 25


class SFF_CMIS_MODULE_TYPE(Enumeration):
    UNDEFINED = 0
    OPTICAL_MMF = 1
    OPTICAL_SMF = 2
    PASSIVE_CU = 3
    ACTIVE_CABLE = 4
    BASE_T = 5


class SFF_CMIS_PASSIVE_COPPER_CODE(Enumeration):
    UNDEFINED = 0
    COPPER_CABLE = 1


class SFF_CMIS_SMF_CODE(Enumeration):
    UNDEFINED = 0
    _10GBASE_LW = 1
    _10GBASE_EW = 2
    _10G_ZW = 3
    _10GBASE_LR = 4
    _10GBASE_ER = 5
    _10G_ZR = 6
    _25GBASE_LR = 7
    _25GBASE_ER = 8
    _40GBASE_LR4 = 9
    _40GBASE_FR = 10
    _50GBASE_FR = 11
    _50GBASE_LR = 12
    _100GBASE_LR4 = 13
    _100GBASE_ER4 = 14
    _100G_PSM4 = 15
    _100G_CWDM4_OCP = 52
    _100G_CWDM4 = 16
    _100G_4WDM_10 = 17
    _100G_4WDM_20 = 18
    _100G_4WDM_40 = 19
    _100GBASE_DR = 20
    _100G_FR = 21
    _100G_LR = 22
    _200GBASE_DR4 = 23
    _200GBASE_FR4 = 24
    _200GBASE_LR4 = 25
    _400GBASE_FR8 = 26
    _400GBASE_LR8 = 27
    _400GBASE_DR4 = 28
    _400G_FR4 = 29
    _400G_LR4 = 30
    _8GFC_SM = 31
    _10GFC_SM = 32
    _16GFC_SM = 33
    _32GFC_SM = 34
    _64GFC_SM = 35
    _128GFC_PSM4 = 36
    _256GFC_PSM4 = 37
    _128GFC_CWDM4 = 38
    _256GFC_CWDM4 = 39
    _4I1_9D1F = 44
    _4L1_9C1F = 45
    _4L1_9D1F = 46
    C4S1_9D1F = 47
    C4S1_4D1F = 48
    _4I1_4D1F = 49
    _8R1_4D1F = 50
    _8I1_4D1F = 51
    _10G_SR = 56
    _10G_LR = 57
    _25G_SR = 58
    _25G_LR = 59
    _10G_LR_BIDI = 60
    _25G_LR_BIDI = 61


class SFF_DOM_FIELD_FLAG(Enumeration):
    TEMP = (1 << 0)
    VOLTAGE = (1 << 1)
    BIAS_CUR = (1 << 2)
    RX_POWER = (1 << 3)
    RX_POWER_OMA = (1 << 4)
    TX_POWER = (1 << 5)


class SFF_DOM_SPEC(Enumeration):
    UNSUPPORTED = 0
    SFF8436 = 1
    SFF8472 = 2
    SFF8636 = 3


class SFF_MEDIA_TYPE(Enumeration):
    COPPER = 0
    FIBER = 1


class SFF_MODULE_CAPS(Enumeration):
    F_100 = (1 << 0)
    F_1G = (1 << 1)
    F_10G = (1 << 2)
    F_25G = (1 << 3)
    F_40G = (1 << 4)
    F_100G = (1 << 5)
    F_400G = (1 << 6)


class SFF_MODULE_TYPE(Enumeration):
    _400G_CR8 = 0
    _400G_SR8 = 1
    _400G_SR4 = 2
    _400G_BASE_FR8 = 3
    _400G_FR4 = 4
    _400G_BASE_LR8 = 5
    _400G_LR4 = 6
    _400G_BASE_DR4 = 7
    _400G_AOC = 8
    _400G_BIDI = 9
    _100G_AOC = 10
    _100G_BASE_CR4 = 11
    _100G_BASE_SR4 = 12
    _100G_BASE_LR4 = 13
    _100G_BASE_ER4 = 14
    _100G_CWDM4 = 15
    _100G_PSM4 = 16
    _100G_SWDM4 = 17
    _100G_PAM4_BIDI = 18
    _40G_BASE_CR4 = 19
    _40G_BASE_SR4 = 20
    _40G_BASE_LR4 = 21
    _40G_BASE_LM4 = 22
    _40G_BASE_ACTIVE = 23
    _40G_BASE_CR = 24
    _40G_BASE_SR2 = 25
    _40G_BASE_SM4 = 26
    _40G_BASE_ER4 = 27
    _40G_BASE_SWDM4 = 28
    _25G_BASE_CR = 29
    _25G_BASE_SR = 30
    _25G_BASE_LR = 31
    _25G_BASE_AOC = 32
    _10G_BASE_SR = 33
    _10G_BASE_LR = 34
    _10G_BASE_LRM = 35
    _10G_BASE_ER = 36
    _10G_BASE_CR = 37
    _10G_BASE_SX = 38
    _10G_BASE_LX = 39
    _10G_BASE_ZR = 40
    _10G_BASE_SRL = 41
    _10G_BASE_T = 42
    _1G_BASE_SX = 43
    _1G_BASE_LX = 44
    _1G_BASE_ZX = 45
    _1G_BASE_CX = 46
    _1G_BASE_T = 47
    _100_BASE_LX = 48
    _100_BASE_FX = 49
    _4X_MUX = 50


class SFF_SFP_TYPE(Enumeration):
    SFP = 0
    QSFP = 1
    QSFP_PLUS = 2
    QSFP28 = 3
    SFP28 = 4
    QSFP_DD = 5

# <auto.end.pyenum(ALL).define>
