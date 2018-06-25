#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sysrepo.h>
#include <sysrepo/plugins.h>
#include <sysrepo/values.h>
#include "common.h"
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef PLUGIN

const char *yang_model = "persona";

char *nombre="Juan";
char *apellido="Cavanagh";
volatile bool *duerme=true;


volatile int exit_application = 0;
volatile pthread_t persona_tid;


sr_session_ctx_t *sess;

typedef struct ctx_s {
    const char *yang_model;
    sr_session_ctx_t *sess;
    sr_subscription_ctx_t *sub;
} ctx_t;



static void *
oven_thread(void *arg)
{
    
	duerme=false;
	sleep(30);
    printf("%s\n", "A Juan le esta dando suenito...");
	duerme=true;
	persona_tid=0;
    return NULL;
}



static int
reloj_state_cb(const char *xpath, sr_val_t **values, size_t *values_cnt, void *private_ctx)
{

    sr_val_t *vals;
    int rc;

    /* convenient functions such as this can be found in sysrepo/values.h */
  
    rc = sr_new_values(3, &vals);
    if (SR_ERR_OK != rc) {
        return rc;
    }

    sr_val_set_xpath(&vals[0], "/persona:human-state/nombre");
    vals[0].type = SR_STRING_T;
    vals[0].data.string_val = nombre;

    sr_val_set_xpath(&vals[1], "/persona:human-state/apellido");
    vals[1].type = SR_STRING_T;
    vals[1].data.string_val = apellido;

    sr_val_set_xpath(&vals[2], "/persona:human-state/sleep-boolean");
    vals[2].type = SR_BOOL_T;
    vals[2].data.bool_val = duerme;

    *values = vals;
    *values_cnt = 3;

    return SR_ERR_OK;

}


static void
event_notif_cb(const sr_ev_notif_type_t notif_type, const char *xpath, const sr_val_t *values, const size_t value_cnt,
       time_t timestamp, void *private_ctx)
{
    /* print notification */
    printf("\n\n ========== SUENA CUMBION ======%s\n\n", ctime(&timestamp));
    printf(">>> Notification content:\n\n");
    for (size_t i = 0; i < value_cnt; ++i) {
        sr_print_val(values+i);
    }
    printf("\n");

    int rc;
    if (duerme == true && persona_tid==0) {
        /* the oven should be turned on and is not (create the oven thread) */
        rc = pthread_create((pthread_t *)&persona_tid, NULL, oven_thread, NULL);
    } 
    /**
     * Here you would normally handle the notification.
     */

    /**
     * Do not deallocate the notification content!
     * Values will get freed automatically by sysrepo.
     */
}



int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_ctx)
{


    int rc = SR_ERR_OK;

    /* INF("sr_plugin_init_cb for sysrepo-plugin-dt-network"); */

    ctx_t *ctx = calloc(1, sizeof(*ctx));
    ctx->sub = NULL;
    ctx->sess = session;
    ctx->yang_model = yang_model;
    *private_ctx = ctx;



    /* subscribe as state data provider for the oven state data */
    rc = sr_dp_get_items_subscribe(session, "/persona:human-state", reloj_state_cb, NULL, SR_SUBSCR_DEFAULT, &ctx->sub);
    if (rc != SR_ERR_OK) {
        goto error;
    }

    /* subscribe for the notification */
    rc = sr_event_notif_subscribe(session, "/alarmafulgor:sonido", event_notif_cb, NULL, SR_SUBSCR_CTX_REUSE, &ctx->sub);
    if (SR_ERR_OK != rc) {
        fprintf(stderr, "Error by sr_event_notif_subscribe: %s\n", sr_strerror(rc));
        goto error;
    }

    INF_MSG("Plugin correctamente inicializado.");
    return SR_ERR_OK;

error:
    ERR("Plugin initialization failed: %s", sr_strerror(rc));
    if (NULL != ctx->sub) {
        sr_unsubscribe(ctx->sess, ctx->sub);
        ctx->sub = NULL;
    }
    return rc;
}


void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_ctx)
{
    INF("Plugin cleanup called, private_ctx is %s available.", private_ctx ? "" : "not");
    
    if (!private_ctx)
        return;

    ctx_t *ctx = private_ctx;
   
    if (NULL == ctx) 
    {
        return;
    }
    if (NULL != ctx->sub) 
    {
        sr_unsubscribe(session, ctx->sub);
    }

    free(ctx);

    DBG_MSG("Plugin cleaned-up successfully");
}




static void sigint_handler(__attribute__((unused)) int signum)
{
    INF_MSG("Sigint called, exiting...");
    exit_application = 1;
}



int main()
{
    INF_MSG("Aplicacion inicializada. Intentando conectar con sysrepo");
    sr_conn_ctx_t *connection = NULL;
    sr_session_ctx_t *session = NULL;
    void *private_ctx = NULL;
    int rc = SR_ERR_OK;

    /* connect to sysrepo */
    rc = sr_connect(yang_model, SR_CONN_DEFAULT, &connection);
    CHECK_RET(rc, cleanup, "Error by sr_connect: %s", sr_strerror(rc));

    /* start session */
    rc = sr_session_start(connection, SR_DS_RUNNING, SR_SESS_DEFAULT, &session);
    CHECK_RET(rc, cleanup, "Error by sr_session_start: %s", sr_strerror(rc));  
    
    sess=session;

    rc = sr_plugin_init_cb(sess, &private_ctx);
    CHECK_RET(rc, cleanup, "Error by sr_plugin_init_cb: %s", sr_strerror(rc));

    /* loop until ctrl-c is pressed / SIGINT is received */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!exit_application) {
        sleep(1); /* or do some more useful work... */
    }

cleanup:
    sr_plugin_cleanup_cb(session, private_ctx);
    if (NULL != session) {
        sr_session_stop(session);
    }
    if (NULL != connection) {
        sr_disconnect(connection);
    }
}
#endif