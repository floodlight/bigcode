/******************************************************************************
 *
 * This file autogenerated from ppe_utests.yml on 2013-12-06 09:07:41.657623.
 *
 * DO NOT EDIT. Changes to this file will be discarded upon regeneration.
 * 
******************************************************************************/
#include <uCli/ucli.h>
ucli_block_t ppe_utests[] = 
{
  {
    "READ_WRITE_ALL", 
    {
      "rwall", 
    },
  },
  {
    "basic", 
    {
      "data {000000000001}{000000000002}{8100}{13FE}{0806}{00112233445566778899AABBCCDDEEFF}", 
      "check ETHER_TYPE == 0x806", 
      "check 8021Q_VLAN == 0x3FE", 
      "missing IP4_VERSION", 
      "missing ICMP_TYPE", 
      "missing IP6_NEXT_HEADER", 
      "checkw ETHERNET_DST_MAC == 00.00.00.00.00.01", 
      "checkw ETHERNET_SRC_MAC == 00.00.00.00.00.02", 
    },
  },
  {
    "8021Q", 
    {
      "data {000000000001}{000000000002}{8100}{33FE}{0806}{00112233445566778899AABBCCDDEEFF}", 
      "check 8021Q_TPID == 0x8100", 
      "check 8021Q_VLAN == 0x3FE", 
      "check 8021Q_CFI == 1", 
      "check 8021Q_PRI == 1", 
      "check ETHER_TYPE == 0x806", 
    },
  },
  {
    "IP4_VERSION", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233445566778899AABBCCDDEEFF}", 
      "check IP4_VERSION == 4", 
      "missing IP6_VERSION", 
    },
  },
  {
    "IP4_SRC_ADDR", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check IP4_SRC_ADDR == 0xccddeeff", 
    },
  },
  {
    "IP4_DST_ADDR", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check IP4_DST_ADDR == 0x10160101", 
    },
  },
  {
    "IP4_PROTOCOL", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check IP4_PROTOCOL == 0x99", 
    },
  },
  {
    "TCP_L4_PORTS", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{DEAD}{BEEF}", 
      "check IP4_PROTOCOL == 06", 
      "check L4_DST_PORT == 0xBEEF", 
      "check L4_SRC_PORT == 0xDEAD", 
      "check TCP_DST_PORT == 0xBEEF", 
      "check TCP_SRC_PORT == 0xDEAD", 
    },
  },
  {
    "META_INGRESS_PORT", 
    {
      "data {000000000001}{000000000002}{8888}", 
      "set META_INGRESS_PORT 1", 
      "check META_INGRESS_PORT == 1", 
    },
  },
  {
    "META_PACKET_LENGTH", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0801}", 
      "check META_PACKET_LENGTH == 0x12", 
    },
  },
  {
    "PACKET_FORMAT_ETHERII", 
    {
      "data {000000000001}{000000000002}{0800}{45112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{DEAD}{BEEF}", 
      "checkf ETHERII", 
    },
  },
  {
    "PACKET_FORMAT_8021Q", 
    {
      "data {000000000001}{000000000002}{8100}{0111}{0800}{45112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{DEAD}{BEEF}", 
      "checkf 8021Q", 
    },
  },
  {
    "FORMAT_SET", 
    {
      "data {000000000001}{000000000002}{8100}{0111}{0800}{45112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{DEAD}{BEEF}", 
      "check 8021Q_VLAN == 0x111", 
      "check ETHER_TYPE == 0x800", 
      "check L4_DST_PORT == 0xBEEF", 
      "format ETHERII", 
      "missing 8021Q_VLAN", 
      "check ETHER_TYPE == 0x800", 
      "check L4_DST_PORT == 0xBEEF", 
      "format 8021Q", 
      "check 8021Q_VLAN == 0", 
      "set 8021Q_VLAN 0x54", 
      "check 8021Q_VLAN == 0x54", 
    },
  },
  {
    "IP4_CHECKSUM", 
    {
      "data {000000000001}{000000000002}{0800}{4500003c}{1c464000}{40060000}{ac100a63}{ac100a0c}{00000000}{00000000}", 
      "check IP4_CHECKSUM == 0", 
      "set IP4_CHECKSUM 0xAA55", 
      "update", 
      "check IP4_CHECKSUM == 0xB1E6", 
    },
  },
  {
    "SET_IP4_ADDRS", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check IP4_DST_ADDR == 0x10160101", 
      "check IP4_SRC_ADDR == 0xccddeeff", 
      "set IP4_SRC_ADDR 0xDEADBEEF", 
      "check IP4_DST_ADDR == 0x10160101", 
      "check IP4_SRC_ADDR == 0xDEADBEEF", 
      "set IP4_DST_ADDR 0xBEEFDEAD", 
      "check IP4_DST_ADDR == 0xBEEFDEAD", 
    },
  },
  {
    "TCP_CHECKSUM_BASIC", 
    {
      "data FF.FF.FF.FF.FF.FF.00.00.00.00.00.01.08.00.45.00.00.28.00.01.00.00.40.06.7C.CD.7F.00.00.01.7F.00.00.01.00.14.00.50.00.00.00.00.00.00.00.00.50.02.20.00.91.7C.00.00", 
      "check IP4_CHECKSUM == 0x7ccd", 
      "set IP4_CHECKSUM 0xA5A5", 
      "update", 
      "check IP4_CHECKSUM == 0x7ccd", 
      "check TCP_CHECKSUM == 0x917c", 
      "set TCP_CHECKSUM 0xA5A5", 
      "update", 
      "check TCP_CHECKSUM == 0x917c", 
    },
  },
  {
    "TCP_CHECKSUM_EVEN", 
    {
      "data FF.FF.FF.FF.FF.FF.00.00.00.00.00.00.08.00.45.00.00.34.00.01.00.00.40.06.7C.C1.7F.00.00.01.7F.00.00.01.00.14.00.50.00.00.00.00.00.00.00.00.50.02.20.00.FA.D4.00.00.44.45.41.44.42.45.45.46.43.41.46.45", 
      "check TCP_CHECKSUM == 0xfad4", 
      "set TCP_CHECKSUM 0xaa55", 
      "update", 
      "check TCP_CHECKSUM == 0xfad4", 
    },
  },
  {
    "TCP_CHECKSUM_ODD", 
    {
      "data FF.FF.FF.FF.FF.FF.00.00.00.00.00.00.08.00.45.00.00.35.00.01.00.00.40.06.7C.C0.7F.00.00.01.7F.00.00.01.00.14.00.50.00.00.00.00.00.00.00.00.50.02.20.00.CA.D3.00.00.44.45.41.44.42.45.45.46.43.41.46.45.30", 
      "check TCP_CHECKSUM == 0xcad3", 
      "set TCP_CHECKSUM 0xaa54", 
      "update", 
      "check TCP_CHECKSUM == 0xcad3", 
    },
  },
  {
    "TCP_CHECKSUM_EXTRA", 
    {
      "data FF.FF.FF.FF.FF.FF.00.00.00.00.00.00.08.00.45.00.00.43.00.01.00.00.40.06.7C.B2.7F.00.00.01.7F.00.00.01.00.14.00.50.00.00.00.00.00.00.00.00.50.02.20.00.B2.A8.00.00.49.4C.49.4B.45.54.4F.45.41.54.41.54.54.48.45.44.45.41.44.42.45.45.46.43.41.46.45", 
      "check TCP_CHECKSUM == 0xb2a8", 
      "set TCP_CHECKSUM 0xaa55", 
      "update", 
      "check TCP_CHECKSUM == 0xb2a8", 
    },
  },
  {
    "IP6-UDP", 
    {
      "data 33.33.00.01.00.03.00.12.3f.97.92.01.86.dd.60.00.00.00.00.25.11.01.fe.80.00.00.00.00.00.00.9c.09.b4.16.07.68.ff.42.ff.02.00.00.00.00.00.00.00.00.00.00.00.01.00.03.cf.dd.14.eb.00.25.b3.3c.77.91.00.00.00.01.00.00.00.00.00.00.0b.41.53.49.53.45.4c.46.48.4f.53.54.00.00.01.00.01", 
      "check ETHER_TYPE == 0x86dd", 
      "check IP6_VERSION == 0x6", 
      "check IP6_PAYLOAD_LENGTH == 0x25", 
      "check L4_SRC_PORT == 53213", 
      "check L4_DST_PORT == 5355", 
    },
  },
  {
    "vmware-beacon", 
    {
      "data 00.01.02.03.04.05.00.06.07.08.09.0a.89.22", 
      "check ETHER_TYPE == 0x8922", 
      "checkf ETHERII", 
    },
  },
  {
    "arp-etherii-request", 
    {
      "data 00010203040500060708090a0806000108000604000100060708090ac0a80001000000000000c0a80002", 
      "check ETHER_TYPE == 0x0806", 
      "check ARP_HTYPE == 1", 
      "check ARP_HLEN == 6", 
      "check ARP_PTYPE == 0x800", 
      "check ARP_PLEN == 4", 
      "check ARP_OPERATION == 1", 
      "check ARP_SPA == 0xc0a80001", 
      "check ARP_TPA == 0xc0a80002", 
    },
  },
  {
    "arp-802-request", 
    {
      "data 00010203040500060708090a810000010806000108000604000100060708090ac0a80001000000000000c0a80002", 
      "check ETHER_TYPE == 0x0806", 
      "check ARP_HTYPE == 1", 
      "check ARP_HLEN == 6", 
      "check ARP_PTYPE == 0x800", 
      "check ARP_PLEN == 4", 
      "check ARP_OPERATION == 1", 
      "check ARP_SPA == 0xc0a80001", 
      "check ARP_TPA == 0xc0a80002", 
      "checkw ARP_SHA == 00.06.07.08.09.0a", 
      "checkw ARP_THA == 00.00.00.00.00.00", 
    },
  },
  {
    "openflow10-header", 
    {
      "setheader OF10 {FFFFFFFFFFFF}{001122334455}{9100}{3011}{10203040}{20304050}{DEAD}{BEEF}{A5}{6A}{B4}{4B}", 
      "check OF10_TPID == 0x9100", 
      "check OF10_PRI == 0x1", 
      "check OF10_CFI == 0x1", 
      "check OF10_VLAN == 0x11", 
      "check OF10_IP4_DST_ADDR == 0x10203040", 
      "check OF10_IP4_SRC_ADDR == 0x20304050", 
      "check OF10_IP4_PROTO == 0xa5", 
      "check OF10_IP4_TOS == 0x6a", 
      "check OF10_ICMP_TYPE == 0xB4", 
      "check OF10_ICMP_CODE == 0x4B", 
      "setheader OF10 NULL", 
    },
  },
  {
    "udp-checksum", 
    {
      "data 00def01234560023456789ab080045000056000100004011f942c0a80001c0a8000210e100500042b12944444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444", 
      "check UDP_CHECKSUM == 0xb129", 
      "set UDP_CHECKSUM 0x1111", 
      "check UDP_CHECKSUM == 0x1111", 
      "update", 
      "check UDP_CHECKSUM == 0xb129", 
    },
  },
  {
    "llc", 
    {
      "data 00010203040500060708090a0003334455", 
      "check LLC_DSAP == 0x33", 
      "check LLC_SSAP == 0x44", 
      "check LLC_CONTROL == 0x55", 
    },
  },
  {
    "snap", 
    {
      "data {000102030405.00060708090a}{0007}{AA}{AA}{FF}{DEAD00}{CAFE}", 
      "check LLC_LENGTH == 0x7", 
      "check LLC_DSAP == 0xAA", 
      "check LLC_SSAP == 0xAA", 
      "check SNAP_OUI == 0xDEAD00", 
      "check SNAP_PROTOCOL == 0xCAFE;", 
      "chm LLC true", 
      "chm ETHERTYPE_MISSING true", 
    },
  },
  {
    "snap-ethertype-ip", 
    {
      "data {000000000001}{000000000002}{0100}{AB}{AA}{FF}{000000}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check LLC_LENGTH == 0x0100", 
      "check LLC_DSAP == 0xAB", 
      "check LLC_SSAP == 0xAA", 
      "check SNAP_OUI == 0", 
      "check SNAP_PROTOCOL == 0x0800", 
      "check ETHER_TYPE == 0x0800", 
      "check IP4_SRC_ADDR == 0xccddeeff", 
      "chm ETHERTYPE_MISSING false", 
    },
  },
  {
    "snap-oui-not-ethertype", 
    {
      "data {000000000001}{000000000002}{0100}{AB}{AA}{FF}{000001}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "check LLC_LENGTH == 0x0100", 
      "check LLC_DSAP == 0xAB", 
      "check LLC_SSAP == 0xAA", 
      "check SNAP_OUI == 1", 
      "check SNAP_PROTOCOL == 0x0800", 
      "missing ETHER_TYPE", 
      "missing IP4_SRC_ADDR", 
      "chm ETHERTYPE_MISSING true", 
    },
  },
  {
    "dfk1", 
    {
      "data {000000000001}{000000000002}{8100}{13FE}{0806}{00112233445566778899AABBCCDDEEFF}", 
      "dfk 8021Q_VLAN IP4_VERSION ICMP_TYPE ETHER_TYPE {03FE.00.00.0806}", 
    },
  },
  {
    "dfk2", 
    {
      "data {000000000001}{000000000002}{8100}{13FE}{0806}{00112233445566778899AABBCCDDEEFF}", 
      "dfk 8021Q_CFI ETHERNET_SRC_MAC ETHER_TYPE ETHERNET_DST_MAC {10.000000000002.0806.000000000001}", 
    },
  },
  {
    "dfk3", 
    {
      "data {000000000001}{000000000002}{0100}{AB}{AA}{FF}{000000}{0800}{45112233}{44556677}{8899AABB}{CCDDEEFF}{10160101}", 
      "dfk LLC_LENGTH SNAP_PROTOCOL IP4_SRC_ADDR LLC_DSAP {0100.0800.CCDDEEFF.AB}", 
    },
  },
  {
    "TCP_L4_PORTS_WITH_IP_OPTIONS6", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{46112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{00000000}{DEAD}{BEEF}", 
      "check IP4_PROTOCOL == 06", 
      "check L4_DST_PORT == 0xBEEF", 
      "check L4_SRC_PORT == 0xDEAD", 
      "check TCP_DST_PORT == 0xBEEF", 
      "check TCP_SRC_PORT == 0xDEAD", 
    },
  },
  {
    "TCP_L4_PORTS_WITH_IP_OPTIONS7", 
    {
      "data {000000000001}{000000000002}{8100}{03FE}{0800}{47112233}{44556677}{8806AABB}{CCDDEEFF}{10160101}{11111111}{22222222}{DEAD}{BEEF}", 
      "check IP4_PROTOCOL == 06", 
      "check L4_DST_PORT == 0xBEEF", 
      "check L4_SRC_PORT == 0xDEAD", 
      "check TCP_DST_PORT == 0xBEEF", 
      "check TCP_SRC_PORT == 0xDEAD", 
    },
  },
  {
    "LACP1", 
    {
      "data 0180C2000002000E8316F5108809010101148000000E8316F500000D800000193C000000021480000013C4120F00000D800000160D0000000310800000000000000000000000000001020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", 
      "check ETHER_TYPE == 0x8809", 
      "chm SLOW_PROTOCOLS true", 
      "chm LACP true", 
      "check SLOW_PROTOCOLS_SUBTYPE == 0x1", 
      "check LACP_VERSION == 0x1", 
      "check LACP_ACTOR_INFO == 0x1", 
      "check LACP_ACTOR_INFO_LEN == 0x14", 
      "check LACP_ACTOR_SYS_PRI == 0x8000", 
      "checkw LACP_ACTOR_SYS == 00.0e.83.16.f5.00", 
      "check LACP_ACTOR_KEY == 0xd", 
      "check LACP_ACTOR_PORT_PRI == 0x8000", 
      "check LACP_ACTOR_PORT == 0x19", 
      "check LACP_ACTOR_STATE == 0x3c", 
      "check LACP_RSV0 == 0x0", 
      "check LACP_PARTNER_INFO == 0x2", 
      "check LACP_PARTNER_INFO_LEN == 0x14", 
      "check LACP_PARTNER_SYS_PRI == 0x8000", 
      "checkw LACP_PARTNER_SYS == 00.13.c4.12.0f.00", 
      "check LACP_PARTNER_KEY == 0xd", 
      "check LACP_PARTNER_PORT_PRI == 0x8000", 
      "check LACP_PARTNER_PORT == 0x16", 
      "check LACP_PARTNER_STATE == 0xd", 
      "check LACP_RSV1 == 0x0", 
      "check LACP_COLLECTOR_INFO == 0x3", 
      "check LACP_COLLECTOR_INFO_LEN == 0x10", 
      "check LACP_COLLECTOR_MAX_DELAY == 0x8000", 
      "check LACP_TERMINATOR_INFO == 0x1", 
      "check LACP_TERMINATOR_LENGTH == 0x2", 
      "set LACP_TERMINATOR_LENGTH 0x3", 
      "check LACP_TERMINATOR_LENGTH == 0x3", 
      "set LACP_ACTOR_PORT_PRI 0xDEAD", 
      "check LACP_ACTOR_PORT_PRI == 0xDEAD", 
    },
  },
  {
    "LLDP1", 
    {
      "data 0180C200000E000130F9ADA088CC020704000130F9ADA0040405312F3106020078081753756D6D69743330302D34382D506F72742031303031000A0D53756D6D69743330302D3438000C4C53756D6D69743330302D3438202D2056657273696F6E20372E34652E3120284275696C642035292062792052656C656173655F4D61737465722030352F32372F30352030343A35333A3131000E0400140014100E0706000130F9ADA002000003E900FE0700120F02070100FE0900120F01036C000010FE0900120F030100000000FE0600120F0405F2FE060080C20101E8FE070080C202010000FE170080C20301E81076322D303438382D30332D3035303500FE050080C204000000", 
      "chm LLDP true", 
      "check ETHER_TYPE == 0x88CC", 
    },
  },
  {
    "ICMP_ECHO_REQUEST", 
    {
      "data 005056e01449000c29340bde08004500003cd743000080012b73c0a89e8bae892a4d08002a5c020021006162636465666768696a6b6c6d6e6f7071727374757677616263646566676869", 
      "chm ICMP true", 
      "check ICMP_TYPE == 8", 
      "check ICMP_CODE == 0", 
      "check ICMP_CHECKSUM == 0x2a5c", 
      "check ICMP_HEADER_DATA == 0x02002100", 
      "check ICMP_PAYLOAD == 0x61", 
    },
  },
  {
    "ICMP_ECHO_REPLY", 
    {
      "data 000c29340bde005056e0144908004500003c76e1000080018bd5ae892a4dc0a89e8b0000325c020021006162636465666768696a6b6c6d6e6f7071727374757677616263646566676869", 
      "chm ICMP true", 
      "check ICMP_TYPE == 0", 
      "check ICMP_CODE == 0", 
      "check ICMP_CHECKSUM == 0x325c", 
      "check ICMP_HEADER_DATA == 0x02002100", 
      "check ICMP_PAYLOAD == 0x61", 
    },
  },
  {
    "DHCP1", 
    {
      "data ffffffffffff000b8201fc4208004500012ca8360000fa11178b00000000ffffffff004400430118591f0101060000003d1d0000000000000000000000000000000000000000000b8201fc4200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000638253633501013d0701000b8201fc4232040000000037040103062aff00000000000000", 
      "chm DHCP true", 
      "check DHCP_OPCODE == 0x1", 
      "check DHCP_HTYPE == 0x1", 
      "check DHCP_HLEN == 0x6", 
      "check DHCP_XID == 0x3d1d", 
      "checkw DHCP_CHADDR == 000b8201fc4200000000000000000000", 
      "check DHCP_MAGIC == 0x63825363", 
      "check DHCP_OPTIONS == 53", 
    },
  },
  {
    "DHCP2", 
    {
      "data 000b8201fc42000874adf19b0800450001480445000080110000c0a80001c0a8000a00430044013422330201060000003d1d0000000000000000c0a8000ac0a8000100000000000b8201fc4200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000638253633501020104ffffff003a04000007083b0400000c4e330400000e103604c0a80001ff0000000000000000000000000000000000000000000000000000", 
      "check DHCP_OPCODE == 0x2", 
      "check DHCP_HTYPE == 1", 
      "check DHCP_HLEN == 6", 
      "check DHCP_CIADDR == 0", 
      "check DHCP_YIADDR == 0xc0a8000a", 
      "check DHCP_SIADDR == 0xc0a80001", 
      "check DHCP_GIADDR == 0", 
      "checkw DHCP_CHADDR == 000b8201fc4200000000000000000000", 
    },
  },
  { (void*)0 }
};
int ppe_utests_count = sizeof(ppe_utests)/sizeof(ppe_utests[0]); 

