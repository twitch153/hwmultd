
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clcommon.h>

#undef ME
#define ME "libscript_cl"

void
clplugin_init()
{
	return ;
}

void
clplugin_fini()
{
	return ;
}



#define MAX_CONF_DIR_LEN 1024
#define MAX_CONF_FILE_LEN 1024
#define CONF_LINE_BUFFER 4096

#ifndef DEFAULT_CONF_DIR
#define DEFAULT_CONF_DIR "/usr/local/etc/hwmultd"
#endif

char *buf;
char script[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];

int
init_cl()
{
	FILE *myfile;
	char conf_file[MAX_CONF_DIR_LEN+MAX_CONF_FILE_LEN];
	char conf_line[CONF_LINE_BUFFER], first[CONF_LINE_BUFFER], second[CONF_LINE_BUFFER];

	if( !(buf = (char *)malloc(MSG_BUFFER*sizeof(char))) )
		return 0;

	memset(buf, 0, MSG_BUFFER*sizeof(char));

	strncpy(script, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(script, "/scripts/");

	strncpy(conf_file, DEFAULT_CONF_DIR, MAX_CONF_DIR_LEN);
	strcat(conf_file, "/");
	strcat(conf_file, ME);
	strcat(conf_file, ".conf");

	if(myfile = fopen(conf_file, "r"))
	{
		while(fgets(conf_line, CONF_LINE_BUFFER, myfile))
		{
			sscanf(conf_line,"%s %s", first, second ) ;
			if( !strcmp(first,"Script") )
				strncat(script, second, MAX_CONF_FILE_LEN);
		}

		fclose(myfile);
	}
	else
		strcat(script, "null.sh");

	return 1;
}

int
reset_cl()
{
	return 1;
}

char *
act_cl(char *msg)
{
	FILE *f = popen(script, "r");
	fread(buf, sizeof(char), 4096, f);
	pclose(f);

	return buf;
}

int
close_cl()
{
	free(buf);
	return 1;
}
