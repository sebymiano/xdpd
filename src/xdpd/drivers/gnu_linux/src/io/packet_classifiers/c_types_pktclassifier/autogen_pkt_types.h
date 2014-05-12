/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PKT_TYPES_H
#define PKT_TYPES_H

//This file has been autogenerated. DO NOT modify it

typedef enum protocol_types{
	PT_PROTO_ETHERNET,
	PT_PROTO_8023,
	PT_PROTO_MPLS,
	PT_PROTO_VLAN,
	PT_PROTO_PPPOE,
	PT_PROTO_PPP,
	PT_PROTO_ARPV4,
	PT_PROTO_IPV4,
	PT_PROTO_ICMPV4,
	PT_PROTO_IPV6,
	PT_PROTO_ICMPV6,
	PT_PROTO_ICMPV6_OPTS,
	PT_PROTO_ICMPV6_OPTS_LLADR_SRC,
	PT_PROTO_ICMPV6_OPTS_LLADR_TGT,
	PT_PROTO_ICMPV6_OPTS_PREFIX_INFO,
	PT_PROTO_TCP,
	PT_PROTO_UDP,
	PT_PROTO_SCTP,
	PT_PROTO_GTPU,
	PT_PROTO_MAX__
}protocol_types_t;

typedef enum pkt_types{
	PT_INVALID = -1,
	PT_ETHERNET,
	PT_8023,
	PT_ETHERNET_VLAN,
	PT_8023_VLAN,
	PT_ETHERNET_VLAN_VLAN,
	PT_8023_VLAN_VLAN,
	PT_ETHERNET_MPLS_nlabels_1,
	PT_ETHERNET_MPLS_nlabels_2,
	PT_ETHERNET_MPLS_nlabels_3,
	PT_ETHERNET_MPLS_nlabels_4,
	PT_ETHERNET_MPLS_nlabels_5,
	PT_ETHERNET_MPLS_nlabels_6,
	PT_ETHERNET_MPLS_nlabels_7,
	PT_ETHERNET_MPLS_nlabels_8,
	PT_ETHERNET_MPLS_nlabels_9,
	PT_ETHERNET_MPLS_nlabels_10,
	PT_ETHERNET_MPLS_nlabels_11,
	PT_ETHERNET_MPLS_nlabels_12,
	PT_ETHERNET_MPLS_nlabels_13,
	PT_ETHERNET_MPLS_nlabels_14,
	PT_ETHERNET_MPLS_nlabels_15,
	PT_ETHERNET_MPLS_nlabels_16,
	PT_8023_MPLS_nlabels_1,
	PT_8023_MPLS_nlabels_2,
	PT_8023_MPLS_nlabels_3,
	PT_8023_MPLS_nlabels_4,
	PT_8023_MPLS_nlabels_5,
	PT_8023_MPLS_nlabels_6,
	PT_8023_MPLS_nlabels_7,
	PT_8023_MPLS_nlabels_8,
	PT_8023_MPLS_nlabels_9,
	PT_8023_MPLS_nlabels_10,
	PT_8023_MPLS_nlabels_11,
	PT_8023_MPLS_nlabels_12,
	PT_8023_MPLS_nlabels_13,
	PT_8023_MPLS_nlabels_14,
	PT_8023_MPLS_nlabels_15,
	PT_8023_MPLS_nlabels_16,
	PT_ETHERNET_VLAN_MPLS_nlabels_1,
	PT_ETHERNET_VLAN_MPLS_nlabels_2,
	PT_ETHERNET_VLAN_MPLS_nlabels_3,
	PT_ETHERNET_VLAN_MPLS_nlabels_4,
	PT_ETHERNET_VLAN_MPLS_nlabels_5,
	PT_ETHERNET_VLAN_MPLS_nlabels_6,
	PT_ETHERNET_VLAN_MPLS_nlabels_7,
	PT_ETHERNET_VLAN_MPLS_nlabels_8,
	PT_ETHERNET_VLAN_MPLS_nlabels_9,
	PT_ETHERNET_VLAN_MPLS_nlabels_10,
	PT_ETHERNET_VLAN_MPLS_nlabels_11,
	PT_ETHERNET_VLAN_MPLS_nlabels_12,
	PT_ETHERNET_VLAN_MPLS_nlabels_13,
	PT_ETHERNET_VLAN_MPLS_nlabels_14,
	PT_ETHERNET_VLAN_MPLS_nlabels_15,
	PT_ETHERNET_VLAN_MPLS_nlabels_16,
	PT_8023_VLAN_MPLS_nlabels_1,
	PT_8023_VLAN_MPLS_nlabels_2,
	PT_8023_VLAN_MPLS_nlabels_3,
	PT_8023_VLAN_MPLS_nlabels_4,
	PT_8023_VLAN_MPLS_nlabels_5,
	PT_8023_VLAN_MPLS_nlabels_6,
	PT_8023_VLAN_MPLS_nlabels_7,
	PT_8023_VLAN_MPLS_nlabels_8,
	PT_8023_VLAN_MPLS_nlabels_9,
	PT_8023_VLAN_MPLS_nlabels_10,
	PT_8023_VLAN_MPLS_nlabels_11,
	PT_8023_VLAN_MPLS_nlabels_12,
	PT_8023_VLAN_MPLS_nlabels_13,
	PT_8023_VLAN_MPLS_nlabels_14,
	PT_8023_VLAN_MPLS_nlabels_15,
	PT_8023_VLAN_MPLS_nlabels_16,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_1,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_2,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_3,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_4,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_5,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_6,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_7,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_8,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_9,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_10,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_11,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_12,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_13,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_14,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_15,
	PT_ETHERNET_VLAN_VLAN_MPLS_nlabels_16,
	PT_8023_VLAN_VLAN_MPLS_nlabels_1,
	PT_8023_VLAN_VLAN_MPLS_nlabels_2,
	PT_8023_VLAN_VLAN_MPLS_nlabels_3,
	PT_8023_VLAN_VLAN_MPLS_nlabels_4,
	PT_8023_VLAN_VLAN_MPLS_nlabels_5,
	PT_8023_VLAN_VLAN_MPLS_nlabels_6,
	PT_8023_VLAN_VLAN_MPLS_nlabels_7,
	PT_8023_VLAN_VLAN_MPLS_nlabels_8,
	PT_8023_VLAN_VLAN_MPLS_nlabels_9,
	PT_8023_VLAN_VLAN_MPLS_nlabels_10,
	PT_8023_VLAN_VLAN_MPLS_nlabels_11,
	PT_8023_VLAN_VLAN_MPLS_nlabels_12,
	PT_8023_VLAN_VLAN_MPLS_nlabels_13,
	PT_8023_VLAN_VLAN_MPLS_nlabels_14,
	PT_8023_VLAN_VLAN_MPLS_nlabels_15,
	PT_8023_VLAN_VLAN_MPLS_nlabels_16,
	PT_ETHERNET_PPPOE,
	PT_8023_PPPOE,
	PT_ETHERNET_VLAN_PPPOE,
	PT_8023_VLAN_PPPOE,
	PT_ETHERNET_VLAN_VLAN_PPPOE,
	PT_8023_VLAN_VLAN_PPPOE,
	PT_ETHERNET_PPPOE_PPP,
	PT_8023_PPPOE_PPP,
	PT_ETHERNET_VLAN_PPPOE_PPP,
	PT_8023_VLAN_PPPOE_PPP,
	PT_ETHERNET_VLAN_VLAN_PPPOE_PPP,
	PT_8023_VLAN_VLAN_PPPOE_PPP,
	PT_ETHERNET_ARPV4,
	PT_8023_ARPV4,
	PT_ETHERNET_VLAN_ARPV4,
	PT_8023_VLAN_ARPV4,
	PT_ETHERNET_VLAN_VLAN_ARPV4,
	PT_8023_VLAN_VLAN_ARPV4,
	PT_ETHERNET_IPV4_noptions_0,
	PT_ETHERNET_IPV4_noptions_1,
	PT_ETHERNET_IPV4_noptions_2,
	PT_ETHERNET_IPV4_noptions_3,
	PT_ETHERNET_IPV4_noptions_4,
	PT_ETHERNET_IPV4_noptions_5,
	PT_ETHERNET_IPV4_noptions_6,
	PT_ETHERNET_IPV4_noptions_7,
	PT_ETHERNET_IPV4_noptions_8,
	PT_ETHERNET_IPV4_noptions_9,
	PT_ETHERNET_IPV4_noptions_10,
	PT_ETHERNET_IPV4_noptions_11,
	PT_ETHERNET_IPV4_noptions_12,
	PT_ETHERNET_IPV4_noptions_13,
	PT_ETHERNET_IPV4_noptions_14,
	PT_ETHERNET_IPV4_noptions_15,
	PT_8023_IPV4_noptions_0,
	PT_8023_IPV4_noptions_1,
	PT_8023_IPV4_noptions_2,
	PT_8023_IPV4_noptions_3,
	PT_8023_IPV4_noptions_4,
	PT_8023_IPV4_noptions_5,
	PT_8023_IPV4_noptions_6,
	PT_8023_IPV4_noptions_7,
	PT_8023_IPV4_noptions_8,
	PT_8023_IPV4_noptions_9,
	PT_8023_IPV4_noptions_10,
	PT_8023_IPV4_noptions_11,
	PT_8023_IPV4_noptions_12,
	PT_8023_IPV4_noptions_13,
	PT_8023_IPV4_noptions_14,
	PT_8023_IPV4_noptions_15,
	PT_ETHERNET_VLAN_IPV4_noptions_0,
	PT_ETHERNET_VLAN_IPV4_noptions_1,
	PT_ETHERNET_VLAN_IPV4_noptions_2,
	PT_ETHERNET_VLAN_IPV4_noptions_3,
	PT_ETHERNET_VLAN_IPV4_noptions_4,
	PT_ETHERNET_VLAN_IPV4_noptions_5,
	PT_ETHERNET_VLAN_IPV4_noptions_6,
	PT_ETHERNET_VLAN_IPV4_noptions_7,
	PT_ETHERNET_VLAN_IPV4_noptions_8,
	PT_ETHERNET_VLAN_IPV4_noptions_9,
	PT_ETHERNET_VLAN_IPV4_noptions_10,
	PT_ETHERNET_VLAN_IPV4_noptions_11,
	PT_ETHERNET_VLAN_IPV4_noptions_12,
	PT_ETHERNET_VLAN_IPV4_noptions_13,
	PT_ETHERNET_VLAN_IPV4_noptions_14,
	PT_ETHERNET_VLAN_IPV4_noptions_15,
	PT_8023_VLAN_IPV4_noptions_0,
	PT_8023_VLAN_IPV4_noptions_1,
	PT_8023_VLAN_IPV4_noptions_2,
	PT_8023_VLAN_IPV4_noptions_3,
	PT_8023_VLAN_IPV4_noptions_4,
	PT_8023_VLAN_IPV4_noptions_5,
	PT_8023_VLAN_IPV4_noptions_6,
	PT_8023_VLAN_IPV4_noptions_7,
	PT_8023_VLAN_IPV4_noptions_8,
	PT_8023_VLAN_IPV4_noptions_9,
	PT_8023_VLAN_IPV4_noptions_10,
	PT_8023_VLAN_IPV4_noptions_11,
	PT_8023_VLAN_IPV4_noptions_12,
	PT_8023_VLAN_IPV4_noptions_13,
	PT_8023_VLAN_IPV4_noptions_14,
	PT_8023_VLAN_IPV4_noptions_15,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15,
	PT_8023_VLAN_VLAN_IPV4_noptions_0,
	PT_8023_VLAN_VLAN_IPV4_noptions_1,
	PT_8023_VLAN_VLAN_IPV4_noptions_2,
	PT_8023_VLAN_VLAN_IPV4_noptions_3,
	PT_8023_VLAN_VLAN_IPV4_noptions_4,
	PT_8023_VLAN_VLAN_IPV4_noptions_5,
	PT_8023_VLAN_VLAN_IPV4_noptions_6,
	PT_8023_VLAN_VLAN_IPV4_noptions_7,
	PT_8023_VLAN_VLAN_IPV4_noptions_8,
	PT_8023_VLAN_VLAN_IPV4_noptions_9,
	PT_8023_VLAN_VLAN_IPV4_noptions_10,
	PT_8023_VLAN_VLAN_IPV4_noptions_11,
	PT_8023_VLAN_VLAN_IPV4_noptions_12,
	PT_8023_VLAN_VLAN_IPV4_noptions_13,
	PT_8023_VLAN_VLAN_IPV4_noptions_14,
	PT_8023_VLAN_VLAN_IPV4_noptions_15,
	PT_ETHERNET_IPV4_noptions_0_ICMPV4,
	PT_ETHERNET_IPV4_noptions_1_ICMPV4,
	PT_ETHERNET_IPV4_noptions_2_ICMPV4,
	PT_ETHERNET_IPV4_noptions_3_ICMPV4,
	PT_ETHERNET_IPV4_noptions_4_ICMPV4,
	PT_ETHERNET_IPV4_noptions_5_ICMPV4,
	PT_ETHERNET_IPV4_noptions_6_ICMPV4,
	PT_ETHERNET_IPV4_noptions_7_ICMPV4,
	PT_ETHERNET_IPV4_noptions_8_ICMPV4,
	PT_ETHERNET_IPV4_noptions_9_ICMPV4,
	PT_ETHERNET_IPV4_noptions_10_ICMPV4,
	PT_ETHERNET_IPV4_noptions_11_ICMPV4,
	PT_ETHERNET_IPV4_noptions_12_ICMPV4,
	PT_ETHERNET_IPV4_noptions_13_ICMPV4,
	PT_ETHERNET_IPV4_noptions_14_ICMPV4,
	PT_ETHERNET_IPV4_noptions_15_ICMPV4,
	PT_8023_IPV4_noptions_0_ICMPV4,
	PT_8023_IPV4_noptions_1_ICMPV4,
	PT_8023_IPV4_noptions_2_ICMPV4,
	PT_8023_IPV4_noptions_3_ICMPV4,
	PT_8023_IPV4_noptions_4_ICMPV4,
	PT_8023_IPV4_noptions_5_ICMPV4,
	PT_8023_IPV4_noptions_6_ICMPV4,
	PT_8023_IPV4_noptions_7_ICMPV4,
	PT_8023_IPV4_noptions_8_ICMPV4,
	PT_8023_IPV4_noptions_9_ICMPV4,
	PT_8023_IPV4_noptions_10_ICMPV4,
	PT_8023_IPV4_noptions_11_ICMPV4,
	PT_8023_IPV4_noptions_12_ICMPV4,
	PT_8023_IPV4_noptions_13_ICMPV4,
	PT_8023_IPV4_noptions_14_ICMPV4,
	PT_8023_IPV4_noptions_15_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_0_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_1_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_2_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_3_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_4_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_5_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_6_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_7_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_8_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_9_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_10_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_11_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_12_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_13_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_14_ICMPV4,
	PT_ETHERNET_VLAN_IPV4_noptions_15_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_0_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_1_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_2_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_3_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_4_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_5_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_6_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_7_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_8_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_9_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_10_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_11_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_12_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_13_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_14_ICMPV4,
	PT_8023_VLAN_IPV4_noptions_15_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14_ICMPV4,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_0_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_1_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_2_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_3_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_4_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_5_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_6_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_7_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_8_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_9_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_10_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_11_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_12_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_13_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_14_ICMPV4,
	PT_8023_VLAN_VLAN_IPV4_noptions_15_ICMPV4,
	PT_ETHERNET_IPV4_noptions_0_TCP,
	PT_ETHERNET_IPV4_noptions_1_TCP,
	PT_ETHERNET_IPV4_noptions_2_TCP,
	PT_ETHERNET_IPV4_noptions_3_TCP,
	PT_ETHERNET_IPV4_noptions_4_TCP,
	PT_ETHERNET_IPV4_noptions_5_TCP,
	PT_ETHERNET_IPV4_noptions_6_TCP,
	PT_ETHERNET_IPV4_noptions_7_TCP,
	PT_ETHERNET_IPV4_noptions_8_TCP,
	PT_ETHERNET_IPV4_noptions_9_TCP,
	PT_ETHERNET_IPV4_noptions_10_TCP,
	PT_ETHERNET_IPV4_noptions_11_TCP,
	PT_ETHERNET_IPV4_noptions_12_TCP,
	PT_ETHERNET_IPV4_noptions_13_TCP,
	PT_ETHERNET_IPV4_noptions_14_TCP,
	PT_ETHERNET_IPV4_noptions_15_TCP,
	PT_8023_IPV4_noptions_0_TCP,
	PT_8023_IPV4_noptions_1_TCP,
	PT_8023_IPV4_noptions_2_TCP,
	PT_8023_IPV4_noptions_3_TCP,
	PT_8023_IPV4_noptions_4_TCP,
	PT_8023_IPV4_noptions_5_TCP,
	PT_8023_IPV4_noptions_6_TCP,
	PT_8023_IPV4_noptions_7_TCP,
	PT_8023_IPV4_noptions_8_TCP,
	PT_8023_IPV4_noptions_9_TCP,
	PT_8023_IPV4_noptions_10_TCP,
	PT_8023_IPV4_noptions_11_TCP,
	PT_8023_IPV4_noptions_12_TCP,
	PT_8023_IPV4_noptions_13_TCP,
	PT_8023_IPV4_noptions_14_TCP,
	PT_8023_IPV4_noptions_15_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_0_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_1_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_2_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_3_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_4_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_5_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_6_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_7_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_8_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_9_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_10_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_11_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_12_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_13_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_14_TCP,
	PT_ETHERNET_VLAN_IPV4_noptions_15_TCP,
	PT_8023_VLAN_IPV4_noptions_0_TCP,
	PT_8023_VLAN_IPV4_noptions_1_TCP,
	PT_8023_VLAN_IPV4_noptions_2_TCP,
	PT_8023_VLAN_IPV4_noptions_3_TCP,
	PT_8023_VLAN_IPV4_noptions_4_TCP,
	PT_8023_VLAN_IPV4_noptions_5_TCP,
	PT_8023_VLAN_IPV4_noptions_6_TCP,
	PT_8023_VLAN_IPV4_noptions_7_TCP,
	PT_8023_VLAN_IPV4_noptions_8_TCP,
	PT_8023_VLAN_IPV4_noptions_9_TCP,
	PT_8023_VLAN_IPV4_noptions_10_TCP,
	PT_8023_VLAN_IPV4_noptions_11_TCP,
	PT_8023_VLAN_IPV4_noptions_12_TCP,
	PT_8023_VLAN_IPV4_noptions_13_TCP,
	PT_8023_VLAN_IPV4_noptions_14_TCP,
	PT_8023_VLAN_IPV4_noptions_15_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_0_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_1_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_2_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_3_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_4_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_5_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_6_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_7_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_8_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_9_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_10_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_11_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_12_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_13_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_14_TCP,
	PT_8023_VLAN_VLAN_IPV4_noptions_15_TCP,
	PT_ETHERNET_IPV4_noptions_0_UDP,
	PT_ETHERNET_IPV4_noptions_1_UDP,
	PT_ETHERNET_IPV4_noptions_2_UDP,
	PT_ETHERNET_IPV4_noptions_3_UDP,
	PT_ETHERNET_IPV4_noptions_4_UDP,
	PT_ETHERNET_IPV4_noptions_5_UDP,
	PT_ETHERNET_IPV4_noptions_6_UDP,
	PT_ETHERNET_IPV4_noptions_7_UDP,
	PT_ETHERNET_IPV4_noptions_8_UDP,
	PT_ETHERNET_IPV4_noptions_9_UDP,
	PT_ETHERNET_IPV4_noptions_10_UDP,
	PT_ETHERNET_IPV4_noptions_11_UDP,
	PT_ETHERNET_IPV4_noptions_12_UDP,
	PT_ETHERNET_IPV4_noptions_13_UDP,
	PT_ETHERNET_IPV4_noptions_14_UDP,
	PT_ETHERNET_IPV4_noptions_15_UDP,
	PT_8023_IPV4_noptions_0_UDP,
	PT_8023_IPV4_noptions_1_UDP,
	PT_8023_IPV4_noptions_2_UDP,
	PT_8023_IPV4_noptions_3_UDP,
	PT_8023_IPV4_noptions_4_UDP,
	PT_8023_IPV4_noptions_5_UDP,
	PT_8023_IPV4_noptions_6_UDP,
	PT_8023_IPV4_noptions_7_UDP,
	PT_8023_IPV4_noptions_8_UDP,
	PT_8023_IPV4_noptions_9_UDP,
	PT_8023_IPV4_noptions_10_UDP,
	PT_8023_IPV4_noptions_11_UDP,
	PT_8023_IPV4_noptions_12_UDP,
	PT_8023_IPV4_noptions_13_UDP,
	PT_8023_IPV4_noptions_14_UDP,
	PT_8023_IPV4_noptions_15_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_0_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_1_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_2_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_3_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_4_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_5_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_6_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_7_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_8_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_9_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_10_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_11_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_12_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_13_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_14_UDP,
	PT_ETHERNET_VLAN_IPV4_noptions_15_UDP,
	PT_8023_VLAN_IPV4_noptions_0_UDP,
	PT_8023_VLAN_IPV4_noptions_1_UDP,
	PT_8023_VLAN_IPV4_noptions_2_UDP,
	PT_8023_VLAN_IPV4_noptions_3_UDP,
	PT_8023_VLAN_IPV4_noptions_4_UDP,
	PT_8023_VLAN_IPV4_noptions_5_UDP,
	PT_8023_VLAN_IPV4_noptions_6_UDP,
	PT_8023_VLAN_IPV4_noptions_7_UDP,
	PT_8023_VLAN_IPV4_noptions_8_UDP,
	PT_8023_VLAN_IPV4_noptions_9_UDP,
	PT_8023_VLAN_IPV4_noptions_10_UDP,
	PT_8023_VLAN_IPV4_noptions_11_UDP,
	PT_8023_VLAN_IPV4_noptions_12_UDP,
	PT_8023_VLAN_IPV4_noptions_13_UDP,
	PT_8023_VLAN_IPV4_noptions_14_UDP,
	PT_8023_VLAN_IPV4_noptions_15_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_0_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_1_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_2_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_3_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_4_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_5_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_6_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_7_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_8_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_9_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_10_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_11_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_12_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_13_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_14_UDP,
	PT_8023_VLAN_VLAN_IPV4_noptions_15_UDP,
	PT_ETHERNET_IPV4_noptions_0_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_1_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_2_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_3_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_4_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_5_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_6_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_7_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_8_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_9_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_10_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_11_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_12_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_13_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_14_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_15_UDP_GTPU,
	PT_8023_IPV4_noptions_0_UDP_GTPU,
	PT_8023_IPV4_noptions_1_UDP_GTPU,
	PT_8023_IPV4_noptions_2_UDP_GTPU,
	PT_8023_IPV4_noptions_3_UDP_GTPU,
	PT_8023_IPV4_noptions_4_UDP_GTPU,
	PT_8023_IPV4_noptions_5_UDP_GTPU,
	PT_8023_IPV4_noptions_6_UDP_GTPU,
	PT_8023_IPV4_noptions_7_UDP_GTPU,
	PT_8023_IPV4_noptions_8_UDP_GTPU,
	PT_8023_IPV4_noptions_9_UDP_GTPU,
	PT_8023_IPV4_noptions_10_UDP_GTPU,
	PT_8023_IPV4_noptions_11_UDP_GTPU,
	PT_8023_IPV4_noptions_12_UDP_GTPU,
	PT_8023_IPV4_noptions_13_UDP_GTPU,
	PT_8023_IPV4_noptions_14_UDP_GTPU,
	PT_8023_IPV4_noptions_15_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_0_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_1_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_2_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_3_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_4_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_5_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_6_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_7_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_8_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_9_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_10_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_11_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_12_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_13_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_14_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV4_noptions_15_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_0_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_1_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_2_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_3_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_4_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_5_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_6_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_7_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_8_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_9_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_10_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_11_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_12_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_13_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_14_UDP_GTPU,
	PT_8023_VLAN_IPV4_noptions_15_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_0_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_1_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_2_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_3_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_4_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_5_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_6_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_7_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_8_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_9_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_10_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_11_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_12_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_13_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_14_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV4_noptions_15_UDP_GTPU,
	PT_ETHERNET_IPV4_noptions_0_SCTP,
	PT_ETHERNET_IPV4_noptions_1_SCTP,
	PT_ETHERNET_IPV4_noptions_2_SCTP,
	PT_ETHERNET_IPV4_noptions_3_SCTP,
	PT_ETHERNET_IPV4_noptions_4_SCTP,
	PT_ETHERNET_IPV4_noptions_5_SCTP,
	PT_ETHERNET_IPV4_noptions_6_SCTP,
	PT_ETHERNET_IPV4_noptions_7_SCTP,
	PT_ETHERNET_IPV4_noptions_8_SCTP,
	PT_ETHERNET_IPV4_noptions_9_SCTP,
	PT_ETHERNET_IPV4_noptions_10_SCTP,
	PT_ETHERNET_IPV4_noptions_11_SCTP,
	PT_ETHERNET_IPV4_noptions_12_SCTP,
	PT_ETHERNET_IPV4_noptions_13_SCTP,
	PT_ETHERNET_IPV4_noptions_14_SCTP,
	PT_ETHERNET_IPV4_noptions_15_SCTP,
	PT_8023_IPV4_noptions_0_SCTP,
	PT_8023_IPV4_noptions_1_SCTP,
	PT_8023_IPV4_noptions_2_SCTP,
	PT_8023_IPV4_noptions_3_SCTP,
	PT_8023_IPV4_noptions_4_SCTP,
	PT_8023_IPV4_noptions_5_SCTP,
	PT_8023_IPV4_noptions_6_SCTP,
	PT_8023_IPV4_noptions_7_SCTP,
	PT_8023_IPV4_noptions_8_SCTP,
	PT_8023_IPV4_noptions_9_SCTP,
	PT_8023_IPV4_noptions_10_SCTP,
	PT_8023_IPV4_noptions_11_SCTP,
	PT_8023_IPV4_noptions_12_SCTP,
	PT_8023_IPV4_noptions_13_SCTP,
	PT_8023_IPV4_noptions_14_SCTP,
	PT_8023_IPV4_noptions_15_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_0_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_1_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_2_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_3_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_4_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_5_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_6_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_7_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_8_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_9_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_10_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_11_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_12_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_13_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_14_SCTP,
	PT_ETHERNET_VLAN_IPV4_noptions_15_SCTP,
	PT_8023_VLAN_IPV4_noptions_0_SCTP,
	PT_8023_VLAN_IPV4_noptions_1_SCTP,
	PT_8023_VLAN_IPV4_noptions_2_SCTP,
	PT_8023_VLAN_IPV4_noptions_3_SCTP,
	PT_8023_VLAN_IPV4_noptions_4_SCTP,
	PT_8023_VLAN_IPV4_noptions_5_SCTP,
	PT_8023_VLAN_IPV4_noptions_6_SCTP,
	PT_8023_VLAN_IPV4_noptions_7_SCTP,
	PT_8023_VLAN_IPV4_noptions_8_SCTP,
	PT_8023_VLAN_IPV4_noptions_9_SCTP,
	PT_8023_VLAN_IPV4_noptions_10_SCTP,
	PT_8023_VLAN_IPV4_noptions_11_SCTP,
	PT_8023_VLAN_IPV4_noptions_12_SCTP,
	PT_8023_VLAN_IPV4_noptions_13_SCTP,
	PT_8023_VLAN_IPV4_noptions_14_SCTP,
	PT_8023_VLAN_IPV4_noptions_15_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_0_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_1_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_2_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_3_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_4_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_5_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_6_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_7_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_8_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_9_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_10_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_11_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_12_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_13_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_14_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV4_noptions_15_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_0_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_1_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_2_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_3_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_4_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_5_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_6_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_7_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_8_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_9_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_10_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_11_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_12_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_13_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_14_SCTP,
	PT_8023_VLAN_VLAN_IPV4_noptions_15_SCTP,
	PT_ETHERNET_IPV6,
	PT_8023_IPV6,
	PT_ETHERNET_VLAN_IPV6,
	PT_8023_VLAN_IPV6,
	PT_ETHERNET_VLAN_VLAN_IPV6,
	PT_8023_VLAN_VLAN_IPV6,
	PT_ETHERNET_IPV6_ICMPV6,
	PT_8023_IPV6_ICMPV6,
	PT_ETHERNET_VLAN_IPV6_ICMPV6,
	PT_8023_VLAN_IPV6_ICMPV6,
	PT_ETHERNET_VLAN_VLAN_IPV6_ICMPV6,
	PT_8023_VLAN_VLAN_IPV6_ICMPV6,
	PT_ETHERNET_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_8023_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_ETHERNET_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_8023_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_ETHERNET_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_8023_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_SRC,
	PT_ETHERNET_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_8023_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_ETHERNET_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_8023_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_ETHERNET_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_8023_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_LLADR_TGT,
	PT_ETHERNET_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_8023_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_ETHERNET_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_8023_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_ETHERNET_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_8023_VLAN_VLAN_IPV6_ICMPV6_ICMPV6_OPTS_PREFIX_INFO,
	PT_ETHERNET_IPV6_TCP,
	PT_8023_IPV6_TCP,
	PT_ETHERNET_VLAN_IPV6_TCP,
	PT_8023_VLAN_IPV6_TCP,
	PT_ETHERNET_VLAN_VLAN_IPV6_TCP,
	PT_8023_VLAN_VLAN_IPV6_TCP,
	PT_ETHERNET_IPV6_UDP,
	PT_8023_IPV6_UDP,
	PT_ETHERNET_VLAN_IPV6_UDP,
	PT_8023_VLAN_IPV6_UDP,
	PT_ETHERNET_VLAN_VLAN_IPV6_UDP,
	PT_8023_VLAN_VLAN_IPV6_UDP,
	PT_ETHERNET_IPV6_UDP_GTPU,
	PT_8023_IPV6_UDP_GTPU,
	PT_ETHERNET_VLAN_IPV6_UDP_GTPU,
	PT_8023_VLAN_IPV6_UDP_GTPU,
	PT_ETHERNET_VLAN_VLAN_IPV6_UDP_GTPU,
	PT_8023_VLAN_VLAN_IPV6_UDP_GTPU,
	PT_ETHERNET_IPV6_SCTP,
	PT_8023_IPV6_SCTP,
	PT_ETHERNET_VLAN_IPV6_SCTP,
	PT_8023_VLAN_IPV6_SCTP,
	PT_ETHERNET_VLAN_VLAN_IPV6_SCTP,
	PT_8023_VLAN_VLAN_IPV6_SCTP,
	PT_MAX__
}pkt_types_t;

