#include "user_input.h"

const char *program_version = "1.0 LTS(BETA ALPHA OMEGA)";
const char *program_bug_address = "<edvinas.bureika@teltonika.lt>";

void usr_print_usage(const char *prog_name)
{
    fprintf(stderr,
        "Usage: %s [OPTION...]\n"
        "daemon program that sends system info to Tuya IoT cloud\n\n"
        "  -d, --dev-id=DEV_ID        Device ID\n"
        "  -s, --dev-secret=DEV_SECRET  Device secret\n"
        "  -p, --product-id=PRODUCT_ID  Product ID\n"
        "  -D, --daemon               Run as daemon in background\n"
        "  -i, --interval=INTERVAL    Set interval in seconds data will be sent to server in\n"
        "  -h, --help                 Give this help list\n"
        "  -V, --version               Print program version\n\n"
        "Report bugs to %s.\n",
        prog_name, program_bug_address);
}

int usr_parse_arguments(int argc, char *argv[], struct Arguments *arguments)
{
    static struct option long_options[] = {
        {"dev-id",      required_argument, 0, 'd'},
        {"dev-secret",  required_argument, 0, 's'},
        {"product-id",  required_argument, 0, 'p'},
        {"daemon",      no_argument,       0, 'D'},
        {"interval",    required_argument, 0, 'i'},
        {"help",        no_argument,       0, 'h'},
        {"version",     no_argument,       0, 'V'},
        {0, 0, 0, 0}
    };

    int c;
    int option_index = 0;
    arguments->got_user_input = 0;

    while ((c = getopt_long(argc, argv, "d:s:p:Di:hV", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                strncpy(arguments->dev_id, optarg, sizeof(arguments->dev_id) - 1);
                arguments->dev_id[sizeof(arguments->dev_id) - 1] = '\0';
                break;

            case 's':
                strncpy(arguments->dev_secret, optarg, sizeof(arguments->dev_secret) - 1);
                arguments->dev_secret[sizeof(arguments->dev_secret) - 1] = '\0';
                break;

            case 'p':
                strncpy(arguments->product_id, optarg, sizeof(arguments->product_id) - 1);
                arguments->product_id[sizeof(arguments->product_id) - 1] = '\0';
                break;

            case 'D':
                arguments->daemon_mode = 1;
                break;

            case 'i':
                if (atoi(optarg) <= 0) {
                    fprintf(stderr, "%s: Interval cannot be less than or equal to 0\n", argv[0]);
                    return APP_FAILURE;
                }
                arguments->interval = atoi(optarg);
                break;

            case 'h':
                usr_print_usage(argv[0]);
                exit(0);

            case 'V':
                fprintf(stderr, "%s\n", program_version);
                exit(0);
            
            case '?':
                return APP_FAILURE;

            default:
                return APP_FAILURE;
        }
    }

    if (arguments->dev_id[0] != '\0' ||
        arguments->dev_secret[0] != '\0' ||
        arguments->product_id[0] != '\0') {
        arguments->got_user_input = 1;
    }

    return APP_SUCCESS;
}