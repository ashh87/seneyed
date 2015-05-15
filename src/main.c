#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#include "seneye_hotplug.h"

static sigset_t   signal_mask;  /* signals to block         */

static void die (const char * format, ...)
{
    va_list vargs;
    va_start (vargs, format);
    syslog(LOG_ERR, format, vargs);
    exit (1);
}

static void usage(const char* argv0)
{
    printf("usage: %s [options]\n", argv0);
    printf("\n");
    printf(" -f run in foreground (not as a daemon)\n");
    printf("\n");
    exit(0);
}

void be_daemon() {
    // Fork, allowing the parent process to terminate.
    pid_t pid = fork();
    if (pid == -1) {
        die("failed to fork while daemonising (errno=%d)",errno);
    } else if (pid != 0) {
        _exit(0);
    }

    // Start a new session for the daemon.
    if (setsid()==-1) {
        die("failed to become a session leader while daemonising(errno=%d)",errno);
    }

    // Fork again, allowing the parent process to terminate.
    signal(SIGHUP,SIG_IGN);
    pid=fork();
    if (pid == -1) {
        die("failed to fork while daemonising (errno=%d)",errno);
    } else if (pid != 0) {
        _exit(0);
    }

    // Set the current working directory to the root directory.
    if (chdir("/") == -1) {
        die("failed to change working directory while daemonising (errno=%d)",errno);
    }

    // Set the user file creation mask to zero.
    umask(0);

    // Close then reopen standard file descriptors.
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if (open("/dev/null",O_RDONLY) == -1) {
        die("failed to reopen stdin while daemonising (errno=%d)",errno);
    }
    if (open("/dev/null",O_WRONLY) == -1) {
        die("failed to reopen stdout while daemonising (errno=%d)",errno);
    }
    if (open("/dev/null",O_RDWR) == -1) {
        die("failed to reopen stderr while daemonising (errno=%d)",errno);
    }
}

void *signal_thread (void *arg)
{
    int sig_caught;    /* signal caught */
    int exiting = 0;
    /* Use same mask as the set of signals that we'd like to know about! */
    do {
        sigwait(&signal_mask, &sig_caught);
        switch(sig_caught) {
        case SIGHUP:
            syslog(LOG_INFO, "hangup signal caught");
            break;
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO,"interrupt signal caught");
            exiting = 1;
            break;
        }
    } while (exiting == 0);
    return 0;
}

int main(int argc, char* argv[])
{
    int input_option;
    int is_daemon = 1;
    int log_opts = LOG_ODELAY;
    int log_facility = LOG_USER;
    int rc;
    pthread_t sig_thr_id;      /* signal handler thread ID */

    while((input_option = getopt(argc, argv, "f")) != -1) {
        switch(input_option) {
        case 'f':
            is_daemon = 0;
            break;
        default:
            usage(argv[0]);
            break;
        }
    }

    if (is_daemon == 0)
        log_opts |= LOG_PERROR; //not a daemon, print to stderr

    //begin logging
    openlog ("seneyed", log_opts, log_facility);

    if (is_daemon == 1)
        be_daemon();
    
    syslog (LOG_INFO, "seneye daemon started");
    
    sigemptyset (&signal_mask);
    sigaddset (&signal_mask, SIGINT);
    sigaddset (&signal_mask, SIGTERM);
    sigaddset (&signal_mask, SIGHUP);
    pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);

    /* New threads will inherit this thread's mask */
    pthread_create (&sig_thr_id, NULL, signal_thread, NULL);
    
    //register usb callback
    rc = start_hotplug();
    
    
    if (rc == 0)
        pthread_join(sig_thr_id, NULL);
    else
        pthread_cancel(sig_thr_id);

    syslog(LOG_INFO, "beginning exit...");
    stop_hotplug();
    closelog ();

	return 0;
}
