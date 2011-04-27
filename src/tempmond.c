
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cmdargs.h>
#include <log.h>
#include <pidfile.h>
#include <service.h>
#include <signalhandler.h>

int
main( int argc, char *argv[] )
{
	pid_t pid, sid;		// pid and sid of demonized process
	uid_t uid;		// uid for drop privileges
	gid_t gid;		// gid for drop privileges

	// Fork into background
	if( (pid = fork()) < 0)
	{
		fprintf(stderr, "fork failed\n") ;
		exit(EXIT_FAILURE);
	}

	if(pid > 0)
		exit(EXIT_SUCCESS);

	pid = getpid() ;

	// Set umask before opening any files
	umask(022);

	// Open log
	if( !open_log() )
	{
		fprintf(stderr, "open log failed\n") ;
		exit(EXIT_FAILURE);
	}

	// Close open files inherited from parent
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	write_log(DBUG, "set process id %d", (int)pid) ;

	// Create a new SID
	if( (sid = setsid()) < 0)
	{
		write_log(CRIT, "set session id failed") ;
		clean_exit();
	}
	else
		write_log(DBUG, "set session id %d", (int)sid) ;

        // chdir to root
        if(chdir("/") < 0)
	{
		write_log(CRIT, "chdir / failed");
		clean_exit();
        }
	else
		write_log(DBUG, "chdir /");

	// Parse config file to set global parameters
	parse_cfg_file();

	// Parse command line arguments to set global parameters
	parse_cmd_args( argc, argv );

	// Get my uid and gid
	uid = pwd->pw_uid ;
	gid = pwd->pw_gid ;

	// Create pid directory
	if( !dir_pid(uid, gid) )
	{
		write_log(CRIT, "create pid file dir failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file dir");

	// Drop privileges
	if(getuid() == 0)
	{
		if(setgid(gid))
		{
			write_log(CRIT, "drop to gid %d failed", (int)gid);
			clean_exit();
		}
		else
			write_log(DBUG, "droped to gid %d", (int)gid);

		if(setuid(uid))
		{
			write_log(CRIT, "drop to uid %d failed", (int)uid);
			clean_exit();
		}
		else
			write_log(DBUG, "droped to uid %d", (int)uid);
	}

	// Create pid file
	if( !open_pid(pid) )
	{
		write_log(CRIT, "create pid file failed");
		clean_exit();
	}
	else
		write_log(DBUG, "created pid file");

	// We'll get the service started, but after this,
	// we'll let signals drive when the service is
	// stopped/started again
	if( !start_service() )
	{
		write_log(CRIT, "leader: initial start service failed");
		clean_exit();
	}
	else
		write_log(DBUG, "leader: initial started service");

	// Handle signals
	if( !sighandler_session_leader() )
	{
		write_log(CRIT, "leader: register signals failed");
		clean_exit();
	}
	else
		write_log(DBUG, "leader: registered signals");

	// The really big loop - sleep for 136 years or until woken
	while(1) sleep(-1);
}
