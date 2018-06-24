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

const char *yang_model = "alarmafulgor";
const char *entrada_state="";
const char *salida_state="";

volatile int exit_application = 0;
volatile pthread_t alarma_tid;
volatile unsigned int hora;
volatile unsigned int minuto;
volatile unsigned int segundo;

volatile unsigned int hora_alarma;
volatile unsigned int minuto_alarma;
volatile unsigned int segundo_alarma;
volatile char *ringtone;

sr_session_ctx_t *sess;

typedef struct ctx_s {
    const char *yang_model;
    sr_session_ctx_t *sess;
    sr_subscription_ctx_t *sub;
} ctx_t;



static void *
oven_thread(void *arg)
{
    int rc;
    rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    char *temp = NULL;
    char buf[100] = {0};
    char *response = NULL;
    unsigned int size = 1;
    unsigned int strlength;
    
    strcpy(buf, "tarado e");
    strlength = strlen(buf);
    temp = realloc(response, size + strlength);
    response = temp;
    strcpy(response + size - 1, buf);
    size += strlength;
    sr_val_t values[1];
    values[0].xpath = "/alarmafulgor:sonido/source/ringtone";
    values[0].type = SR_STRING_T;
    values[0].data.string_val = response;

    while (alarma_tid) {
        time_t rawtime;
        struct tm *info;
        char buffer[80];
        time( &rawtime );
        info = localtime( &rawtime );
        strftime(buffer,80,"%x - %I:%M%p", info);
        hora=info->tm_hour;
        minuto=info->tm_min;
        segundo=info->tm_sec;
        if(hora==hora_alarma && minuto==minuto_alarma && segundo==segundo_alarma)
        {   
            printf("%s\n", "ENTRO");
            rc = sr_event_notif_send(sess, "/alarmafulgor:sonido", values, 1, SR_EV_NOTIF_DEFAULT);
                    if (rc != SR_ERR_OK) {
                        SRP_LOG_ERR("OVEN: Oven-ready notification generation failed: %s.", sr_strerror(rc));
            
            }
            sleep(0.9);
            alarma_tid=0;
            return NULL; 
        }
        sleep(0.9);
    }
    return NULL;
}



static int rpc_cb(__attribute__((unused)) const char *xpath,
                  const sr_val_t *input,
                  __attribute__((unused)) const size_t input_cnt,
                  sr_val_t **output,
                  size_t *output_cnt,
                  __attribute__((unused)) void *private_ctx)
{
    int rc = SR_ERR_OK;
    char *temp = NULL;
    char buf[100] = {0};
    char *response = NULL;
    unsigned int size = 1;
    unsigned int strlength;
    void *res;
    CHECK_NULL_MSG(input, &rc, error, "input is empty");

    
    if (0 == strcmp("activar", input[0].data.string_val))
    {
        if (alarma_tid == 0) {
        /* the oven should be turned on and is not (create the oven thread) */
        rc = pthread_create((pthread_t *)&alarma_tid, NULL, oven_thread, NULL);
        if (rc != 0) {
            goto error;
            }
        } 
        strcpy(buf, "ACTIVO");
    }

    else{
        if (alarma_tid != 0) {
        /* the oven should be turned off but is on (stop the oven thread) */
        rc = pthread_cancel(alarma_tid);
        if (rc != 0) {
            goto error;
        }
        rc = pthread_join(alarma_tid, &res);
       if (rc != 0) {
            goto error;
        }

       if (res == PTHREAD_CANCELED){
        printf("main(): thread was canceled\n");
        alarma_tid=0;
    }
        strcpy(buf, "DESACTIVO");
    }
}

    
    
    strlength = strlen(buf);
    temp = realloc(response, size + strlength);
    CHECK_NULL(temp, &rc, error, "failed realloc for command: \"%s\"", input[0].data.string_val);
    response = temp;
    strcpy(response + size - 1, buf);
    size += strlength;
    
    rc = sr_new_values(1, output);
    CHECK_RET(rc, error, "failed sr_new_values %s", sr_strerror(rc));
    *output_cnt = 1;
    sr_val_set_xpath(*output, "/alarmafulgor:alarmaconfig/respuesta");
    sr_val_set_str_data(*output, SR_STRING_T, response);

error:
    if (response) {
        free(response);
    }
    return rc;
}



