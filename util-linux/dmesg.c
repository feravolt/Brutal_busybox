#include "internal.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/* dmesg.c -- Print out the contents of the kernel ring buffer
 * Created: Sat Oct  9 16:19:47 1993
 * Revised: Thu Oct 28 21:52:17 1993 by faith@cs.unc.edu
 * Copyright 1993 Theodore Ts'o (tytso@athena.mit.edu)
 * This program comes with ABSOLUTELY NO WARRANTY.
 * Modifications by Rick Sladkey (jrs@world.std.com)
 * from util-linux; adapted for busybox
 */

#include <linux/unistd.h>
#include <stdio.h>

#define __NR_klog __NR_syslog

#if defined(__GLIBC__)
#include <sys/klog.h>
#define klog klogctl
#else
static inline _syscall3 (int, klog, int, type, char *, b, int, len)
#endif				/* __GLIBC__ */



static const char dmesg_usage[] = "dmesg [-c] [-n level]\n";

int dmesg_main (int argc, char **argv)
{

    char buf[4096];
    int i;
    int n;
    int level = 0;
    int lastc;
    int cmd = 3;

    argc--;
    argv++;

    /* Parse any options */
    while (argc && **argv == '-') {
	while (*++(*argv))
	    switch (**argv) {
	    case 'c':
		cmd = 4;
		break;
	    case 'n':
		cmd = 8;
		if (--argc == 0)
		    goto end;
		level = atoi (*(++argv));
		--argc;
		++argv;
		break;
	    default:
		goto end;
	    }
    }

    if (cmd == 8) {
	n = klog (cmd, NULL, level);
	if (n < 0) {
	    perror ("klog");
	    exit (FALSE);
	}
	exit (TRUE);
    }

    n = klog (cmd, buf, sizeof (buf));
    if (n < 0) {
	perror ("klog");
	exit (FALSE);
    }

    lastc = '\n';
    for (i = 0; i < n; i++) {
	if ((i == 0 || buf[i - 1] == '\n') && buf[i] == '<') {
	    i++;
	    while (buf[i] >= '0' && buf[i] <= '9')
		i++;
	    if (buf[i] == '>')
		i++;
	}
	lastc = buf[i];
	putchar (lastc);
    }
    if (lastc != '\n')
	putchar ('\n');
    exit (TRUE);

  end:
    usage( dmesg_usage);
    exit (FALSE);
}
