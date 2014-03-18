/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <debug_counter/debug_counter_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

int aim_main(int argc, char* argv[])
{
    printf("debug_counter Utest Is Empty\n");
    debug_counter_config_show(&aim_pvs_stdout);
    return 0;
}

