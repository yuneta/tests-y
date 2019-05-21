#include <stdio.h>
#include <string.h>
#include <yuneta.h>
#include "c_root.h"

/***************************************************************************
 *      Constans
 ***************************************************************************/
#define MIN_BLOCK           32                  /* smaller memory block */
#define MAX_BLOCK           256*1024LL          /* largest memory block */
#define SUPERBLOCK          512*1024LL          /* super-block size */
#define MAX_SYSTEM_MEMORY   64*1024LL*1024LL    /* maximum core memory */

/***************************************************************************
 *      Config
 ***************************************************************************/
PRIVATE char fixed_config[]= "\
{                                                       \n\
    'environment': {                                    \n\
    },                                                  \n\
    'yuno': {                                           \n\
        'yuno_role': 'test_dynrule'                     \n\
    }                                                   \n\
}                                                       \n\
";
PRIVATE char variable_config[]= "\
{                                                       \n\
   'yuno': {                                            \n\
   },                                                   \n\
   'global': {                                          \n\
   },                                                   \n\
   'services': [                                        \n\
        {                                               \n\
            'name': 'test',                             \n\
            'gclass': 'Root',                           \n\
            'default_service': true,                    \n\
            'autostart': true,                          \n\
            'autoplay': false,                          \n\
            'kw': {                                     \n\
            },                                          \n\
            'zchilds': [                                \n\
            ]                                           \n\
        }                                               \n\
   ]                                                    \n\
}                                                       \n\
";

/***************************************************************************
 *      Data
 ***************************************************************************/

/***************************************************************************
 *  Your yuno
 ***************************************************************************/
PRIVATE void register_test_yuno(void)
{
    GCLASS *gclass;

    /*
     *  Subclass default yuno
     */
    gclass = gobj_subclass_gclass(gclass_default_yuno(), "Test");
    //gclass->gmt.mt_play = mt_play;
    //gclass->gmt.mt_pause = mt_pause;

    gobj_register_yuno("test", gclass, FALSE);
}

/***************************************************************************
 *                      Entry point
 ***************************************************************************/
int main(int argc, char *argv[])
{
    /*------------------------------------------------*
     *  Init ghelpers
     *------------------------------------------------*/
    init_ghelpers_library("test");
    log_startup(
        "test",             // application name
        "1.0.0",            // applicacion version
        "test_glogger"     // executable program, to can trace stack
    );
    log_add_handler("test_stdout", "stdout", LOG_OPT_UP_WARNING|LOG_HND_OPT_TRACE_STACK, 0);

    static uint32_t mem_list[] = {0};
    // You also need link with ghelpersd library
    gbmem_trace_alloc_free(0, mem_list);

    /*---------------------------------*
     *  Build config
     *---------------------------------*/
    helper_quote2doublequote(fixed_config);
    helper_quote2doublequote(variable_config);
    char *sconfig = json_config(
        0,  // pprint_verbose_config
        0,  // print_final_config
        fixed_config,
        variable_config,
        0,  // config_json_file
        0,  // parameter_config,
        PEF_EXIT
    );
    if(!sconfig) {
        print_error(
            PEF_EXIT,
            "ERROR YUNETA",
            "json_config() failed"
        );
    }

    /*------------------------------------------------*
     *          Re-alloc with gbmem
     *------------------------------------------------*/
    json_t *__jn_config__ = legalstring2json(sconfig, TRUE); //
    if(!__jn_config__) {
        print_error(
            PEF_EXIT,
            "ERROR YUNETA",
            "legalstring2json() failed"
        );
    }
    free(sconfig);  // HACK I know that sconfig is malloc'ed

    /*------------------------------------------------*
     *      Get yuno attributes.
     *------------------------------------------------*/
    json_t *jn_yuno = kw_get_dict(__jn_config__, "yuno", 0, 0);
    if(!jn_yuno) {
        print_error(
            PEF_EXIT,
            "ERROR YUNETA",
            "'yuno' dict NOT FOUND in json config"
        );
    }
    const char *realm_name  = kw_get_str(jn_yuno, "realm_name", "", 0);
    const char *yuno_role  = kw_get_str(jn_yuno, "yuno_role", 0, 0);
    const char *yuno_name  = kw_get_str(jn_yuno, "yuno_name", "", 0);
    const char *yuno_alias  = kw_get_str(jn_yuno, "yuno_alias", "", 0);

    /*------------------------------------------------*
     *  Init ginsfsm and yuneta
     *------------------------------------------------*/
    init_ginsfsm_library();
    json_t *jn_global = kw_get_dict(__jn_config__, "global", 0, 0);
    gobj_start_up(
        jn_global,
        0,
        0,
        0,
        0,
        0,
        0
    );
    yuneta_register_c_core();

    /*---------------------------------*
     *  Register your yuno/service
     *---------------------------------*/
    register_test_yuno();
    gobj_register_gclass(GCLASS_ROOT);

    /*---------------------------------*
     *  Create yuno
     *---------------------------------*/
    json_incref(jn_yuno);
    hgobj __yuno_gobj__ = gobj_yuno_factory(
        realm_name,
        yuno_name,
        yuno_alias,
        jn_yuno
    );
    if(!__yuno_gobj__) {
        log_error(0,
            "gobj",         "%s", __FILE__,
            "function",     "%s", __FUNCTION__,
            "msgset",       "%s", MSGSET_RUNTIME_ERROR,
            "msg",          "%s", "gobj_yuno_factory() FAILED",
            "role",         "%s", yuno_role,
            NULL
        );
        exit(0); // Exit with 0 to avoid that watcher restart yuno.
    }

    /*------------------------------------------------*
     *          Create services
     *------------------------------------------------*/
    json_t *jn_services = kw_get_list(
        __jn_config__,
        "services",
        0,
        0
    );
    if(jn_services) {
        size_t index;
        json_t *jn_service_tree;
        json_array_foreach(jn_services, index, jn_service_tree) {
            if(!json_is_object(jn_service_tree)) {
                log_error(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                    "msg",          "%s", "service config MUST BE an json object",
                    "index",        "%d", index,
                    NULL
                );
                continue;
            }
            const char *service_name = kw_get_str(jn_service_tree, "name", 0, 0);
            if(empty_string(service_name)) {
                log_error(0,
                    "gobj",         "%s", __FILE__,
                    "function",     "%s", __FUNCTION__,
                    "msgset",       "%s", MSGSET_PARAMETER_ERROR,
                    "msg",          "%s", "service without name",
                    "index",        "%d", index,
                    NULL
                );
                continue;
            }
            json_incref(jn_service_tree);
            gobj_service_factory(service_name, jn_service_tree);
        }
    }

    /*---------------------------------*
     *      Start main
     *---------------------------------*/
    gobj_start(__yuno_gobj__);

    /*---------------------------------*
     *      Auto services
     *---------------------------------*/
    gobj_autostart_services();
    gobj_autoplay_services();

    /*---------------------------------*
     *  HACK Play if need it.
     *---------------------------------*/
    //gobj_play(find_default_service());

    /*-----------------------------------*
     *      Run main event loop
     *-----------------------------------*/
    mt_run(__yuno_gobj__);       // Forever loop. Returning is because someone order to stop
    mt_clean(__yuno_gobj__);     // Before destroying check that uv handlers are closed.

    /*---------------------------*
     *      Destroy all
     *---------------------------*/
    gobj_shutdown();
    gobj_end();

    end_ginsfsm_library();
    gbmem_shutdown();
    end_ghelpers_library();

    return 0;
}
