/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <slot_allocator/slot_allocator_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

int aim_main(int argc, char* argv[])
{
    printf("slot_allocator Utest Is Empty\n");
    slot_allocator_config_show(&aim_pvs_stdout);
    return 0;
}

