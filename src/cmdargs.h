
#include <utmp.h>
#include <pwd.h>
#include <stdint.h>

#define MAX_IP_LEN 50			// Max chars for an ip --- anticipating ipv6
#define MAX_IF_LEN 50			// Max chars for an interface

/*
 * Network parameters
 *
 * See http://www.iana.org/assignments/multicast-addresses/multicast-addresses.xml
 * for the choice of multicast address in Site Local Scope, RFC RFC2365.
 */
#define DEFAULT_MULTICAST_IP "239.252.67.151"
#define DEFAULT_PORT 8000
#define MIN_PORT 1024
#define MAX_PORT 65535

char multicast_ip[MAX_IP_LEN];		// multicast address
uint16_t port;				// multicast port

#define DEFAULT_INTERFACE_IP "0.0.0.0"
#define DEFAULT_INTERFACE_NAME "dummy0"

char interface_ip[MAX_IP_LEN];
char interface_name[MAX_IF_LEN];


/*
 * Client mode vs server mode
 */
#define CLIENT_MODE 0
#define SERVER_MODE 1
#define DEFAULT_SERVER_MODE 0

int server_mode;


/*
 * username for privilege dropping
 */
#define DEFAULT_USERNAME "hwmultd"
#define FALLBACK_USERNAME "root"

char user_name[UT_NAMESIZE];
struct passwd *pwd;


/*
 * Timing parameter
 */
#define MIN_TIMING 1
#define MAX_TIMING 300
#define DEFAULT_TIMING 1

int timing;


/*
 * Log parameters
 *
 * Log level before log_level is read from command line or config file
 */
#define EARLY_LOG_LEVEL 3
#define DEFAULT_LOG_LEVEL 2

int log_level;


/*
 * Plugins
 *
 */

#define MAX_PLUGIN_LEN 128
#define DEFAULT_HW_PLUGIN "null"
#define DEFAULT_CL_PLUGIN "null"

char hw_plugin_name[MAX_PLUGIN_LEN];
char cl_plugin_name[MAX_PLUGIN_LEN];


/*
 * Directory for configure files
 */
#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif



void parse_cfg_file();
void parse_cmd_args( int, char ** );
