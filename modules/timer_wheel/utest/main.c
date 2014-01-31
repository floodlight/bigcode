/**************************************************************************//**
 *
 *
 *
 *****************************************************************************/
#include <timer_wheel/timer_wheel_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AIM/aim.h>

int aim_main(int argc, char* argv[])
{
    printf("timer_wheel Utest Is Empty\n");
    timer_wheel_config_show(&aim_pvs_stdout);
    return 0;
}

