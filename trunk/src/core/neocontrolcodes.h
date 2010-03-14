#ifndef neocontrolcodes_g
#define neocontrolcodes_g
// #########################################################################

#include <neoutil.h>

// #########################################################################

#define neo_sram_block_size (64*KB)

// #########################################################################

struct neocontrolcode
{
    const char*code;
    DWORD size;
    const char*display_name;
};

// #########################################################################

struct menu_info
{
    const char*id;
    neocontrolcode write2select_menu;
    neocontrolcode write2write_menu;
    DWORD menu_size;
    DWORD sram_size;
    DWORD smallblock_size;
    DWORD normalblock_size;
};

// #########################################################################

struct flash_info
{
    const char*id;
    neocontrolcode write2select_flash;
    neocontrolcode write2write_flash;
    DWORD nSelect_low;
    DWORD nSelect_high;
    DWORD nDies;
    DWORD flash_size;
    DWORD smallblock_size;
    DWORD normalblock_size;
};

// #########################################################################

extern const neocontrolcode neocontrolcodes[];
extern const menu_info menu_infos[];
extern const flash_info flash_infos[];

// #########################################################################
#endif