/* retrieves & prints current alarma configuration */
static void
retrieve_current_config(sr_session_ctx_t *session)
{
    sr_val_t *values = NULL;
    size_t count = 0;
    int rc = SR_ERR_OK;

    printf("Configuracion actual de la alarma:\n");

    rc = sr_get_items(session, "/alarmafulgor:alarma/*", &values, &count);
    if (SR_ERR_OK != rc) {
        SRP_LOG_ERR("Error by sr_get_items: %s", sr_strerror(rc));
        return;
    }
    for (size_t i = 0; i < count; i++){
        sr_print_val(&values[i]);
    }
    sr_free_values(values, count);
}


static int
alarma_config_change_cb(sr_session_ctx_t *session, const char *module_name, sr_notif_event_t event, void *private_ctx)
{
    
    SRP_LOG_DBG_MSG("La configuracion de la alarma cambio.");


    int rc;
    sr_val_t *val;


    /* get the value from sysrepo, we do not care if the value did not change in our case */
    rc = sr_get_item(session, "/alarmafulgor:alarma/horas", &val);
    hora_alarma = val->data.uint8_val;
    sr_free_val(val);

    /* get the value from sysrepo, we do not care if the value did not change in our case */
    rc = sr_get_item(session, "/alarmafulgor:alarma/minutos", &val);
    if (SR_ERR_OK != rc) {
        return rc;
    }
    minuto_alarma = val->data.uint8_val;
    sr_free_val(val);

    /* get the value from sysrepo, we do not care if the value did not change in our case */
    rc = sr_get_item(session, "/alarmafulgor:alarma/segundos", &val);
    if (SR_ERR_OK != rc) {
        return rc;
    }
    segundo_alarma = val->data.uint8_val;
    sr_free_val(val);

    /* get the value from sysrepo, we do not care if the value did not change in our case */
    rc = sr_get_item(session, "/alarmafulgor:alarma/segundos", &val);
    if (SR_ERR_OK != rc) {
        return rc;
    }
    ringtone = val->data.string_val;
    sr_free_val(val);

    retrieve_current_config(session);

    return SR_ERR_OK;
}



static int
reloj_state_cb(const char *xpath, sr_val_t **values, size_t *values_cnt, void *private_ctx)
{

    time_t rawtime;
    struct tm *info;
    char buffer[80];

    time( &rawtime );

    info = localtime( &rawtime );

    strftime(buffer,80,"%x - %I:%M%p", info);


    hora=info->tm_hour;
    minuto=info->tm_min;
    segundo=info->tm_sec;
    
    sr_val_t *vals;
    int rc;

    /* convenient functions such as this can be found in sysrepo/values.h */
  
    rc = sr_new_values(3, &vals);
    if (SR_ERR_OK != rc) {
        return rc;
    }

    sr_val_set_xpath(&vals[0], "/alarmafulgor:reloj-sistema/horasis");
    vals[0].type = SR_UINT8_T;
    vals[0].data.uint8_val = hora;

    sr_val_set_xpath(&vals[1], "/alarmafulgor:reloj-sistema/minutosis");
    vals[1].type = SR_UINT8_T;
    vals[1].data.uint8_val = minuto;

    sr_val_set_xpath(&vals[2], "/alarmafulgor:reloj-sistema/segundosis");
    vals[2].type = SR_UINT8_T;
    vals[2].data.uint8_val = segundo;

    *values = vals;
    *values_cnt = 3;

    return SR_ERR_OK;

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


    /* subscribe for alarmafulgor module changes - also causes startup alarmafulgor data to be copied into running and enabling the module */
    rc = sr_module_change_subscribe(session, "alarmafulgor", alarma_config_change_cb, NULL, 2,
            SR_SUBSCR_APPLY_ONLY, &ctx->sub);
    if (rc != SR_ERR_OK) {
        goto error;
    }

    /* subscribe as state data provider for the oven state data */
    rc = sr_dp_get_items_subscribe(session, "/alarmafulgor:reloj-sistema", reloj_state_cb, NULL, SR_SUBSCR_CTX_REUSE, &ctx->sub);
    if (rc != SR_ERR_OK) {
        goto error;
    }


    /* Subscripcion para la rpc */
    rc = sr_rpc_subscribe(session, "/alarmafulgor:alarmaconfig", rpc_cb, (void *) session, SR_SUBSCR_CTX_REUSE, &ctx->sub);
    if (rc != SR_ERR_OK) {
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