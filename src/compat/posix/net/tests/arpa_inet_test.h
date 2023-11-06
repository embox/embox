#ifndef EMBOX_TEST_NET_INET_PTON_TEST_H_
#define EMBOX_TEST_NET_INET_PTON_TEST_H_

#include "netinet/in.h"

#define V4_MAX_ADDR_LEN 15
struct v4_test_info {
    char test_string[V4_MAX_ADDR_LEN*2];
    uint8_t pton_pass;
    uint8_t aton_pass;
    struct in_addr pton_val;
    struct in_addr aton_val;
    char true_ntop[V4_MAX_ADDR_LEN];
    char true_ntoa[V4_MAX_ADDR_LEN];
};
#define V6_MAX_ADDR_LEN 46
struct v6_test_info {
    uint8_t pass;
    char test_string[V6_MAX_ADDR_LEN];
    char true_ntop[V6_MAX_ADDR_LEN];
    struct in6_addr true_val;
};

#define V4_TESTS sizeof(v4_test_ref) / sizeof(struct v4_test_info)
#define V6_TESTS sizeof(v6_test_ref) / sizeof(struct v6_test_info)


static struct v4_test_info v4_test_ref[] = {
    {
        .test_string = "255.255.255.0",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00ffffff,
        .aton_val.s_addr = 0x00ffffff,
        .true_ntop = "255.255.255.0",
        .true_ntoa = "255.255.255.0"
    },
    {
        .test_string = "192.0.00a.003",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "192.0.40.30",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x1e2800c0,
        .aton_val.s_addr = 0x1e2800c0,
        .true_ntop = "192.0.40.30",
        .true_ntoa = "192.0.40.30"
    },
    {
        .test_string = "0.0.0.0",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "0.0.0.0",
        .true_ntoa = "0.0.0.0"
    },
    {
        .test_string = "1.2.3.4",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x04030201,
        .aton_val.s_addr = 0x04030201,
        .true_ntop = "1.2.3.4",
        .true_ntoa = "1.2.3.4"
    },
    {
        .test_string = "192.168.4.3",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x0304a8c0,
        .aton_val.s_addr = 0x0304a8c0,
        .true_ntop = "192.168.4.3",
        .true_ntoa = "192.168.4.3"
    },
    {
        .test_string = "192.0.4.3.",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "24.136.46.33",
        .pton_pass = 1,
        .aton_pass = 1,
        .pton_val.s_addr = 0x212e8818,
        .aton_val.s_addr = 0x212e8818,
        .true_ntop = "24.136.46.33",
        .true_ntoa = "24.136.46.33"
    },
    {
        .test_string = "24.136.46.33g",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "256.20.0.1",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "100.20.0.0.1",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "f.1.2.3",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "100.20.0",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00001464,
        .true_ntop = "",
        .true_ntoa = "100.20.0.0"
    },
    {
        .test_string = "100.20",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x14000064,
        .true_ntop = "",
        .true_ntoa = "100.0.0.20"
    },
    {
        .test_string = "100",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x64000000,
        .true_ntop = "",
        .true_ntoa = "0.0.0.100"
    },
    {
        .test_string = "100.20.16000",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x803e1464,
        .true_ntop = "",
        .true_ntoa = "100.20.62.128"
    },
    {
        .test_string = "192.0.004.003",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x030400c0,
        .true_ntop = "",
        .true_ntoa = "192.0.4.3"
    },
    {
        .test_string = "192.0.04.03",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x030400c0,
        .true_ntop = "",
        .true_ntoa = "192.0.4.3"
    },
    {
        .test_string = "192.0.0a.0b",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
    {
        .test_string = "192.0.0xa.0xb",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x0b0a00c0,
        .true_ntop = "",
        .true_ntoa = "192.0.10.11"
    },
    {
        .test_string = "192.0.0xa.017",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x0f0a00c0,
        .true_ntop = "",
        .true_ntoa = "192.0.10.15"
    },
    {
        .test_string = "0x54.54.54.054",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x2c363654,
        .true_ntop = "",
        .true_ntoa = "84.54.54.44"
    },
    {
        .test_string = "0xf.1.2.3",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x0302010f,
        .true_ntop = "",
        .true_ntoa = "15.1.2.3"
    },
    {
        .test_string = "1239871928",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0xb8f1e649,
        .true_ntop = "",
        .true_ntoa = "73.230.241.184"
    },
    {
        .test_string = "0xfdecba",
        .pton_pass = 0,
        .aton_pass = 1,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0xbaecfd00,
        .true_ntop = "",
        .true_ntoa = "0.253.236.186"
    },
    {
        .test_string = "google.com",
        .pton_pass = 0,
        .aton_pass = 0,
        .pton_val.s_addr = 0x00000000,
        .aton_val.s_addr = 0x00000000,
        .true_ntop = "",
        .true_ntoa = ""
    },
};
static struct v6_test_info v6_test_ref[] = {
    {.pass = 1,
        .test_string = "1:0:0:0:0:0:0:8",
        .true_ntop = "1::8",
        .true_val.s6_addr16 = {0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0800}},
    {.pass = 1,
        .test_string = "1:2:3:4:5:6:7:8",
        .true_ntop = "1:2:3:4:5:6:7:8",
        .true_val.s6_addr16 = {0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600,
            0x0700, 0x0800}},
    {.pass = 1,
        .test_string = "0001:0002:0003:0004:0005:0006:0007:0008",
        .true_ntop = "1:2:3:4:5:6:7:8",
        .true_val.s6_addr16 = {0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600,
            0x0700, 0x0800}},
    {.pass = 1,
        .test_string = "1111:2222:3333:4444:5555:6666:7777:8888",
        .true_ntop = "1111:2222:3333:4444:5555:6666:7777:8888",
        .true_val.s6_addr16 = {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666,
            0x7777, 0x8888}},
    {.pass = 1,
        .test_string = "ffff:eeee:dddd:cccc:bbbb:aaaa:255.255.255.255",
        .true_ntop = "ffff:eeee:dddd:cccc:bbbb:aaaa:ffff:ffff",
        .true_val.s6_addr16 = {0xffff, 0xeeee, 0xdddd, 0xcccc, 0xbbbb, 0xaaaa,
            0xffff, 0xffff}},
    {.pass = 1,
        .test_string = "ff:0:0:aa:0:0:aa:ff",
        .true_ntop = "ff::aa:0:0:aa:ff",
        .true_val.s6_addr16 = {0xff00, 0x0000, 0x0000, 0xaa00, 0x0000, 0x0000,
            0xaa00, 0xff00}},
    {.pass = 1,
        .test_string = "fF:0:0:Aa:0:0:aA:Ff",
        .true_ntop = "ff::aa:0:0:aa:ff",
        .true_val.s6_addr16 = {0xff00, 0x0000, 0x0000, 0xaa00, 0x0000, 0x0000,
            0xaa00, 0xff00}},
    {.pass = 1,
        .test_string = "::FFFF",
        .true_ntop = "::ffff",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0xffff}},
    {.pass = 1,
        .test_string = "FFFF::",
        .true_ntop = "ffff::",
        .true_val.s6_addr16 = {0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "FFAA::1",
        .true_ntop = "ffaa::1",
        .true_val.s6_addr16 = {0xaaff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0100}},
    {.pass = 1,
        .test_string = "FF:AA::1",
        .true_ntop = "ff:aa::1",
        .true_val.s6_addr16 = {0xff00, 0xaa00, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0100}},
    {.pass = 1,
        .test_string = "FF::AA:1",
        .true_ntop = "ff::aa:1",
        .true_val.s6_addr16 = {0xff00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0xaa00, 0x0100}},
    {.pass = 1,
        .test_string = "FF:BB:CC::AA:1",
        .true_ntop = "ff:bb:cc::aa:1",
        .true_val.s6_addr16 = {0xff00, 0xbb00, 0xcc00, 0x0000, 0x0000, 0x0000,
            0xaa00, 0x0100}},
    {.pass = 0,
        .test_string = "10000:BB:CC::AA:1",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 0,
        .test_string = "ff:0:0:aa:0:0:aa",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 0,
        .test_string = "ff::1::1",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "ff:0:0:aa:0:0:1.2.3.4",
        .true_ntop = "ff::aa:0:0:102:304",
        .true_val.s6_addr16 = {0xff00, 0x0000, 0x0000, 0xaa00, 0x0000, 0x0000,
            0x0201, 0x0403}},
    {.pass = 0,
        .test_string = "ff:0:0:aa:0:0:1.2.3.a",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "ff::1.2.3.4",
        .true_ntop = "ff::102:304",
        .true_val.s6_addr16 = {0xff00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0201, 0x0403}},
    {.pass = 1,
        .test_string = "::ffff:102:304",
        .true_ntop = "::ffff:1.2.3.4",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
            0x0201, 0x0403}},
    {.pass = 1,
        .test_string = "::102:304",
        .true_ntop = "::1.2.3.4",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0201, 0x0403}},
    {.pass = 1,
        .test_string = "::FFFF:1.2.3.4",
        .true_ntop = "::ffff:1.2.3.4",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
            0x0201, 0x0403}},
    {.pass = 1,
        .test_string = "0:0:0:0:0:FFFF:1.2.3.4",
        .true_ntop = "::ffff:1.2.3.4",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff,
            0x0201, 0x0403}},
    {.pass = 0,
        .test_string = "1:0:0:0:0:0:FFFF:1.2.3.4",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "::1.2.3.4",
        .true_ntop = "::1.2.3.4",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0201, 0x0403}},
    {.pass = 0,
        .test_string = "ff:1.2.3.f",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 0,
        .test_string = "1:2:3:4:5:6:7:8:9",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 0,
        .test_string = "1:2:3:4:5:6:7:",
        .true_ntop = "",
        .true_val.s6_addr16 = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "1:2:3:4:5:6::",
        .true_ntop = "1:2:3:4:5:6::",
        .true_val.s6_addr16 = {0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600,
            0x0000, 0x0000}},
    {.pass = 1,
        .test_string = "1:2:3:4:5:6:7::",
        .true_ntop = "1:2:3:4:5:6:7:0",
        .true_val.s6_addr16 = {0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600,
            0x0700, 0x0000}},
};

#endif /*EMBOX_TEST_NET_INET_PTON_TEST_H_*/
