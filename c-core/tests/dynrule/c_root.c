/***********************************************************************
 *          C_ROOT.C
 *          Root GClass.
 *
 *          GClass for testing
 *
 *          Copyright (c) 2018 Niyamaka.
 *          All Rights Reserved.
 ***********************************************************************/
#include <string.h>
#include <stdio.h>
#include "c_root.h"

/***************************************************************************
 *              Constants
 ***************************************************************************/

/***************************************************************************
 *              Structures
 ***************************************************************************/

/***************************************************************************
 *              Prototypes
 ***************************************************************************/
PRIVATE int testing(hgobj gobj);


/***************************************************************************
 *          Data: config, public data, private data
 ***************************************************************************/


/*---------------------------------------------*
 *      Attributes - order affect to oid's
 *---------------------------------------------*/
PRIVATE sdata_desc_t tattr_desc[] = {
/*-ATTR-type------------name----------------flag------------------------default---------description---------- */
SDATA (ASN_OCTET_STR,   "appName",          SDF_RD,                     "",             "App name, must match the role"),
SDATA (ASN_OCTET_STR,   "appDesc",          SDF_RD,                     "",             "App Description"),
SDATA (ASN_OCTET_STR,   "appDate",          SDF_RD,                     "",             "App date/time"),
SDATA (ASN_OCTET_STR,   "work_dir",         SDF_RD,                     "",             "Work dir"),
SDATA (ASN_OCTET_STR,   "domain_dir",       SDF_RD,                     "",             "Domain dir"),
SDATA (ASN_OCTET_STR,   "yuno_role",        SDF_RD,                     0,              "Yuno Role"),
SDATA (ASN_OCTET_STR,   "yuno_version",     SDF_RD,                     "",             "Yuno version"),
SDATA (ASN_INTEGER,     "timeout",          SDF_RD,                     2*1000,         "Timeout"),
SDATA (ASN_POINTER,     "user_data",        0,                          0,              "user data"),
SDATA (ASN_POINTER,     "user_data2",       0,                          0,              "more user data"),
SDATA (ASN_POINTER,     "subscriber",       0,                          0,              "subscriber of output-events. Not a child gobj."),
SDATA_END()
};

/*---------------------------------------------*
 *      GClass trace levels
 *---------------------------------------------*/
enum {
    TRACE_USER = 0x0001,
};
PRIVATE const trace_level_t s_user_trace_level[16] = {
{"trace_user",        "Trace user description"},
{0, 0},
};


/*---------------------------------------------*
 *              Private data
 *---------------------------------------------*/
typedef struct _PRIVATE_DATA {
    int32_t timeout;

    hgobj dynrule;
} PRIVATE_DATA;




            /******************************
             *      Framework Methods
             ******************************/




/***************************************************************************
 *      Framework Method create
 ***************************************************************************/
PRIVATE void mt_create(hgobj gobj)
{
    PRIVATE_DATA *priv = gobj_priv_data(gobj);

    /*
     *  SERVICE subscription model
     */
    hgobj subscriber = (hgobj)gobj_read_pointer_attr(gobj, "subscriber");
    if(subscriber) {
        gobj_subscribe_event(gobj, NULL, NULL, subscriber);
    }

    priv->dynrule = gobj_create("dynrule", GCLASS_DYNRULE, 0, gobj);

    /*
     *  Do copy of heavy used parameters, for quick access.
     *  HACK The writable attributes must be repeated in mt_writing method.
     */
    SET_PRIV(timeout,               gobj_read_int32_attr)
}

/***************************************************************************
 *      Framework Method writing
 ***************************************************************************/
PRIVATE void mt_writing(hgobj gobj, const char *path)
{
    PRIVATE_DATA *priv = gobj_priv_data(gobj);

    IF_EQ_SET_PRIV(timeout,             gobj_read_int32_attr)
    END_EQ_SET_PRIV()
}

/***************************************************************************
 *      Framework Method destroy
 ***************************************************************************/
PRIVATE void mt_destroy(hgobj gobj)
{
}

/***************************************************************************
 *      Framework Method start
 ***************************************************************************/
PRIVATE int mt_start(hgobj gobj)
{
    PRIVATE_DATA *priv = gobj_priv_data(gobj);

    gobj_start(priv->dynrule);
    testing(gobj);

    return 0;
}

/***************************************************************************
 *      Framework Method stop
 ***************************************************************************/
PRIVATE int mt_stop(hgobj gobj)
{
    PRIVATE_DATA *priv = gobj_priv_data(gobj);
    gobj_stop(priv->dynrule);
    return 0;
}




            /***************************
             *      Commands
             ***************************/




            /***************************
             *      Local Methods
             ***************************/