//Matrix of header offsets from the first byte of the buffer.
extern const int protocol_offsets_bt[PT_MAX__][PT_PROTO_MAX__];

//Only used in transitions (inside the MACRO )
typedef enum __unrolled_protocol_types{
	__UNROLLED_PT_PROTO_ETHERNET,
	__UNROLLED_PT_PROTO_8023,
	__UNROLLED_PT_PROTO_MPLS,
	__UNROLLED_PT_PROTO_VLAN,
	__UNROLLED_PT_PROTO_PPPOE,
	__UNROLLED_PT_PROTO_PPP,
	__UNROLLED_PT_PROTO_ARPV4,
	__UNROLLED_PT_PROTO_IPV4_noptions_0,
	__UNROLLED_PT_PROTO_IPV4_noptions_1,
	__UNROLLED_PT_PROTO_IPV4_noptions_2,
	__UNROLLED_PT_PROTO_IPV4_noptions_3,
	__UNROLLED_PT_PROTO_IPV4_noptions_4,
	__UNROLLED_PT_PROTO_IPV4_noptions_5,
	__UNROLLED_PT_PROTO_IPV4_noptions_6,
	__UNROLLED_PT_PROTO_IPV4_noptions_7,
	__UNROLLED_PT_PROTO_IPV4_noptions_8,
	__UNROLLED_PT_PROTO_IPV4_noptions_9,
	__UNROLLED_PT_PROTO_IPV4_noptions_10,
	__UNROLLED_PT_PROTO_IPV4_noptions_11,
	__UNROLLED_PT_PROTO_IPV4_noptions_12,
	__UNROLLED_PT_PROTO_IPV4_noptions_13,
	__UNROLLED_PT_PROTO_IPV4_noptions_14,
	__UNROLLED_PT_PROTO_IPV4_noptions_15,
	__UNROLLED_PT_PROTO_ICMPV4,
	__UNROLLED_PT_PROTO_IPV6,
	__UNROLLED_PT_PROTO_ICMPV6,
	__UNROLLED_PT_PROTO_ICMPV6_OPTS,
	__UNROLLED_PT_PROTO_ICMPV6_OPTS_LLADR_SRC,
	__UNROLLED_PT_PROTO_ICMPV6_OPTS_LLADR_TGT,
	__UNROLLED_PT_PROTO_ICMPV6_OPTS_PREFIX_INFO,
	__UNROLLED_PT_PROTO_TCP,
	__UNROLLED_PT_PROTO_UDP,
	__UNROLLED_PT_PROTO_SCTP,
	__UNROLLED_PT_PROTO_GTPU,
	__UNROLLED_PT_PROTO_MAX__
}__unrolled_protocol_types_t;

