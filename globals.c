/**
 * nInvaders - a space invaders clone for ncurses
 * Copyright (C) 2002-2003 Dettus
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * homepage: http://ninvaders.sourceforge.net
 * mailto: ninvaders-devel@lists.sourceforge.net
 *
 */
 
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "globals.h"


#define MAJOR    0
#define MINOR    1
#define RELEASE  1



#ifdef WIN32
#define usleep(x) Sleep(x/1000)
#endif


/**
 * sleep for specified time
 */
void doSleep(int microseconds)
{
        usleep(microseconds);
}


/**
 * show version information
 */
void showVersion()
{
	fprintf(stderr, "*** nInvaders %i.%i.%i\n", MAJOR, MINOR, RELEASE);
	fprintf(stderr, "*** (C)opyleft 2k2 by Dettus\n");
	fprintf(stderr, "*** dettus@matrixx-bielefeld.de\n");
	fprintf(stderr, "Additional code by Mike Saarna,\n");
	fprintf(stderr, "Sebastian Gutsfeld -> segoh@gmx.net,\n");
	fprintf(stderr, "Alexander Hollinger -> alexander.hollinger@gmx.net and\n");
	fprintf(stderr, "Matthias Thar -> hiast2@compuserve.de\n");
}


/**
 * show usage of command line parameters
 */
void showUsage()
{

	fprintf(stderr, "\n\nUsage: nInvaders [-l skill] [-gpl]\n");
	fprintf(stderr, "   where -l 0=NIGHTMARE\n");
	fprintf(stderr, "         -l 1=okay\n");
	fprintf(stderr, "         -l 9=May I play daddy?!\n");
	fprintf(stderr, "\n         -gpl shows you the license file\n");
}


/**
 * wait for input of return to continue
 */
void waitForReturn()
{
	char b[2];
	fprintf(stderr, "...Please press <Enter> to read on...");
	fgets(b, sizeof(b), stdin);
}

/**
 * show short version of Gnu GPL
 */
void showGplShort()
{
	fprintf(stderr,"\n");
	fprintf(stderr,"This program is free software; you can redistribute it and/or modify\n");
	fprintf(stderr,"it under the terms of the GNU General Public License as published by\n");
	fprintf(stderr,"the Free Software Foundation; either version 2 of the License, or\n");
	fprintf(stderr,"(at your option) any later version.\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"This program is distributed in the hope that it will be useful,\n");
	fprintf(stderr,"but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	fprintf(stderr,"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	fprintf(stderr,"GNU General Public License for more details.\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"You should have received a copy of the GNU General Public License\n");
	fprintf(stderr,"along with this program; if not, write to the Free Software\n");
	fprintf(stderr,"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n");
	fprintf(stderr,"\n");
	fprintf(stderr,"Use the -gpl  command line switch to see the full license of this program\n");
	fprintf(stderr,"Use the -help command line switch to see who wrote this program \n");
	fprintf(stderr,"\n");
}

/**
 * show GNU GENERAL PUBLIC LICENSE
 */
void showGpl()
{
	/* Fix fuxored GPL display */

	FILE	*GPL;

	// Asciiverse: the gpl.txt shipped beside the game first (av starts a
	// title in its install folder), then Debian's copy
	char    *gpl_file = "gpl.txt";
	char    *buff = NULL;
	char    *check = NULL;

	int     lines = 23;
	int     buff_size = 4096;
	int     count = 0;

	if (!(GPL = fopen(gpl_file, "r"))) {
		gpl_file = "/usr/share/common-licenses/GPL";
		if (!(GPL = fopen(gpl_file, "r"))) {
			perror("gpl():\t");
			return;
		}
	}

	if (!(buff = calloc(buff_size, sizeof(char)))) {
		perror("gpl():\t");
		fclose(GPL);
		return;
	}

	do {
		while (count < lines && (check = fgets(buff, buff_size, GPL))) {
			fputs(buff, stderr);
			count++;
		}

		/* Pause */
		waitForReturn();
		count = 0;
	} while (check);

	fclose(GPL);
	free(buff);
}