/***************************************************************************
 *      Convert a data frame in a jn_edifact message
 ***************************************************************************/
json_t *msg2jnedifact(const char *data)
{
    json_t *jn_edifact = json_object();

    //TODO
    return jn_edifact;
}

/***************************************************************************
 *      testing
 ***************************************************************************/
// WARNING si maxmsgs (200) fuese distinto para cada gds,
// necesitariamos una rule para cada gds,
// el gds lo filtrariamos en 'input'
PRIVATE char RULE[]= "\
[{                                                          \n\
    'name': 'generica',                                     \n\
    'input': [                                              \n\
        {                                                   \n\
            'function': 'match',                            \n\
            'parameters': ['.*', '$this`gds']               \n\
        }                                                   \n\
    ],                                                      \n\
    'condition': [                                          \n\
        {                                                   \n\
            'function': 'less',                             \n\
            'parameters': ['$this`curmsg', 200]             \n\
        }                                                   \n\
    ],                                                      \n\
    'true': [                                               \n\
        {                                                   \n\
            'function': 'assign',                           \n\
            'parameters': ['$this`prov_dst', 'IB']          \n\
        },                                                  \n\
        {                                                   \n\
            'function': 'append',                           \n\
            'parameters': ['$this`prov_fake', ['ITA', 'OWN']] \n\
        }                                                   \n\
    ],                                                      \n\
    'false': [                                              \n\
        {                                                   \n\
            'function': 'assign',                           \n\
            'parameters': ['$this`prov_dst', '']            \n\
        }                                                   \n\
    ]                                                       \n\
}]                                                          \n\
";

/*
    Estadisticas
    ============

    Mensajes        Tiempo      Condiciones
    ----------      ------      -----------------------------
    10.000.000      11 sec      En ACER nuevo, EV_RUN_RULE en 3 lugar, ac_run_rule() vacio
    10.000.000      11 sec      En ACER nuevo, EV_RUN_RULE en 1 lugar, ac_run_rule() vacio

    10.000.000      11 sec      En HP del trabajo, EV_RUN_RULE en 3 lugar, ac_run_rule() vacio
    10.000.000      11 sec      En HP del trabajo, EV_RUN_RULE en 1 lugar, ac_run_rule() vacio

    10.000.000      199 sec     En Stratus

En linux ==> 1 millon tr/seg por core
En VOS   ==> 50.000 tr/seg por core

    Con DynRule
    ===========
    HP curro ==========> time for 1000000 dynrules: 4 seconds, 250000 m/s
    Stratus  ==========> time for 1000000 dynrules: 34 seconds, 29411 m/s



*/


PRIVATE int testing(hgobj gobj)
{
    static int curmsg = 0, maxmsgs = 100;
    int bucle = 1000000;
    PRIVATE_DATA *priv = gobj_priv_data(gobj);

    helper_quote2doublequote(RULE);
    json_t *kw_rule = nonlegalstring2json(RULE, strlen(RULE));
    gobj_send_event(priv->dynrule, "EV_ADD_RULES", kw_rule, gobj);

    time_t t1, t2;
    time(&t1);
    for(int i=0;i<bucle; i++) {
        json_t *kw = json_pack("{s:s, s:i, s:i, s:s, s:o}",
            "gds", "1A",
            "curmsg", curmsg,
            "maxmsgs", maxmsgs,
            "prov_dst", "",
            "prov_fake", json_array()
        );
        json_t *kw_io = json_pack("{s:O}",
            "$this", kw
        );
        curmsg++;
        if(curmsg > 250) {
            curmsg = 0;
        }
        json_incref(kw_io);
        int ret = gobj_send_event(priv->dynrule, "EV_RUN_RULE", kw_io, gobj);
        if(ret < 0) {
            printf("ret = %d\n", ret);
        }
        if(0) {
            print_json(kw);
            const char *prov_dst = kw_get_str(kw, "prov_dst", "?", 0);
            json_t* jn_list = kw_get_list(kw, "prov_fake", 0, FALSE);
            printf("prov_dst: %s\n", prov_dst);
            printf("prov_fake: ");
            print_json(jn_list);
        }

        JSON_DECREF(kw_io);
        JSON_DECREF(kw);
    }
    time(&t2);
    printf("\n\n==========> time for %d dynrules: %ld seconds, %d m/s\n\n", bucle, t2-t1, ((t2-t1)>0)?(bucle/(int)(t2-t1)):0);

    return 0;
}




            /***************************
             *      Actions
             ***************************/




