#ifndef _NEO_SN64E_TYPES_H_
#define _NEO_SN64E_TYPES_H_

#ifndef __cplusplus
typedef enum NEON64_SaveType NEON64_SaveType;
typedef enum NEON64_RomFormat NEON64_RomFormat;
typedef enum NEON64_BootChip NEON64_BootChip;
typedef enum NEON64_Region NEON64_Region;
#endif

enum NEON64_RomFormat
{
    NEON64_FMT_Z64 = 0x1,
    NEON64_FMT_V64,//will be converted to z64(binary)
    NEON64_FMT_N64,//will be converted to z64(binary)
    NEON64_FMT_UNKNOWN
};

enum NEON64_SaveType
{
    NEON64_SAVE_CARD = 0x1,
    NEON64_SAVE_EEPROM_4K,
    NEON64_SAVE_EEPROM_16K,
    NEON64_SAVE_SRAM_32K,
    NEON64_SAVE_FLASHRAM_1M,
    NEON64_SAVE_CONTROLLERPACK,
    NEON64_SAVE_NONE
};

enum NEON64_BootChip
{
    NEON64_CIC_6101 = 0x1,
    NEON64_CIC_6102,
    NEON64_CIC_6103,
    NEON64_CIC_6105 = 5,
    NEON64_CIC_6106
};

enum NEON64_Region
{
    NEON64_REGION_DEFAULT = 0,
    NEON64_REGION_GERMANY,
    NEON64_REGION_FRENCH,
    NEON64_REGION_ITALIAN,
    NEON64_REGION_EUROPE,
    NEON64_REGION_SPANISH,
    NEON64_REGION_AUSTRALIA,
    NEON64_REGION_XPAL,
    NEON64_REGION_MODE7,
    NEON64_REGION_USA,
    NEON64_REGION_JAPAN,
    NEON64_REGION_COUNT
};

static const char* NEON64_REGION_STRING_TABLE[] =
{
    "Default\0",
    "Germany\0",
    "French\0",
    "Italian\0",
    "Europe\0",
    "Spanish\0",
    "Australia\0",
    "XPAL\0",
    "Mode7\0",
    "USA\0",
    "Japan\0"
};

static const unsigned char NEON64_REGION_CODE_TABLE[] =
{
    0x44,
    0x46,
    0x49,
    0x50,
    0x53,
    0x55,
    0x58,
    0x59,
    0x37,
    0x45,
    0x4a
};

#endif // _NEO_TYPES_H_
