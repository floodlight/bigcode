/************************************************************
 * <bsn.cl fy=2013 v=epl>
 * 
 *        Copyright 2013, 2014 Big Switch Networks, Inc.       
 * 
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * 
 *        http://www.eclipse.org/legal/epl-v10.html
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 * 
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/

#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "orc/orc_logger.h"
#include "orc/options.h"

static char * Ifnam_Prefix = "orc";
static char * Default_Driver = "orc_brcm.so";
static char * Driver_Search_Paths[] = {
        "/usr/share/orc/drivers",
        NULL
    };

void usage(char * msg1, char * msg2)
{
    orc_options_t defaults;
    options_init_default( &defaults);
    if (msg1)
        fprintf(stderr, "%s ", msg1);
    if (msg2)
        fprintf(stderr, "%s ", msg2);
    if (msg1 || msg2)
        fprintf(stderr, "\n");
    fprintf(stderr, "Usage: orc [options] \n"
        "-d, --driver=DRIVER_NAME  -- ASIC driver to load (\"%s\") \n"
        "-D, --debug[=val]         -- Enable debugging (%s)\n"
        "-h, --help                -- Print this message\n"
        "-i, --interface-prefix     -- Virtual interface prefix (\"%s\") \n"
        "-p, --dpath=path          -- Add a path to the driver search path\n"
        "-P, --ports-only          -- Only create virtual ports and do TX/RX - no L3\n"
        "-V, --verbose             -- Enable verbose opertations\n",
        defaults.driver,
        orc_log_threshold(),
        Ifnam_Prefix);

    fprintf(stderr, "\n\n   Default driver search path:\n");
    int i;
    for (i=0; i < defaults.num_driver_paths; i++)
        fprintf(stderr, "          Path: %s\n", defaults.driver_paths[i]);
    fprintf(stderr, "\n\n   Valid Logging Levels:\n");
    for (i=0; i < ORC_LOG_MAX; i++)
        fprintf(stderr, "          Log Level: %s == %d\n",
                        orc_log_threshold_level(i), i);
    /* and now exit... */
    exit(1);
}


#define OPT_STRING "d:D::hp:P:i:V"
static struct option long_options[] = {
        {"driver",              required_argument, 0, 'd' },
        {"debug",               optional_argument, 0, 'D'},
        {"help",                optional_argument, 0, 'h'},
        {"interface-prefix",    required_argument, 0, 'i' },
        {"dpath",               required_argument, 0, 'p' },
        {"verbose",             no_argument,       0, 'V' },
        {"ports-only",          no_argument,    0, 'P' },
        {0,         0,                 0,  0 }
};


int options_init_default( orc_options_t * options)
{
    int i;

    memset(options, 0, sizeof(*options));
    options->ifnam_prefix = Ifnam_Prefix;
    options->driver       = Default_Driver;
    options->debug        = DEBUG_THRESHOLD;
    options->drv          = NULL;
    orc_debug("Setting default driver to \"%s\"\n", options->driver);
    for (i=0; Driver_Search_Paths[i] != NULL; i++)
    {
        options->driver_paths[i] = Driver_Search_Paths[i];
        orc_debug("Adding to default driver search path: \"%s\"\n",
                        options->driver_paths[i]);
    }
    options->num_driver_paths = i;
    return 0;
}



int options_parse( orc_options_t * options, char * argv[], int argc)
{
    int c;
    int opt_index = 0;
    int done = 0;
    while ( !done )
    {
        c = getopt_long(argc, argv, OPT_STRING, long_options, &opt_index);
        switch (c)
        {
            case 'd':
                    options->driver = optarg;
                    orc_log("Setting driver to \"%s\"\n", options->driver);
                    break;
            case 'D':
                    if (optarg != NULL)
                    {
                        int debug = atoi(optarg);
                        if (debug >= ORC_LOG_MAX)
                            usage("--debug: value out of range:", optarg);
                        options->debug = atoi(optarg);
                    }
                    else
                    {
                        options->debug = ORC_LOG_DEBUG;
                    }
                    DEBUG_THRESHOLD = options->debug;
                    orc_log("Setting Debug level to %s\n", orc_log_threshold());

                    break;
            case 'p':
                    if (options->num_driver_paths >= MAX_DRIVER_PATHS)
                        usage("Too many driver paths specified", NULL);
                    options->driver_paths[options->num_driver_paths++] =
                            optarg;
                    orc_log("Adding to driver search path: \"%s\"\n", optarg);
                    break;
            case 'P':
                    options->ports_only = 1;
                    break;
            case 'V':
                    options->verbose = 1;
                    break;
            case 'h':
                    usage("Help requested", NULL);
                    break;
            case 'i':
                    if (optarg == NULL)
                       usage("option missing interface prefix",NULL);
                    options->ifnam_prefix = strdup(optarg);
                    orc_info("Setting interface prefix to \"%s\"\n", options->ifnam_prefix);
                    break;
            case '?':
                    usage("Invalid option", argv[optind]);
                    break;
            case -1:
                    done = 1;
                    break;
        };

    }
    return opt_index + 1;
}