//Matrix of incremental classification steps. The only purpose of this matrix is to simplify parsing code, it is not essentially necessary, although _very_ desirable.
extern const int parse_transitions [PT_MAX__][__UNROLLED_PT_PROTO_MAX__];

//Matrix for pushing protocol headers. 
extern const int push_transitions [PT_MAX__][__UNROLLED_PT_PROTO_MAX__];

//Matrix for popping protocol headers. 
extern const int pop_transitions [PT_MAX__][__UNROLLED_PT_PROTO_MAX__];


#define PT_GET_HDR(tmp, state, proto)\
	do{\
		tmp = state->base + protocol_offsets_bt[ state->type ][ proto ];\
		if(tmp < state->base )\
			tmp = NULL;\
	}while(0)


#define PT_CLASS_ADD_PROTO(state, PROTO_TYPE) do{\
	pkt_types_t next_header = (pkt_types_t)parse_transitions[state->type][ __UNROLLED_PT_PROTO_##PROTO_TYPE ];\
	if( unlikely(next_header == 0) ){ assert(0); return; }else{ state->type = next_header;  }\
}while(0)

#define PT_CLASS_ADD_IPV4_OPTIONS(state, NUM_OPTIONS) do{\
	switch(NUM_OPTIONS){\
		case 0: PT_CLASS_ADD_PROTO(state, IPV4_noptions_0);break;\
		case 1: PT_CLASS_ADD_PROTO(state, IPV4_noptions_1);break;\
		case 2: PT_CLASS_ADD_PROTO(state, IPV4_noptions_2);break;\
		case 3: PT_CLASS_ADD_PROTO(state, IPV4_noptions_3);break;\
		case 4: PT_CLASS_ADD_PROTO(state, IPV4_noptions_4);break;\
		case 5: PT_CLASS_ADD_PROTO(state, IPV4_noptions_5);break;\
		case 6: PT_CLASS_ADD_PROTO(state, IPV4_noptions_6);break;\
		case 7: PT_CLASS_ADD_PROTO(state, IPV4_noptions_7);break;\
		case 8: PT_CLASS_ADD_PROTO(state, IPV4_noptions_8);break;\
		case 9: PT_CLASS_ADD_PROTO(state, IPV4_noptions_9);break;\
		case 10: PT_CLASS_ADD_PROTO(state, IPV4_noptions_10);break;\
		case 11: PT_CLASS_ADD_PROTO(state, IPV4_noptions_11);break;\
		case 12: PT_CLASS_ADD_PROTO(state, IPV4_noptions_12);break;\
		case 13: PT_CLASS_ADD_PROTO(state, IPV4_noptions_13);break;\
		case 14: PT_CLASS_ADD_PROTO(state, IPV4_noptions_14);break;\
		case 15: PT_CLASS_ADD_PROTO(state, IPV4_noptions_15);break;\
		default: assert(0);\
}}while(0)

#define PT_PUSH_PROTO(state, PROTO_TYPE)\
	(pkt_types_t)push_transitions[state->type][ __UNROLLED_PT_PROTO_##PROTO_TYPE ]

#define PT_POP_PROTO(state, PROTO_TYPE)\
	(pkt_types_t)pop_transitions[state->type][ __UNROLLED_PT_PROTO_##PROTO_TYPE ]
#endif //PKT_TYPES_H