/***************************************************************************
 *
 ***************************************************************************/
PRIVATE int ac_sample(hgobj gobj, const char *event, json_t *kw, hgobj src)
{

    KW_DECREF(kw);
    return 0;
}

/***************************************************************************
 *
 ***************************************************************************/
PRIVATE int ac_timeout(hgobj gobj, const char *event, json_t *kw, hgobj src)
{

    KW_DECREF(kw);
    return 0;
}


/***************************************************************************
 *                          FSM
 ***************************************************************************/
PRIVATE const EVENT input_events[] = {
    // top input
    {"EV_SAMPLE",       0,  0,  "Description of resource"},
    // bottom input
    {"EV_TIMEOUT",      0,  0,  ""},
    {"EV_STOPPED",      0,  0,  ""},
    // internal
    {NULL, 0, 0, ""}
};
PRIVATE const EVENT output_events[] = {
    {"EV_ON_SAMPLE1",       0,  0,  "Sample1"},
    {"EV_ON_SAMPLE2",       0,  0,  "Sample2"},
    {NULL, 0, 0, ""}
};
PRIVATE const char *state_names[] = {
    "ST_IDLE",
    NULL
};

PRIVATE EV_ACTION ST_IDLE[] = {
    {"EV_SAMPLE",               ac_sample,              0},
    {"EV_TIMEOUT",              ac_timeout,             0},
    {"EV_STOPPED",              0,                      0},
    {0,0,0}
};

PRIVATE EV_ACTION *states[] = {
    ST_IDLE,
    NULL
};

PRIVATE FSM fsm = {
    input_events,
    output_events,
    state_names,
    states,
};

/***************************************************************************
 *              GClass
 ***************************************************************************/
/*---------------------------------------------*
 *              Local methods table
 *---------------------------------------------*/
PRIVATE LMETHOD lmt[] = {
    {0, 0, 0}
};

/*---------------------------------------------*
 *              GClass
 *---------------------------------------------*/
PRIVATE GCLASS _gclass = {
    0,  // base
    GCLASS_ROOT_NAME,
    &fsm,
    {
        mt_create,
        0, //mt_create2,
        mt_destroy,
        mt_start,
        mt_stop,
        0, //mt_play,
        0, //mt_pause,
        mt_writing,
        0, //mt_reading,
        0, //mt_subscription_added,
        0, //mt_subscription_deleted,
        0, //mt_child_added,
        0, //mt_child_removed,
        0, //mt_stats,
        0, //mt_command_parser,
        0, //mt_inject_event,
        0, //mt_create_resource,
        0, //mt_list_resource,
        0, //mt_update_resource,
        0, //mt_delete_resource,
        0, //mt_add_child_resource_link
        0, //mt_delete_child_resource_link
        0, //mt_get_resource
        0, //mt_authorization_parser,
        0, //mt_authenticate,
        0, //mt_list_childs,
        0, //mt_stats_updated,
        0, //mt_disable,
        0, //mt_enable,
        0, //mt_trace_on,
        0, //mt_trace_off,
        0, //mt_gobj_created,
        0, //mt_future33,
        0, //mt_future34,
        0, //mt_publish_event,
        0, //mt_publication_pre_filter,
        0, //mt_publication_filter,
        0, //mt_authz_checker,
        0, //mt_future39,
        0, //mt_create_node,
        0, //mt_update_node,
        0, //mt_delete_node,
        0, //mt_link_nodes,
        0, //mt_future44,
        0, //mt_unlink_nodes,
        0, //mt_future46,
        0, //mt_get_node,
        0, //mt_list_nodes,
        0, //mt_shoot_snap,
        0, //mt_activate_snap,
        0, //mt_list_snaps,
        0, //mt_treedbs,
        0, //mt_treedb_topics,
        0, //mt_topic_desc,
        0, //mt_topic_links,
        0, //mt_topic_hooks,
        0, //mt_node_parents,
        0, //mt_node_childs,
        0, //mt_node_instances,
        0, //mt_future60,
        0, //mt_topic_size,
        0, //mt_future62,
        0, //mt_future63,
        0, //mt_future64
    },
    lmt,
    tattr_desc,
    sizeof(PRIVATE_DATA),
    0,  // acl
    s_user_trace_level,
    0,  // command_table
    0,  // gcflag
};

/***************************************************************************
 *              Public access
 ***************************************************************************/
PUBLIC GCLASS *gclass_root(void)
{
    return &_gclass;
}
