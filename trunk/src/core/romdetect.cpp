// #########################################################################

#include <romdetect.h>

// #########################################################################

const char*romtype_str[romtype_count]=
{
    "unknown",
    "md",
    "smd",
    "sms",
    "32x",
    "scd bios",
    "scd bram",
    "scd bios+bram",
    "scd multibios",//not used
    "vgm",
    "n64"
};

// #########################################################################

struct patch_data
{
    int offset;
    WORD value;
};

// #########################################################################

struct auto_patch_entry
{
    const int size;
    const md5struct md5;
    const char*name;
    const int romtype;
    patch_data patch[16];
};

// #########################################################################

struct save_entry
{
    const char*game_id;
    const WORD chk;
    const BYTE eeprom;
    const WORD ssize;
};

// #########################################################################

struct sms_save_entry
{
    const int size;
    const md5struct md5;
    const char*name;
    const int ssize;
};

// #########################################################################

auto_patch_entry auto_patch_table[]=
{
//scd_bios
    {
        128*KB,
        {0xbd,0xeb,0x4c,0x47,0xda,0x61,0x39,0x46,0xd4,0x22,0xd9,0x7d,0x98,0xb2,0x1c,0xda},
        "[BIOS] Mega-CD (Japan) (v1.00s)",scd_bios,
        {{0x722,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x27,0x8a,0x93,0x97,0xd1,0x92,0x14,0x9e,0x84,0xe8,0x20,0xac,0x62,0x1a,0x8e,0xdd},
        "[BIOS] Mega-CD (Japan) (v1.00p)",scd_bios,
        {{0x720,0x4e75},{-1}}
    },
    {
        128*KB,
        {0xe6,0x6f,0xa1,0xdc,0x58,0x20,0xd2,0x54,0x61,0x1f,0xdc,0xdb,0xa0,0x66,0x23,0x72},
        "[BIOS] Mega-CD (Europe) (v1.00)",scd_bios,
        {{0x7c8,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x2e,0xfd,0x74,0xe3,0x23,0x2f,0xf2,0x60,0xe3,0x71,0xb9,0x9f,0x84,0x02,0x4f,0x7f},
        "[BIOS] Sega CD (USA) (v1.10)",scd_bios,
        {{0x7c2,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x68,0x3a,0x8a,0x9e,0x27,0x36,0x62,0x56,0x11,0x72,0x46,0x8d,0xfa,0x28,0x58,0xeb},
        "[BIOS] Mega-CD 2 (Japan) (v2.00c)",scd_bios,
        {{0x72c,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x9b,0x56,0x2e,0xbf,0x2d,0x09,0x5b,0xf1,0xda,0xba,0xdb,0xc1,0x88,0x1f,0x51,0x9a},
        "[BIOS] Mega-CD 2 (Europe) (v2.00)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
    {
        128*KB,
        {0xb1,0x0c,0x0a,0x97,0xab,0xc5,0x7b,0x75,0x84,0x97,0xd3,0xfa,0xe6,0xab,0x35,0xa4},
        "[BIOS] Mega-CD 2 (Europe) (v2.00w)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
//    {
//        128*KB,
//        {0x68,0x45,0x57,0x9b,0xd2,0x11,0xe2,0x4e,0xaf,0xe3,0x13,0x93,0x3e,0x6f,0x8d,0x7b},
//        "[BIOS] Sega CD 2 (USA) (v2.00) [b]",scd_bios,
//        {{0x7ae,0x4e75},{-1}}
//    },
    {
        128*KB,
        {0xec,0xc8,0x37,0xc3,0x1d,0x77,0xb7,0x74,0xc6,0xe2,0x7e,0x38,0xf8,0x28,0xaa,0x9a},
        "[BIOS] Sega CD 2 (USA) (v2.11x)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x85,0x4b,0x91,0x50,0x24,0x0a,0x19,0x80,0x70,0x15,0x0e,0x45,0x66,0xae,0x12,0x90},
        "[BIOS] Sega CD 2 (USA) (v2.00w)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
    {
        128*KB,
        {0xba,0xca,0x1d,0xf2,0x71,0xd7,0xc1,0x1f,0xe5,0x00,0x87,0xc0,0x35,0x8f,0x4e,0xb5},
        "[BIOS] CDX (USA) (v2.21x)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
    {
        128*KB,
        {0xae,0x52,0xba,0xd8,0xad,0x2e,0xf4,0x83,0xd8,0x7f,0xb4,0x72,0xe5,0x21,0x29,0x4d},
        "[BIOS] Multi-Mega (Europe) (v2.21x)",scd_bios,
        {{0x7ae,0x4e75},{-1}}
    },
    {
        128*KB,
        {0xa5,0xa2,0xf9,0xaa,0xe5,0x7d,0x46,0x4b,0xc6,0x6b,0x80,0xee,0x79,0xc3,0xda,0x6e},
        "[BIOS] LaserActive (Japan) (v0.98)",scd_bios,
        {{0x84a,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x69,0x1c,0x3f,0xd3,0x68,0x21,0x12,0x80,0xd2,0x68,0x64,0x5c,0x0e,0xfd,0x2e,0xff},
        "[BIOS] LaserActive (USA) (v0.98)",scd_bios,
        {{0x84a,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x0e,0x73,0x93,0xcd,0x09,0x51,0xd6,0xdd,0xe8,0x18,0xfc,0xd4,0xcd,0x81,0x94,0x66},
        "[BIOS] LaserActive (USA) (v1.04)",scd_bios,
        {{0x84a,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x73,0x2b,0x60,0x96,0x02,0x26,0xca,0xf3,0x3d,0xf6,0xb4,0x87,0xf3,0x76,0xde,0x69},
        "[BIOS] WonderMega (Japan) (v1.00) (sega)",scd_bios,
        {{0x720,0x4e75},{-1}}
    },
    {
        128*KB,
        {0x82,0xce,0x23,0x63,0xf7,0xdd,0xc6,0x20,0xb9,0xc2,0x55,0xd3,0x58,0x33,0x11,0x8f},
        "[BIOS] X'Eye (USA) (v2.00)",scd_bios,
        {{0x84a,0x4e75},{-1}}
    },
    {
        512*KB,
        {0x16,0x97,0x9d,0x13,0x37,0x7c,0x1b,0xbb,0xbf,0xa9,0x51,0xdd,0xa5,0x6d,0xdf,0xd6},
        "SegaCD MultiBIOS",scd_multibios, // http://arakon.dyndns.org/
        {{-1}}
    },
//md_rom
    {
        5*MB,
        {0x75,0x95,0x0e,0x3a,0xa9,0x35,0x7a,0x21,0x71,0x5f,0xfe,0x2f,0xa5,0x1a,0x45,0x4c},
        "Super Street Fighter II - The New Challengers (USA)",md_rom,
        {{0x1f0,0x5520},{0x3bdc,0x605e},{-1}}
    },
    {
        5*MB,
        {0x48,0x90,0xfe,0x7d,0xbb,0xed,0x12,0xd7,0xdb,0x1e,0xf0,0x29,0xca,0x7d,0x93,0x0a},
        "Super Street Fighter II - The New Challengers (Europe)",md_rom,
        {{0x3be6,0x6060},{0x63fa,0x6042},{-1}}
    },
    {
        5*MB,
        {0x82,0x5e,0x67,0x79,0x7b,0xfc,0xf3,0xc2,0x77,0x54,0xd5,0x9b,0x4f,0x66,0x84,0x93},
        "Super Street Fighter II - The New Challengers (Japan)",md_rom,
        {{0x1f0,0x5520},{0x3bdc,0x605e},{-1}}
    },
    {
        32*KB,
        {0xa0,0x02,0x8b,0x30,0x43,0xf9,0xd5,0x9c,0xee,0xb0,0x3d,0xa5,0xb0,0x73,0xb3,0x0d},
        "Action Replay (Europe) (Program)",md_rom,
        {{-1}}
    },
    {
        1*MB,
        {0x3c,0xc6,0xdf,0x24,0x3e,0x71,0x40,0x97,0xf1,0x59,0x9c,0xf6,0x18,0xf9,0x4d,0x0b},
        "Aq Renkan Awa (China) (Unl)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x4d,0x24,0x7a,0x5f,0xc6,0x11,0xe6,0x6d,0xbe,0x46,0xf9,0x60,0x29,0x2e,0x36,0x56},
        "Budokan - The Martial Spirit (USA)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0xb2,0x7a,0x54,0xc8,0xac,0x36,0x71,0xe7,0x2e,0x85,0x38,0x75,0xef,0x60,0x73,0xeb},
        "Golden Axe II (World) (Beta)",md_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x30,0xee,0x5a,0xe0,0xe2,0xa4,0xb2,0x99,0x5a,0x79,0x17,0x3f,0x67,0x27,0x8c,0x69},
        "Ishido - The Way of Stones (USA)",md_rom,
        {{-1}}
    },
    {
        1*MB,
        {0xb6,0xce,0x4e,0x32,0x55,0x8f,0x71,0x76,0x6b,0x66,0x2c,0xb7,0x41,0xcd,0x45,0x70},
        "Ma Qiao E Mo Ta - Devilish Mahjong Tower (China) (Unl)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x45,0x5e,0x83,0x6c,0xa5,0xdd,0x9c,0xb7,0x82,0xe7,0x45,0x5e,0x58,0x4c,0xf6,0x5a},
        "Onslaught (USA, Europe)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x23,0x49,0x5b,0x29,0x85,0x1e,0x9d,0x49,0x59,0x39,0xa8,0x8b,0x62,0x7c,0xe2,0xcd},
        "Populous (USA)",md_rom,
        {{-1}}
    },
    {
        64*KB,
        {0x9b,0xce,0x44,0x31,0x30,0x81,0x34,0x6c,0x5a,0xc9,0x05,0x50,0x96,0x4a,0xf7,0x6d},
        "Pro Action Replay 2 (Europe) (Program)",md_rom,
        {{-1}}
    },
    {
        2*MB,
        {0x4a,0xbb,0x04,0x05,0xb2,0x70,0x69,0x52,0x61,0x49,0x47,0x20,0xa2,0xaf,0x07,0x83},
        "Shi Jie Zhi Bang Zheng Ba Zhan - World Pro Baseball 94 (China) (Unl)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x7d,0x20,0xd7,0x4e,0x81,0x52,0x47,0x73,0x40,0x9f,0x4f,0x89,0xe9,0x44,0xcf,0xaa},
        "Whac-a-Critter (USA) (Unl)",md_rom,
        {{-1}}
    },
    {
        4*MB,
        {0x80,0x94,0xc9,0xa1,0xc0,0xcf,0x82,0xb2,0x66,0x21,0xa3,0x82,0xd3,0xee,0x47,0x24},
        "Xin Qi Gai Wang Zi (China) (Alt) (Unl)",md_rom,
        {{-1}}
    },
    {
        4*MB,
        {0x52,0x8e,0x17,0x12,0x64,0xeb,0xfd,0x90,0xe5,0x3f,0xd9,0x66,0x7d,0x1c,0x61,0xd1},
        "Xin Qi Gai Wang Zi (China) (Unl)",md_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x6a,0x23,0xd5,0x58,0x5b,0x4f,0xe6,0x53,0x2d,0x10,0xf4,0x40,0xf7,0x8a,0xf1,0x9a},
        "Zany Golf (USA, Europe)",md_rom,
        {{-1}}
    },
    {
        1000*KB+239,
        {0x99,0x37,0xc7,0x69,0x8d,0x62,0x67,0x2f,0x7a,0xb4,0x6d,0x3f,0xc0,0x9f,0x0f,0x2b},
        "Zombie High (USA) (Proto)",md_rom,
        {{-1}}
    },
//sms_rom
    {
        128*KB,
        {0x5c,0xd8,0xf6,0x2c,0xd8,0x78,0x6a,0xf0,0x22,0x6e,0x6d,0x22,0x48,0x27,0x93,0x38},
        "[BIOS] Hang-On (USA, Europe) (v3.4)",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x08,0xb8,0x1a,0xa6,0xbe,0x18,0xb9,0x2d,0xae,0xf1,0xb8,0x75,0xde,0xec,0xf8,0x24},
        "[BIOS] Missile Defense 3-D (USA, Europe) (v4.4)",sms_rom,
        {{-1}}
    },
    {
        8*KB,
        {0x24,0xa5,0x19,0xc5,0x3f,0x67,0xb0,0x06,0x40,0xd0,0x04,0x8e,0xf7,0x08,0x91,0x05},
        "[BIOS] Sega Master System (Japan) (v2.1)",sms_rom,
        {{-1}}
    },
    {
        8*KB,
        {0x31,0x53,0x44,0x7d,0x15,0x5f,0xb8,0xbf,0xc6,0x6a,0x4d,0x1c,0x41,0xe3,0x99,0xb9},
        "[BIOS] Sega Master System (USA) (M404) (Proto)",sms_rom,
        {{-1}}
    },
    {
        16*KB,
        {0xb2,0x64,0xbe,0xf9,0xbd,0xa2,0x64,0xff,0xe8,0x3a,0xfc,0xeb,0xac,0x21,0xb8,0x1f},
        "[BIOS] Sega Master System (USA) (Store Display Unit)",sms_rom,
        {{-1}}
    },
    {
        8*KB,
        {0x84,0x04,0x81,0x17,0x72,0x70,0xd5,0x64,0x2a,0x14,0xca,0x71,0xee,0x72,0x84,0x4c},
        "[BIOS] Sega Master System (USA) (v1.3) [b]",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x8e,0x8b,0x04,0xb5,0xa9,0xfe,0x7b,0x6c,0xdb,0x25,0xb6,0x55,0x0e,0x7a,0x83,0x1e},
        "Argos no Juujiken (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0xa7,0x46,0x2d,0x79,0x84,0x4a,0x12,0xa9,0x29,0x41,0x17,0x42,0x52,0xc9,0xca,0x6e},
        "Astro Flash (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x2d,0x59,0xb3,0xd2,0x7f,0x02,0x2b,0x04,0xa5,0x97,0xe7,0x67,0xe6,0x66,0x0c,0xa9},
        "Comical Machine Gun Joe (Japan)",sms_rom,
        {{-1}}
    },
    {
        432*KB,
        {0x3a,0xd7,0xb3,0xe2,0x34,0x00,0xee,0x10,0xb7,0x23,0x9e,0x22,0xd4,0x5a,0xca,0x4e},
        "Dallyeora Pigu-Wang (Korea) (Unl)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x60,0xe4,0xb9,0xcf,0x6c,0x00,0x0e,0xb3,0xaf,0x97,0xe7,0x58,0x41,0x3c,0xe9,0x9f},
        "F-16 Fighting Falcon (Japan)",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x3a,0x1d,0xfa,0xd2,0xce,0xa1,0x11,0xd0,0xdb,0xfb,0x4a,0x54,0x5b,0xf3,0xf8,0x7e},
        "Fantasy Zone (Japan)",sms_rom,
        {{-1}}
    },
    {
        256*KB,
        {0x35,0x73,0xa6,0x59,0x21,0xb0,0x50,0x6f,0x71,0x24,0xf1,0xf9,0xff,0xb3,0x09,0x74},
        "Final Bubble Bobble (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x20,0x9e,0xe3,0x7d,0xca,0xbc,0x26,0x3a,0xa4,0x62,0xc7,0x81,0xd3,0x12,0x3f,0xce},
        "Fushigi no Oshiro Pit Pot (Japan)",sms_rom,
        {{-1}}
    },
    {
        512*KB,
        {0x8a,0x37,0x9e,0x0f,0x81,0x2f,0x1b,0x78,0x7b,0x71,0x26,0xe4,0x17,0x3f,0xbe,0x36},
        "Gaegujangi Ggachi (Korea) (Unl)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x89,0x02,0x8b,0xfb,0x15,0x2a,0x55,0x3a,0xdf,0x25,0x66,0x86,0x49,0xd3,0x9c,0xa6},
        "Ghost House (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x94,0xca,0x79,0xd4,0xeb,0x27,0x09,0xad,0x98,0xb8,0x50,0xd3,0x37,0x28,0xa0,0x45},
        "Great Baseball (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x0e,0xd8,0x83,0x30,0x2e,0x87,0xca,0x46,0xc1,0xc1,0xa5,0x56,0x60,0xe1,0x79,0x00},
        "Great Soccer (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x67,0x84,0x9e,0x73,0x4a,0x13,0x04,0x79,0x96,0x06,0xba,0x5a,0x22,0xf8,0x40,0xd2},
        "Hang-On (Japan)",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x8d,0xcd,0xd8,0x3e,0x58,0xbe,0x63,0x4a,0x37,0x35,0x91,0x1a,0x1a,0xf0,0x5b,0xb3},
        "Loretta no Shouzou (Japan)",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0x19,0xdb,0x67,0xf8,0x7b,0xb3,0xd1,0xd8,0xef,0xea,0xd0,0x54,0x2d,0xe3,0x0c,0xd1},
        "Mahjong Sengoku Jidai (Japan) (Beta)",sms_rom,
        {{-1}}
    },
    {
        512*KB,
        {0xf8,0x53,0xb7,0xdd,0xca,0x63,0x86,0x47,0x35,0xc0,0x30,0x01,0xc9,0xac,0x47,0x7b},
        "Phantasy Star (Korea)",sms_rom,
        {{-1}}
    },
    {
        1*MB,
        {0x51,0x7c,0x37,0x1a,0x92,0x9a,0xb0,0x12,0xee,0x95,0xa2,0x08,0x43,0xc5,0xf1,0xc2},
        "Sangokushi 3 (Korea) (Unl)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0xea,0xc6,0xa8,0x43,0x97,0x5c,0x50,0xe3,0x9e,0xb3,0x2f,0x76,0x4d,0xa2,0xa5,0xac},
        "Satellite 7 (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0xfb,0x2a,0xd2,0x52,0x46,0x46,0xbd,0x06,0x9e,0x3e,0xc1,0xb5,0x1f,0xd7,0xeb,0x39},
        "Seishun Scandal (Japan)",sms_rom,
        {{-1}}
    },
    {
        128*KB,
        {0xcf,0xa8,0x48,0x06,0x07,0x5e,0x01,0x7e,0x29,0xff,0xcd,0xd0,0x12,0x7f,0xe6,0x47},
        "Solomon no Kagi - Oujo Rihita no Namida (Japan)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0x2a,0x6e,0xe7,0x8e,0x26,0x17,0x88,0x6f,0xe5,0x40,0xdc,0xb8,0xa1,0x50,0x0e,0x90},
        "Spy vs Spy (Japan)",sms_rom,
        {{-1}}
    },
    {
        48*KB,
        {0x3b,0x0b,0x80,0xce,0xf0,0x2a,0x15,0xa7,0xef,0x09,0xf5,0x4e,0xb2,0x22,0x82,0xa7},
        "Teddy Boy Blues (Japan) (Proto) (Ep-MyCard)",sms_rom,
        {{-1}}
    },
    {
        32*KB,
        {0xde,0x5d,0x6c,0x9e,0x13,0x49,0x84,0x4b,0x74,0xf5,0x3c,0xaa,0xf2,0xaf,0x68,0x0b},
        "Teddy Boy Blues (Japan)",sms_rom,
        {{-1}}
    },
};

const int auto_patch_table_size=sizeof(auto_patch_table)/sizeof(auto_patch_entry);

// #########################################################################

save_entry save_table[]=   // http://code.google.com/p/genplus-gx/source/checkout
{                          // http://gxdev.wordpress.com/category/genesis-plus/
    /* ACCLAIM mappers */
    /* 24C02 (old mapper) */
    {"GM T-081326"   ,0     ,1,0},   /* NBA Jam (UE) */
    {"GM T-81033"    ,0     ,1,0},   /* NBA Jam (J) */

    /* 24C02 */
    {"GM T-81406"    ,0     ,1,0},   /* NBA Jam TE */
    {"GM T-081276"   ,0     ,1,0},   /* NFL Quarterback Club */
    /* 24C16 */
    {"GM T-081586"   ,0     ,1,0},   /* NFL Quarterback Club '96 */
    /* 24C65 */
    {"GM T-81576"    ,0     ,1,0},   /* College Slam */
    {"GM T-81476"    ,0     ,1,0},   /* Frank Thomas Big Hurt Baseball */

    /* EA mapper (24C01 only) */
    {"GM T-50176"    ,0     ,1,0},   /* Rings of Power */
    {"GM T-50396"    ,0     ,1,0},   /* NHLPA Hockey 93 */
    {"GM T-50446"    ,0     ,1,0},   /* John Madden Football 93 */
    {"GM T-50516"    ,0     ,1,0},   /* John Madden Football 93 (Championship Ed.) */
    {"GM T-50606"    ,0     ,1,0},   /* Bill Walsh College Football */

    /* SEGA mapper (24C01 only) */
    {"GM T-12046"    ,0     ,1,0},   /* Megaman - The Wily Wars */
    {"GM T-12053"    ,0xEA80,1,0},   /* Rockman Mega World (J) [A] */
    {"GM MK-1215"    ,0     ,1,0},   /* Evander 'Real Deal' Holyfield's Boxing */
    {"GM MK-1228"    ,0     ,1,0},   /* Greatest Heavyweights of the Ring (U) */
    {"GM G-5538"     ,0     ,1,0},   /* Greatest Heavyweights of the Ring (J) */
    {"GM GM PR-1993" ,0     ,1,0},   /* Greatest Heavyweights of the Ring (E) */
    {"GM G-4060"     ,0     ,1,0},   /* Wonderboy in Monster World */
    {"GM 00001211-00",0     ,1,0},   /* Sports Talk Baseball */
    {"GM_00004076-00",0     ,1,0},   /* Honoo no Toukyuuji Dodge Danpei */

    /* CODEMASTERS mapper */
    /* 24C01 */
    {"GM T-120106"   ,0     ,1,0},   /* Brian Lara Cricket */
    /* 24C08 */
    {"GM T-120096"   ,0     ,1,0},   /* Micro Machines 2 - Turbo Tournament (E) */
    {"GM 00000000-00",0x168B,1,0},   /* Micro Machines Military */
    {"GM 00000000-00",0xCEE0,1,0},   /* Micro Machines Military (Bad)*/
    /* 24C16 */
    {"GM 00000000-00",0x165E,1,0},   /* Micro Machines Turbo Tournament 96 */
    {"GM 00000000-00",0x2C41,1,0},   /* Micro Machines Turbo Tournament 96 (Bad)*/
    /* 24C65 */
    {"GM T-120146-50",0     ,1,0},   /* Brian Lara Cricket 96, Shane Warne Cricket */

    /* SRAM */
    {"GM T-50286 -00",0 ,0,16*KB},   /* Buck Rogers - Countdown to Doomsday (USA, Europe) */
    {"GM T-50086 -01",0 ,0,16*KB},   /* PGA Tour Golf (USA, Europe) (v1.1) */
    {"GM T-50086 -02",0 ,0,16*KB},   /* PGA Tour Golf (USA, Europe) (v1.2) */
    {"GM T-26013 -00",0 ,0,16*KB},   /* Psy-O-Blade (Japan) */
    {"GM ACLD007 -00",0 ,0, 4*KB},   /* Winter Challenge (USA, Europe) */

    {"GM T-50216 -00",0 ,0,16*KB},   /* Starflight (USA, Europe) */
    {"GM T-50216 -01",0 ,0,16*KB},   /* Starflight (USA, Europe) (v1.1) */

//    {"",0  ,0,16*KB},   /*  */
};

const int save_table_size=sizeof(save_table)/sizeof(save_entry);

// #########################################################################

sms_save_entry sms_save_table[]=
{
    {
        256*KB,
        {0x8f,0xe5,0x5a,0xde,0x7b,0x34,0xaa,0x4e,0x42,0xc5,0xf6,0x3f,0x43,0x68,0x49,0x7f},
        "Desert Speedtrap Starring Road Runner and Wile E. Coyote (Europe) (En,Fr,De,Es,It)",
        8*KB,
    },
    {
        128*KB,
        {0xff,0x75,0x02,0xdd,0x8a,0x71,0x7d,0xb5,0xad,0xb4,0x2c,0x71,0x1d,0xdb,0xc9,0xf5},
        "Doki Doki Penguin Land - Uchuu Daibouken (Japan)",
        8*KB,
    },
    {
        256*KB,
        {0xd4,0x6e,0x40,0xbb,0xb7,0x29,0xba,0x23,0x3f,0x17,0x1a,0xd7,0xbf,0x61,0x69,0xf5},
        "Golden Axe Warrior (USA, Europe)",
        8*KB,
    },
    {
        256*KB,
        {0x1f,0xa3,0x30,0xa1,0x7d,0xf1,0x98,0x20,0xf4,0xc8,0xe0,0x23,0x0d,0x3c,0xfb,0xe2},
        "Golfamania (Europe) (Beta)",
        8*KB,
    },
    {
        256*KB,
        {0x55,0x54,0xf9,0xf6,0x71,0x03,0xe0,0xd5,0xb1,0xa4,0x07,0x5a,0x2d,0x26,0xd1,0x5b},
        "Golfamania (Europe)",
        8*KB,
    },
    {
        128*KB,
        {0x8d,0xde,0xc5,0x89,0xf7,0x2c,0xdc,0xf2,0xcd,0x4c,0xaa,0xfb,0x07,0x5e,0xc8,0xe4},
        "Penguin Land (USA, Europe)",
        8*KB,
    },
    {
        512*KB,
        {0x1b,0x69,0x71,0x6f,0x9f,0x40,0x53,0xe1,0x53,0x3f,0x65,0x4c,0x09,0x1a,0xe4,0x10},
        "Phantasy Star (Brazil)",
        8*KB,
    },
    {
        512*KB,
        {0xa0,0xf6,0x14,0xf2,0x6e,0x99,0x63,0x34,0x93,0xe3,0xa3,0x33,0x9b,0xc8,0x55,0x86},
        "Phantasy Star (Japan) [En by SMS Power v1.02]",
        8*KB,
    },
    {
        512*KB,
        {0xdf,0xeb,0xc4,0x8d,0xfe,0x81,0x65,0x20,0x2b,0x7f,0x00,0x2d,0x8b,0xac,0x47,0x7b},
        "Phantasy Star (Japan)",
        8*KB,
    },
    {
        512*KB,
        {0xf8,0x53,0xb7,0xdd,0xca,0x63,0x86,0x47,0x35,0xc0,0x30,0x01,0xc9,0xac,0x47,0x7b},
        "Phantasy Star (Korea)",
        8*KB,
    },
    {
        512*KB,
        {0x35,0x9a,0xe4,0xa1,0x69,0x3e,0x99,0x6d,0x42,0x19,0xb9,0x26,0x95,0x41,0xc7,0x6b},
        "Phantasy Star (USA, Europe) (v1.2) [Hack by Komrade v1.44Lutz] (~Phantasy Star - Hordes of Nei)",
        8*KB,
    },
    {
        512*KB,
        {0x5b,0xa9,0x11,0x4e,0xde,0xa5,0xde,0xb5,0x28,0x2f,0xd9,0xad,0x7d,0x4b,0x2d,0x62},
        "Phantasy Star (USA, Europe) (v1.2)",
        8*KB,
    },
    {
        512*KB,
        {0xce,0x2c,0x9a,0xae,0x1f,0x48,0xaf,0x31,0x0e,0x57,0x67,0x43,0x67,0x35,0xec,0xfe},
        "Phantasy Star (USA, Europe) (v1.3) [Hack by Komrade v1.44Lutz] (~Phantasy Star - Hordes of Nei)",
        8*KB,
    },
    {
        512*KB,
        {0x11,0x10,0x93,0x8d,0xf8,0x0f,0x4e,0x44,0xc8,0x21,0x3d,0x7f,0x85,0xcf,0xb5,0xe6},
        "Phantasy Star (USA, Europe) (v1.3)",
        8*KB,
    },
    {
        512*KB,
        {0xd9,0x2b,0x72,0xc8,0x71,0x79,0xb7,0x18,0xc8,0xb1,0x50,0x6b,0x34,0x7c,0x2e,0x36},
        "Ultima IV - Quest of the Avatar (Europe) (Beta)",
        8*KB,
    },
    {
        512*KB,
        {0xc5,0x4c,0x1a,0x25,0x59,0xce,0xc1,0x89,0xf5,0x3e,0x79,0x41,0x77,0xd0,0x02,0xaf},
        "Ultima IV - Quest of the Avatar (Europe)",
        8*KB,
    },
    {
        256*KB,
        {0x17,0x36,0x41,0x62,0xda,0x3c,0xce,0xe8,0x47,0x85,0x0d,0xd9,0xb4,0x03,0x76,0x5c},
        "Ys (Japan)",
        8*KB,
    },
    {
        256*KB,
        {0xdd,0x5d,0x01,0xd4,0xe1,0xfe,0x7d,0x27,0x5f,0x95,0xac,0x95,0xdc,0x63,0xb0,0x4f},
        "Ys - The Vanished Omens (USA, Europe) [Sound Hack by Hungdongs v1.2] (FM Sound Restoration)",
        8*KB,
    },
    {
        256*KB,
        {0x77,0xd2,0xff,0x05,0x44,0x29,0xf7,0x97,0x01,0x34,0xe6,0x78,0x00,0x53,0x9a,0x78},
        "Ys - The Vanished Omens (USA, Europe)",
        8*KB,
    }
};

const int sms_save_table_size=sizeof(sms_save_table)/sizeof(sms_save_entry);

// #########################################################################

int compareHeaderString(void*data,int size,int offset,const char*str)
{
    BYTE*romdata=(BYTE*)data;
    int l=strlen(str);
    if(size>(offset+l))
    {
        if(memcmp(romdata+offset,str,l)==0)
        {
            return 1;
        }
    }
    return 0;
}

// #########################################################################

int ap_sortbysize(void*data,DWORD a,DWORD b)
{
    auto_patch_entry*ape=(auto_patch_entry*)data;
    if(ape[a].size>ape[b].size)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

int autoPatchRom(void*data,int size)
{
    static int ap_sorted=0;
    static DWORD auto_patch_table_index[auto_patch_table_size];
    if(ap_sorted==0)
    {
        ap_sorted=1;
        for(int i=0;i<auto_patch_table_size;i++)
        {
            auto_patch_table_index[i]=i;
        }
        quickSort(auto_patch_table_index,0,auto_patch_table_size-1,ap_sortbysize,auto_patch_table);
    }
    md5struct md5;
    int md5size=0;
    for(int i=0;i<auto_patch_table_size;i++)
    {
        if(size>=sorted(auto_patch_table,i).size)
        {
            if(md5size!=sorted(auto_patch_table,i).size)
            {
                if(md5size==0||md5size>sorted(auto_patch_table,i).size)
                {
                    getmd5(data,sorted(auto_patch_table,i).size,md5);
                }
                else
                {
                    updatemd5(((BYTE*)data)+md5size,sorted(auto_patch_table,i).size-md5size,md5);
                }
                md5size=sorted(auto_patch_table,i).size;
            }
            if(memcmp(md5,sorted(auto_patch_table,i).md5,16)==0)
            {
                if(getoption(enableKnownPatches))
                {
                    if(sorted(auto_patch_table,i).patch[0].offset!=-1)
                    {
                        for(int j=0;sorted(auto_patch_table,i).patch[j].offset!=-1;j++)
                        {
                            //printf("\n\toffset: %.4x\tvalue: %.4x\n",sorted(auto_patch_table,i).patch[j].offset,sorted(auto_patch_table,i).patch[j].value);
                            store_word_be(sorted(auto_patch_table,i).patch[j].value,((BYTE*)data)+sorted(auto_patch_table,i).patch[j].offset);
                        }
                        printf("\tauto patched: %s\n",sorted(auto_patch_table,i).name);
                    }
                    if(getoption(savePatchedFiles))
                    {
                        char rname[1024];
                        sprintf(rname,"%s_auto_patched.bin",sorted(auto_patch_table,i).name);
                        SaveFile(rname,(char*)data,size);
                    }
                }
                if(sorted(auto_patch_table,i).romtype==scd_bios)
                {
                    if((size>(sorted(auto_patch_table,i).size+16))&&(memcmp(((BYTE*)data)+sorted(auto_patch_table,i).size,"\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04",16)==0))
                    {
                        return scd_bios_bram;
                    }
                }
                return sorted(auto_patch_table,i).romtype;
            }
        }
    }
    return unknown_rom;
}

// #########################################################################

int decodeSmdRom(void*data,int size,membuf&out,int dn,int&ns)
{
    char*romdata=(char*)data;
    ns=0;
    if(size<(16*KB+0x200))
    {
        return 0;
    }
    if(size%(16*KB)!=0x200)
    {
        return 0;
    }
    size-=0x200;
    if(dn==0)
    {
        dn=size;
    }
    if(dn>size)
    {
        return 0;
    }
    if(!out.resize(size))
    {
        return 0;
    }
    int dp;
    const int tb=size/(16*KB);
    dp=0;
    for(int i=0;i<tb&&dp<dn;i++)
    {
        for(int j=0;j<8*KB&&dp<dn;j++)
        {
            out[i*16*KB+j*2+0]=romdata[0x200+i*16*KB+0*KB+j];
            out[i*16*KB+j*2+1]=romdata[0x200+i*16*KB+8*KB+j];
            dp+=2;
        }
    }
    ns=dp;
    if(memcmp(out+0x100,"SEGA",4)==0||memcmp(out+0x101,"SEGA",4)==0)
    {
        return 1;
    }
    dp=0;
    for(int i=0;i<tb&&dp<dn;i++)
    {
        for(int j=0;j<8*KB&&dp<dn;j++)
        {
            out[i*16*KB+j*2+1]=romdata[0x200+i*16*KB+0*KB+j];
            out[i*16*KB+j*2+0]=romdata[0x200+i*16*KB+8*KB+j];
            dp+=2;
        }
    }
    ns=dp;
    if(memcmp(out+0x100,"SEGA",4)==0||memcmp(out+0x101,"SEGA",4)==0)
    {
        return 1;
    }
    return 0;
}

// #########################################################################

void fixByteOrder(void*data,int size)
{
    BYTE*romdata=(BYTE*)data;
    for(int i=0;i<size;i+=2)
    {
        BYTE c=romdata[i];
        romdata[i]=romdata[i+1];
        romdata[i+1]=c;
    }
}

void fixWordOrder(void*data,int size)
{
    WORD*romdata=(WORD*)data;
    for(int i=0;i<(size/2);i+=2)
    {
        WORD c=romdata[i];
        romdata[i]=romdata[i+1];
        romdata[i+1]=c;
    }
}

void fix4ByteOrder(void*data,int size)
{
    BYTE*romdata=(BYTE*)data,c;
    for(int i=0;i<size;i+=4)
    {
        c=romdata[i+0];
        romdata[i+0]=romdata[i+3];
        romdata[i+3]=c;
        c=romdata[i+1];
        romdata[i+1]=romdata[i+2];
        romdata[i+2]=c;
    }
}

// #########################################################################

int getRomType(void*data,int size)
{
    BYTE*romdata=(BYTE*)data;
#if 0 //z64
    if(memcmp(romdata,"\x80\x37\x12\x40",4)==0)
    {
        return n64_rom;
    }
    if(memcmp(romdata,"\x37\x80\x40\x12",4)==0)
    {
        fixByteOrder(data,size);
        return n64_rom;
    }
    if(memcmp(romdata,"\x40\x12\x37\x80",4)==0)
    {
        fix4ByteOrder(data,size);
        return n64_rom;
    }
#else //v64
    if(memcmp(romdata,"\x37\x80\x40\x12",4)==0)
    {
        return n64_rom;
    }
    if(memcmp(romdata,"\x80\x37\x12\x40",4)==0)
    {
        fixByteOrder(data,size);
        return n64_rom;
    }
    if(memcmp(romdata,"\x40\x12\x37\x80",4)==0)
    {
        fixWordOrder(data,size);
        return n64_rom;
    }
#endif
    if(memcmp(romdata+0x100,"ESAG",4)==0)
    {
        fixByteOrder(data,size);
        //**/SaveFile("fixed.bin",(char*)data,size);
    }
    int ap=autoPatchRom(data,size);
    if(ap!=unknown_rom)
    {
        return ap;
    }
    if(compareHeaderString(data,size,0x120,"CDX BOOT ROM")||
       compareHeaderString(data,size,0x120,"CD2 BOOT ROM")||
       compareHeaderString(data,size,0x120,"MEGA-LD BOOT ROM")||
       compareHeaderString(data,size,0x120,"MEGA-CD BOOT ROM")||
       compareHeaderString(data,size,0x120,"SEGA-CD BOOT ROM")||
       compareHeaderString(data,size,0x120,"WONDER-MEGA BOOT")||
       compareHeaderString(data,size,0x120,"WONDERMEGA2 BOOT"))
    {
        if(size>128*KB)
        {
            DWORD romsize=0;
            load_dword_be(romdata+0x1a4,romsize);
            romsize++;
            if((size>(int)(romsize+16))&&(memcmp(romdata+romsize,"\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04",16)==0))
            {
                return scd_bios_bram;
            }
        }
        return scd_bios;
    }
    if(compareHeaderString(data,size,0,"\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04\xff\x04"))
    {
        return scd_bram;
    }
    if(compareHeaderString(data,size,0x7FF0,"TMR SEGA"))
    {
        return sms_rom;
    }
    if(size>0x203&&(memcmp(romdata+0x100,"SEGA",4)==0||memcmp(romdata+0x101,"SEGA",4)==0))
    {
        if((romdata[0x203]==0x88)||(romdata[0x203]==0x90)||(romdata[0x203]==0x91)||compareHeaderString(data,size,0x120,"MARS SAMPLE PROGRAM"))
        {
            if(romdata[0x200]!=0)
            {
                return s32x_rom;
            }
        }
        return md_rom;
    }
    membuf romdhdr;
    int ns;
    if(decodeSmdRom(data,size,romdhdr,0x300,ns))
    {
        if(memcmp(romdhdr+0x100,"SEGA",4)==0||memcmp(romdhdr+0x101,"SEGA",4)==0)
        {
            return smd_rom;
        }
    }
    if(memcmp(data,"Vgm ",4)==0)
    {
        return vgm_file;
    }
    return unknown_rom;
}

// #########################################################################

int getSaveSize(int romtype,void*data,int size,int&even)
{
    BYTE*romdata=(BYTE*)data;
    even=both_bytes;
    if(romtype==md_rom||romtype==s32x_rom)
    {
        even=odd_bytes;//malformed headers?
        if(size>0x1bf)
        {
            if(size>(2*MB+0x200))
            {
                if(compareHeaderString(data,size,0x180,"GM MK-1563 -00"))
                {
                    /*
                    s&k "GM MK-1563 -00"
                    s1  "GM 00001009-00" "GM 00004049-01"
                    s2  "GM 00001051-00" "GM 00001051-01" "GM 00004049-01"
                    s3  "GM MK-1079 -00"
                    */
                    int ss;
                    ss=getSaveSize(romtype,romdata,2*MB,even);
                    if(ss>0)
                    {
                        return ss;
                    }
                    ss=getSaveSize(romtype,romdata+2*MB,size-2*MB,even);
                    if(ss>0)
                    {
                        return ss;
                    }
                    if(compareHeaderString(data,size,2*MB+0x180,"GM MK-1079 -00"))
                    {
                        even=odd_bytes;
                        return 1*KB;
                    }
                    return 0;
                }
            }
            for(int i=0;i<save_table_size;i++)
            {
                if(compareHeaderString(data,size,0x180,save_table[i].game_id))
                {
                    WORD chk;
                    load_word_be(romdata+0x18e,chk);
                    if((save_table[i].chk==0)||(save_table[i].chk==chk))
                    {
                        if(compareHeaderString(data,size,0x180,"GM 00000000-00")&&(memcmp(romdata,"DNLD",4)!=0))
                        {
                            return 0;
                        }
                        if(save_table[i].eeprom==1)
                        {
                            return 1;
                        }
                        else
                        {
                            even=odd_bytes;
                            return save_table[i].ssize;
                        }
                    }
                }
            }
            if((romdata[0x1b0]==0x52)&&(romdata[0x1b1]==0x41))
            {
                DWORD begin,end;
                load_dword_be(romdata+0x1b4,begin);
                load_dword_be(romdata+0x1b8,end);
                if(end<begin)
                {
                    return 64*KB;
                }
                else
                {
                    //even=romdata[0x1b2]==0x20?odd_bytes:(romdata[0x1b2]>>3)&3;
                    if((end-begin)<2)
                    {
                        return 1;
                    }
                    int ssize=(end-begin+2)&(~1);
                    ssize=Min(ssize,64*KB);
                    return ssize;
                }
            }
        }
    }
    if(romtype==md_rom)
    {
        if(size==4*MB)
        {
            const md5struct x_md5[]=
            {
                {0x52,0x8e,0x17,0x12,0x64,0xeb,0xfd,0x90,0xe5,0x3f,0xd9,0x66,0x7d,0x1c,0x61,0xd1},//Xin Qi Gai Wang Zi (China) (Unl)
                {0x80,0x94,0xc9,0xa1,0xc0,0xcf,0x82,0xb2,0x66,0x21,0xa3,0x82,0xd3,0xee,0x47,0x24},//Xin Qi Gai Wang Zi (China) (Alt) (Unl)
            };
            md5struct md5;
            getmd5(data,size,md5);
            if(memcmp(md5,x_md5[0],16)==0||memcmp(md5,x_md5[1],16)==0)
            {
                even=odd_bytes;
                return 64*KB;
            }
        }
    }
    if(romtype==scd_bram||romtype==scd_bios_bram)
    {
        return 64*KB;
    }
    if(romtype==sms_rom)
    {
        static int sms_sorted=0;
        static DWORD sms_save_table_index[sms_save_table_size];
        if(sms_sorted==0)
        {
            sms_sorted=1;
            for(int i=0;i<sms_save_table_size;i++)
            {
                sms_save_table_index[i]=i;
            }
            quickSort(sms_save_table_index,0,sms_save_table_size-1,ap_sortbysize,sms_save_table);
        }
        md5struct md5;
        int md5size=0;
        for(int i=0;i<sms_save_table_size;i++)
        {
            if(size>=sorted(sms_save_table,i).size)
            {
                if(md5size!=sorted(sms_save_table,i).size)
                {
                    if(md5size==0||md5size>sorted(sms_save_table,i).size)
                    {
                        getmd5(data,sorted(sms_save_table,i).size,md5);
                    }
                    else
                    {
                        updatemd5(((BYTE*)data)+md5size,sorted(sms_save_table,i).size-md5size,md5);
                    }
                    md5size=sorted(sms_save_table,i).size;
                }
                if(memcmp(md5,sorted(sms_save_table,i).md5,16)==0)
                {
                    return sorted(sms_save_table,i).ssize;
                }
            }
        }
    }
    return 0;
}

// #########################################################################

void neo2n64(const BYTE*neo,membuf&n64,int&size)
{
    if(size)
    {
        const WORD*nd=(WORD*)neo;
        int fs_n64=size/16;
        //n64.resize(fs_n64);
        for(int i=0;i<fs_n64;i++)
        {
            BYTE tmp=0;
            for(DWORD j=0;j<8;j++)
            {
                tmp*=2;
                tmp|=(nd[i*8+j]!=0)?1:0;
            }
            ((BYTE*)(char*)n64)[i]=tmp;
        }
        size=fs_n64;
    }
}

// #########################################################################

void n642neo(const BYTE*n64,membuf&neo,int&size)
{
    if(size)
    {
        int fs_neo=size*16;
        //neo.resize(fs_neo);
        WORD*nd=(WORD*)(char*)neo;
        for(int i=0;i<size;i++)
        {
            BYTE tmp=n64[i];
            for(int j=0;j<8;j++)
            {
                nd[i*8+j]=(tmp&0x80)?0xffff:0;
                tmp*=2;
            }
        }
        size=fs_neo;
    }
}

// #########################################################################

void wswap(BYTE*data,int fs)
{
    if(fs)
    {
        WORD*nd=(WORD*)data;
        for(int i=0;i<fs/4;i++)
        {
            WORD tmp=nd[i*2+0];
            nd[i*2+0]=nd[i*2+1];
            nd[i*2+1]=tmp;
        }
    }
}

// #########################################################################

void convertSave2neo(int savetype,const BYTE*data_in,membuf&data_out,int&size)
{
    if(savetype==st_n64eep4k)
    {
        data_out.resize(st_n64eep4k*16);
        memset(data_out,0,st_n64eep4k*16);
        size=Min(size,st_n64eep4k);
        n642neo(data_in,data_out,size);
        size=st_n64eep4k*16;
    }
    if(savetype==st_n64eep16k)
    {
        data_out.resize(st_n64eep16k*16);
        memset(data_out,0,st_n64eep16k*16);
        size=Min(size,st_n64eep16k);
        n642neo(data_in,data_out,size);
        size=st_n64eep16k*16;
    }
    if(savetype==st_n64sram)
    {
        data_out.resize(st_n64sram);
        memset(data_out,0,st_n64sram);
        size=Min(size,st_n64sram);
        memcpy(data_out,data_in,size);
        wswap((BYTE*)(char*)data_out,size);
        size=st_n64sram;
    }
    if(savetype==st_n64flash)
    {
        data_out.resize(st_n64flash);
        memset(data_out,0,st_n64flash);
        size=Min(size,st_n64flash);
        memcpy(data_out,data_in,size);
        wswap((BYTE*)(char*)data_out,size);
        size=st_n64flash;
    }
}

// #########################################################################

void convertSave2n64(int savetype,const BYTE*data_in,membuf&data_out,int&size)
{
    if(savetype==st_n64eep4k)
    {
        data_out.resize(st_n64eep4k);
        memset(data_out,0,st_n64eep4k);
        size=Min(size,st_n64eep4k*16);
        neo2n64(data_in,data_out,size);
        size=st_n64eep4k;
    }
    if(savetype==st_n64eep16k)
    {
        data_out.resize(st_n64eep16k);
        memset(data_out,0,st_n64eep16k);
        size=Min(size,st_n64eep16k*16);
        neo2n64(data_in,data_out,size);
        size=st_n64eep16k;
    }
    if(savetype==st_n64sram)
    {
        data_out.resize(st_n64sram);
        memset(data_out,0,st_n64sram);
        size=Min(size,st_n64sram);
        memcpy(data_out,data_in,size);
        wswap((BYTE*)(char*)data_out,size);
        size=st_n64sram;
    }
    if(savetype==st_n64flash)
    {
        data_out.resize(st_n64flash);
        memset(data_out,0,st_n64flash);
        size=Min(size,st_n64flash);
        memcpy(data_out,data_in,size);
        wswap((BYTE*)(char*)data_out,size);
        size=st_n64flash;
    }
}

// #########################################################################
