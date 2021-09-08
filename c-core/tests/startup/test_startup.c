/****************************************************************************
 *          test.c
 *
 *          Copyright (c) 2015 Niyamaka.
 *          All Rights Reserved.
 ****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <yuneta.h>
#include "c_root.h"

/***************************************************************************
 *                      Names
 ***************************************************************************/
#define APP_NAME        "test_startup"
#define APP_DOC         "Test"

#define APP_VERSION     "4.16.1"
#define APP_SUPPORT     "<niyamaka at yuneta.io>"
#define APP_DATETIME    __DATE__ " " __TIME__

/***************************************************************************
 *                      Default config
 ***************************************************************************/
PRIVATE char fixed_config[]= "\
{                                                               \n\
    'environment': {                                            \n\
    },                                                          \n\
    'yuno': {                                                   \n\
        'yuno_role': 'test_startup'                             \n\
    }                                                           \n\
}                                                               \n\
";

PRIVATE char variable_config[]= "\
{                                                               \n\
    '__json_config_variables__': {                              \n\
    },                                                          \n\
    'environment': {                                            \n\
        'work_dir': '.',                                        \n\
        'log_gbmem_info': true,                                 \n\
        'MEM_MIN_BLOCK': 32,                                    \n\
        'MEM_MAX_BLOCK': 262144,            #^^ 256*K           \n\
        'MEM_SUPERBLOCK': 524288,           #^^ 512*K           \n\
        'MEM_MAX_SYSTEM_MEMORY': 67108864,  #^^ 64*M            \n\
        'console_log_handlers': {                               \n\
            'to_stdout': {                                      \n\
                'handler_type': 'stdout',                       \n\
                'handler_options': 31                           \n\
            }                                                   \n\
        },                                                      \n\
        'daemon_log_handlers': {                                \n\
            'to_file': {                                        \n\
                'handler_type': 'file',                         \n\
                'filename_mask': 'yuneta_agent-W.log'       \n\
#^^            },                                                  \n\
#^^            'to_udp': {                                         \n\
#^^                'handler_type': 'udp',                          \n\
#^^                'url': 'udp://127.0.0.1:1000'                   \n\
            }                                                   \n\
        }                                                       \n\
    },                                                          \n\
    'yuno': {                                                   \n\
    },                                                          \n\
    'global': {                                                 \n\
    },                                                          \n\
    'services': [                                               \n\
    ]                                                           \n\
}                                                               \n\
";

/***************************************************************************
 *  Simulate
 ***************************************************************************/
PUBLIC void mt_run(hgobj gobj)
{
}
PUBLIC void mt_clean(hgobj gobj)
{
}
PUBLIC int yuneta_register_c_core(void)
{
    return 0;
}

/***************************************************************************
 *  Your yuno
 ***************************************************************************/
PRIVATE void register_test_yuno(void)
{
    gobj_register_yuno("test_startup", GCLASS_ROOT, FALSE);
}

/***************************************************************************
 *
 ***************************************************************************/
static inline struct timespec ts_diff (struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/***************************************************************************
 *
 ***************************************************************************/
static inline void test(int argc, char *argv[], int flags, uint64_t cnt)
{
    uint64_t i;
    struct timespec st, et, dt;

    /*------------------------------------------------*
     *  To trace memory
     *------------------------------------------------*/
//#ifdef DEBUG
    static uint32_t mem_list[] = {0};
    gbmem_trace_alloc_free(0, mem_list);
//#endif

    /*------------------------------------------------*
     *          Start yuneta
     *------------------------------------------------*/
    helper_quote2doublequote(fixed_config);
    helper_quote2doublequote(variable_config);


    switch(flags) {
    case 0:
        break;
    case 1:
        break;
    case 2:
        break;
    }

    /*-------------------------------------*
     *  Your start code
     *-------------------------------------*/

    /*-------------------------------------*
     *  Loop
     *-------------------------------------*/
    switch(flags) {
    case 0:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            yuneta_entry_point(
                argc, argv,
                APP_NAME, APP_VERSION, APP_SUPPORT, APP_DOC, APP_DATETIME,
                fixed_config,
                variable_config,
                register_test_yuno
            );
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    default:
        {
            clock_gettime (CLOCK_MONOTONIC, &st);
            for (i = 0; i < cnt; i++) {
                yuneta_entry_point(
                    argc, argv,
                    APP_NAME, APP_VERSION, APP_SUPPORT, APP_DOC, APP_DATETIME,
                    fixed_config,
                    variable_config,
                    register_test_yuno
                );
            }
            clock_gettime (CLOCK_MONOTONIC, &et);
        }
        break;
    }


    /*-------------------------------------*
     *  Your end code
     *-------------------------------------*/

    /*-------------------------------------*
     *  Print times
     *-------------------------------------*/
    dt = ts_diff (st, et);

    printf("# test(%12" PRIu64 "): %lu.%lus\n",
          cnt,
          dt.tv_sec, dt.tv_nsec);
}

/***************************************************************************
 *                      Main
 ***************************************************************************/
int main(int argc, char *argv[])
{
    if(argc == 1) {
        test(argc, argv, 0, 10LL);
    } else {
        test(argc, argv, 1, 10000LL);
    }
    return 0;
}

