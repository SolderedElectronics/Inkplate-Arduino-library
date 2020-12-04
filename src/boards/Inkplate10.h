#define E_INK_WIDTH  1200
#define E_INK_HEIGHT 825

#define MCP23017_INT_ADDR      0x20
#define MCP23017_EXT_ADDR      0x22
#define MCP23017_INT_PORTA     0x00
#define MCP23017_INT_PORTB     0x01
#define MCP23017_INT_NO_MIRROR false
#define MCP23017_INT_MIRROR    true
#define MCP23017_INT_PUSHPULL  false
#define MCP23017_INT_OPENDRAIN true
#define MCP23017_INT_ACTLOW    false
#define MCP23017_INT_ACTHIGH   true

#define MCP23017_IODIRA   0x00
#define MCP23017_IPOLA    0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA  0x06
#define MCP23017_INTCONA  0x08
#define MCP23017_IOCONA   0x0A
#define MCP23017_GPPUA    0x0C
#define MCP23017_INTFA    0x0E
#define MCP23017_INTCAPA  0x10
#define MCP23017_GPIOA    0x12
#define MCP23017_OLATA    0x14

#define MCP23017_IODIRB   0x01
#define MCP23017_IPOLB    0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB  0x07
#define MCP23017_INTCONB  0x09
#define MCP23017_IOCONB   0x0B
#define MCP23017_GPPUB    0x0D
#define MCP23017_INTFB    0x0F
#define MCP23017_INTCAPB  0x11
#define MCP23017_GPIOB    0x13
#define MCP23017_OLATB    0x15

#define WAVEFORM3BIT                                                                                                   \
    {{0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 2, 2, 2, 1, 1, 0}, {0, 2, 1, 1, 2, 2, 1, 0}, {1, 2, 2, 1, 2, 2, 1, 0},           \
     {0, 2, 1, 2, 2, 2, 1, 0}, {2, 2, 2, 2, 2, 2, 1, 0}, {0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 2, 2, 2, 2, 2, 0}};

#define WAVEFORM                                                                                                       \
    {0x00000008, 0x00000008, 0x00200408, 0x80281888, 0x60A81898, 0x60A8A8A8, 0x60A8A8A8, 0x6068A868, 0x6868A868,       \
     0x6868A868, 0x68686868, 0x6A686868, 0x5A686868, 0x5A686868, 0x5A586A68, 0x5A5A6A68, 0x5A5A6A68, 0x55566A68,       \
     0x55565A64, 0x55555654, 0x55555556, 0x55555556, 0x55555556, 0x55555516, 0x55555596, 0x15555595, 0x95955595,       \
     0x95959595, 0x95949495, 0x94949495, 0x94949495, 0xA4949494, 0x9494A4A4, 0x84A49494, 0x84948484, 0x84848484,       \
     0x84848484, 0x84848484, 0xA5A48484, 0xA9A4A4A8, 0xA9A8A8A8, 0xA5A9A9A4, 0xA5A5A5A4, 0xA1A5A5A1, 0xA9A9A9A9,       \
     0xA9A9A9A9, 0xA9A9A9A9, 0xA9A9A9A9, 0x15151515, 0x11111111};