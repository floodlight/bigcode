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
    F_100 = 1
    F_1G = 2
    F_10G = 4
    F_25G = 8
    F_40G = 16
    F_100G = 32


class SFF_MODULE_TYPE(Enumeration):
    _100G_AOC = 0
    _100G_BASE_CR4 = 1
    _100G_BASE_SR4 = 2
    _100G_BASE_LR4 = 3
    _100G_BASE_ER4 = 4
    _100G_CWDM4 = 5
    _100G_PSM4 = 6
    _100G_SWDM4 = 7
    _100G_PAM4_BIDI = 8
    _40G_BASE_CR4 = 9
    _40G_BASE_SR4 = 10
    _40G_BASE_LR4 = 11
    _40G_BASE_LM4 = 12
    _40G_BASE_ACTIVE = 13
    _40G_BASE_CR = 14
    _40G_BASE_SR2 = 15
    _40G_BASE_SM4 = 16
    _40G_BASE_ER4 = 17
    _40G_BASE_SWDM4 = 18
    _25G_BASE_CR = 19
    _25G_BASE_SR = 20
    _25G_BASE_LR = 21
    _25G_BASE_AOC = 22
    _10G_BASE_SR = 23
    _10G_BASE_LR = 24
    _10G_BASE_LRM = 25
    _10G_BASE_ER = 26
    _10G_BASE_CR = 27
    _10G_BASE_SX = 28
    _10G_BASE_LX = 29
    _10G_BASE_ZR = 30
    _10G_BASE_SRL = 31
    _10G_BASE_T = 32
    _1G_BASE_SX = 33
    _1G_BASE_LX = 34
    _1G_BASE_ZX = 35
    _1G_BASE_CX = 36
    _1G_BASE_T = 37
    _100_BASE_LX = 38
    _100_BASE_FX = 39
    _4X_MUX = 40


class SFF_SFP_TYPE(Enumeration):
    SFP = 0
    QSFP = 1
    QSFP_PLUS = 2
    QSFP28 = 3
    SFP28 = 4

# <auto.end.pyenum(ALL).define>
