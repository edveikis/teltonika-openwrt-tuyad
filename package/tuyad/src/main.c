#include "main.h"

int running = 1;

int main(int argc, char *argv[])
{
    openlog("tuyad", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    
    syslog(LOG_INFO, "Setting up signals");
    if (setup_signals() != APP_SUCCESS) {
        syslog(LOG_ERR, "Failed setting up signals");
        return APP_FAILURE;
    }

    struct Arguments arguments = {0};
    memset(&arguments, 0, sizeof(arguments));

    syslog(LOG_INFO, "Parsing CLI arguments");
    if (usr_parse_arguments(argc, argv, &arguments) != APP_SUCCESS) {
        syslog(LOG_ERR, "Failed to parse command line argumments");
        return APP_FAILURE;
    }

    if (arguments.daemon_mode) {
        int nochdir = 0;
        int noclose = 0;
        syslog(LOG_INFO, "Creating a deamon");
        if(daemon(nochdir, noclose)) {
            syslog(LOG_ERR, "Failed to deamonise");
            return APP_FAILURE;
        }
    }

    char product_id[17], dev_id[23], dev_secret[17];
    if (arguments.got_user_input == 0) {
        syslog(LOG_INFO, "Loading UCI config");
        if (config_load_uci() != APP_SUCCESS) {
            syslog(LOG_ERR, "Failed to load UCI config");
            closelog();
            return APP_FAILURE;
        }
        memcpy(product_id, g_cfg_product_id, sizeof(product_id));
        memcpy(dev_id, g_cfg_device_id, sizeof(dev_id));
        memcpy(dev_secret, g_cfg_device_secret, sizeof(dev_secret));
    } 
    else {
        memcpy(product_id, arguments.product_id, sizeof(product_id));
        memcpy(dev_id, arguments.dev_id, sizeof(dev_id));
        memcpy(dev_secret, arguments.dev_secret, sizeof(dev_secret));
    }

    if (product_id[0] == '\0' || dev_id[0] == '\0' || dev_secret[0] == '\0') {
        syslog(LOG_ERR, "Missing input data");
        closelog();
        return APP_FAILURE;
    }

    struct ubus_context *ctx;

    ctx = ubus_connect(NULL);
    if (!ctx) {
        syslog(LOG_ERR, "Failed to connect to ubus");
        closelog();
        return APP_FAILURE;
    }

    syslog(LOG_INFO, "Initializing Tuya");
    tuya_mqtt_context_t *client = &client_instance;
    int ret = OPRT_OK;
    ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = dev_id,
        .device_secret = dev_secret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = tuya_on_connected,
        .on_disconnect = tuya_on_disconnect,
        .on_messages = tuya_on_messages
    });

    if (ret != OPRT_OK) {
        tuya_mqtt_deinit(client);
        syslog(LOG_ERR, "Failed to init Tuya");
        closelog();
        ubus_free(ctx);
        return MQTT_INIT_FAIL;
    }

    ret = tuya_mqtt_connect(client);
    if (ret != OPRT_OK) {
        tuya_mqtt_deinit(client);
        syslog(LOG_ERR, "Failed to connect to Tuya");
        closelog();
        ubus_free(ctx);
        return MQTT_CONN_FAIL;
    }

    int report_interval = S_OUTPUT_INTERVAL;
    if (arguments.got_user_input != 0 && arguments.interval > 0) {
        report_interval = arguments.interval;
    }
    else if (g_cfg_interval > 0) {
        report_interval = g_cfg_interval;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    syslog(LOG_INFO, "Starting application's mainloop");
    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &stop);
        double result = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / 1e9;
        if (result >= report_interval || g_cfg_send_now == 1) {
            char *sys_data_json = get_data_json(ctx);
            if (sys_data_json == NULL) {
                syslog(LOG_ERR, "Failed to pack data to JSON format");
                clock_gettime(CLOCK_MONOTONIC, &start);
                continue;
            }

            syslog(LOG_INFO, "Sending data to the cloud");
            tuyalink_thing_property_report(client, NULL, sys_data_json);
            
            tuya_mqtt_loop(client);

            free(sys_data_json);
            clock_gettime(CLOCK_MONOTONIC, &start);
            if (g_cfg_send_now == 1) {
                if (config_set_send_now_false() != APP_SUCCESS) {
                    syslog(LOG_ERR, "Failed to reset send_now back to 0");
                    running = 0;
                }
                else {
                    g_cfg_send_now = 0;
                }
            } 
        }
    }

    syslog(LOG_INFO, "Main loop terminated, cleaning up");
    tuya_mqtt_disconnect(client);
    tuya_mqtt_deinit(client);
    ubus_free(ctx);
    uloop_done();
    syslog(LOG_INFO, "Application successfully finished executing");
    closelog();
    return APP_SUCCESS;
}

int setup_signals(void) 
{
    if (signal(SIGINT, sig_handler) == SIG_ERR) {
        return APP_FAILURE;
    }
    if (signal(SIGTERM, sig_handler) == SIG_ERR) {
        return APP_FAILURE;
    }
    if (signal(SIGHUP, sig_handler) == SIG_ERR) {
        return APP_FAILURE;
    }
    if (signal(SIGQUIT, sig_handler) == SIG_ERR) {
        return APP_FAILURE;
    }

    return APP_SUCCESS;
}

void sig_handler(int signum) 
{
    running = 0;
}