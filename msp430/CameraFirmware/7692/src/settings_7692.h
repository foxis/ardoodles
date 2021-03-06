
struct reg_addr_val_pair_struct {
	char reg_addr;
	char reg_val;
};

struct reg_addr_val_pair_struct ov7692_init_settings_array[] = {
    {0x12, 0x80},
    {0x0e, 0x08},
    //{0x69, 0x52},
    //{0x1e, 0xb3},
    {0x48, 0x42},
    {0xff, 0x01},
    {0xae, 0xa0},
    {0xa8, 0x26},
    {0xb4, 0xc0},
    {0xb5, 0x40},
    {0xff, 0x00},
    {0x0c, 0x10}, //{0x0c, 0x00},
    //{0x62, 0x10},
    {0x12, 0x00},
    {0x17, 0x65},
    {0x18, 0xa4},
    {0x19, 0x0a},
    {0x1a, 0xf6},
    {0x3e, 0x30},
    {0x64, 0x0a},
    {0xff, 0x01},
    {0xb4, 0xc0},
    {0xff, 0x00},
    {0x67, 0x20},
    {0x82, 0x03},
    {0x0e, 0x00},
    {0x70, 0x04}, //{0x70, 0x00},
    {0x71, 0x34},
    {0x74, 0x28},
    {0x75, 0x98},
    {0x76, 0x00},
    {0x77, 0x64},
    {0x78, 0x01},
    {0x79, 0xc2},
    {0x7a, 0x4e},
    {0x7b, 0x1f},
    {0x7c, 0x00},

    //{0x11, 0x00}, // 25 �� (�� �������)
    //{0x29, 0x50},
    //{0x2A, 0x30},
    //{0x2B, 0x08},
    {0x2C, 0x67},
    //{0x15, 0x00},
    //{0x2D, 0x00},
    //{0x2E, 0x00},

    {0x20, 0x00},
    {0x21, 0x23},
    {0x50, 0x9a},
    {0x51, 0x80},
    {0x4c, 0x7d},
    {0x0e, 0x00},
    {0x80, 0x7f},
    {0x85, 0x10},
    {0x86, 0x00},
    {0x87, 0x00},
    {0x88, 0x00},
    {0x89, 0x2a},
    {0x8a, 0x26},
    {0x8b, 0x22},
    {0xbb, 0x7a},
    {0xbc, 0x69},
    {0xbd, 0x11},
    {0xbe, 0x13},
    {0xbf, 0x81},
    {0xc0, 0x96},
    {0xc1, 0x1e},
    {0xb7, 0x05},
    {0xb8, 0x09},
    {0xb9, 0x00},
    {0xba, 0x18},
    {0x5a, 0x1f},
    {0x5b, 0x9f},
    {0x5c, 0x6a},
    {0x5d, 0x42},
    {0x24, 0x78},
    {0x25, 0x68},
    {0x26, 0xb3},
    {0xa3, 0x0b},
    {0xa4, 0x15},
    {0xa5, 0x2a},
    {0xa6, 0x51},
    {0xa7, 0x63},
    {0xa8, 0x74},
    {0xa9, 0x83},
    {0xaa, 0x91},
    {0xab, 0x9e},
    {0xac, 0xaa},
    {0xad, 0xbe},
    {0xae, 0xce},
    {0xaf, 0xe5},
    {0xb0, 0xf3},
    {0xb1, 0xfb},
    {0xb2, 0x06},
    {0x8c, 0x5c},
    {0x8d, 0x11},
    {0x8e, 0x12},
    {0x8f, 0x19},
    {0x90, 0x50},
    {0x91, 0x20},
    {0x92, 0x96},
    {0x93, 0x80},
    {0x94, 0x13},
    {0x95, 0x1b},
    {0x96, 0xff},
    {0x97, 0x00},
    {0x98, 0x3d},
    {0x99, 0x36},
    {0x9a, 0x51},
    {0x9b, 0x43},
    {0x9c, 0xf0},
    {0x9d, 0xf0},
    {0x9e, 0xf0},
    {0x9f, 0xff},
    {0xa0, 0x68},
    {0xa1, 0x62},
    {0xa2, 0x0e},

    // ��
    {0x19, 0x06},
    {0x1A, 0xF6},
    {0xCB, 0xE6},
    {0xCF, 0xE6},
};

