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

 
#include "highscore.h"
#include <ctype.h>
#include <stdio.h>	
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#define MKDIR(p) mkdir(p, 0755)
#endif


HighScore highscore;


/**************************************************************************************************
 * 
 * private constants, variables, structures, declarations, macros
 *
 **************************************************************************************************/

#define HIGHSCORE_FILE          "highscore"		   /* filename of highscore             */
#define HIGHSCORE_MAX_SCORE     9999999                    /* the status line shows 7 digits    */
#define HIGHSCORE_ID            "nInvaders Highscore"      /* header for highscore file         */
#define HIGHSCORE_VERSION       "0.1"                      /* version number for highscore file */




/**************************************************************************************************
 *
 * highscorePath (Asciiverse)
 *
 * the highscore file in the player's data folder: $XDG_DATA_HOME/ninvaders,
 * else ~/.local/share/ninvaders, or %APPDATA%\ninvaders on Windows; with
 * create, the folders are made as needed. NULL when there is no such folder.
 *
 **************************************************************************************************/
static const char* highscorePath (int create)
{
	static char path[1024];
	const char *base;
	char *p;

#ifdef _WIN32
	if ((base = getenv("APPDATA")) != NULL && *base)
		snprintf(path, sizeof(path), "%s/ninvaders", base);
	else
		return NULL;
#else
	if ((base = getenv("XDG_DATA_HOME")) != NULL && *base)
		snprintf(path, sizeof(path), "%s/ninvaders", base);
	else if ((base = getenv("HOME")) != NULL && *base)
		snprintf(path, sizeof(path), "%s/.local/share/ninvaders", base);
	else
		return NULL;
#endif
	if (create) {
		for (p = path + 1; *p != '\0'; p++) {
			if (*p == '/' || *p == '\\') {
				char c = *p;
				*p = '\0';
				MKDIR(path);
				*p = c;
			}
		}
		MKDIR(path);
	}
	strncat(path, "/" HIGHSCORE_FILE, sizeof(path) - strlen(path) - 1);
	return path;
}



/**************************************************************************************************
 *
 * getStandardHighScore
 *
 * returns a HighScore object initialized with standard values
 *
 **************************************************************************************************/
HighScore getStandardHighScore()
{

	HighScore hs;
	HighScoreEntry *hs_b, *hs_n, *hs_e;
	int n;

	hs_b = hs.beginner;
	hs_n = hs.normal;
	hs_e = hs.expert;
	
        for (n = MAX_HIGHSCORE_ENTRIES; n > 0; n--) {
		
		hs_b->score = n * 1000;
		strcpy ( hs_b->name, "hollinge" );
		hs_b++;
		
		hs_n->score = n * 1000;
		strcpy ( hs_n->name, "sen_hoss" );
		hs_n++;
		
		hs_e->score = n * 1000;
		strcpy ( hs_e->name, "segoh" );
		hs_e++;

	}	
	
	return hs;
	
} 



/**************************************************************************************************
 *
 * fput_HighScoreData
 *
 * writes highscore entries to file
 *
 **************************************************************************************************/
static void fput_HighScoreData (HighScoreEntry* hs_e, FILE* fp)
{
	int n;
	for (n = 0; n < MAX_HIGHSCORE_ENTRIES; n++) {
		fprintf ( fp, "%i %s\n", hs_e -> score, hs_e -> name );
		hs_e++;
	}
	fputs ("\n", fp);
}


	
/**************************************************************************************************
 *
 * writeHighScore
 *
 * writes highscore to file 
 * return value
 *     1 if successful
 *     0 if error occurred
 *
 **************************************************************************************************/
int writeHighScore (HighScore hs)
{

	FILE* fp_HighScore;
	const char* path = highscorePath(1);
	
	if ( path != NULL && ( fp_HighScore = fopen (path, "w") ) != NULL ) {
		
		/* write header */
		fprintf ( fp_HighScore, "%s\nv%s\n\n", HIGHSCORE_ID, HIGHSCORE_VERSION ); 
		
                /* write data */
		fputs ("beginner\n", fp_HighScore);
		fput_HighScoreData (hs.beginner, fp_HighScore);
		fputs ("normal\n", fp_HighScore);
		fput_HighScoreData (hs.normal,   fp_HighScore);
		fputs ("expert\n", fp_HighScore);
		fput_HighScoreData (hs.expert,   fp_HighScore);
	
		/* close file */
		fclose (fp_HighScore);
		
	} else {
		
		return 0;	/* no message: the screen belongs to curses */
		
	}

	return 1;

}



/**************************************************************************************************
 *
 * fget_HighScoreData
 *
 * reads highscore data from file; returns 0 when the data is not as
 * writeHighScore leaves it (Asciiverse: names are read with a length limit
 * and every entry is checked, so a damaged file cannot overrun the table)
 *
 **************************************************************************************************/
static int fget_HighScoreData (HighScoreEntry *hs_e, FILE* fp)
{
	char line[64];
	char name[SIZE_HIGHSCORE_NAME + 2];
	int  n, score;
	char *c;

	for (n = 0; n < MAX_HIGHSCORE_ENTRIES; n++) {
		if (fgets (line, sizeof(line), fp) == NULL
		    || sscanf (line, "%d %9s", &score, name) != 2
		    || score < 0 || score > HIGHSCORE_MAX_SCORE
		    || strlen (name) > SIZE_HIGHSCORE_NAME) {
			return 0;
		}
		for (c = name; *c != '\0'; c++) {
			if (!isgraph ((unsigned char) *c)) {
				return 0;
			}
		}
		if (n > 0 && score > hs_e[-1].score) {
			return 0;	/* a table is sorted, highest first */
		}
		hs_e->score = score;
		strcpy (hs_e->name, name);
		hs_e++;
	}
	return fgets (line, sizeof(line), fp) != NULL && line[0] == '\n';
}



/**************************************************************************************************
 *
 * fget_line (Asciiverse)
 *
 * reads a line and compares it, without its newline, with expected
 *
 **************************************************************************************************/
static int fget_line (FILE* fp, const char* expected)
{
	char line[64];

	if (fgets (line, sizeof(line), fp) == NULL) {
		return 0;
	}
	line[strcspn (line, "\n")] = '\0';
	return strcmp (line, expected) == 0;
}



/**************************************************************************************************
 *
 * readHighScore
 *
 * reads highscore from file; if no file exists or the file seems corrupt or
 * of a wrong version, the standard values are used
 *
 **************************************************************************************************/
HighScore readHighScore (){
	
	FILE* fp_HighScore;
	HighScore hs;
	const char* path = highscorePath(0);
	int ok;

	if ( path == NULL || ( fp_HighScore = fopen (path, "r") ) == NULL ) {
		return getStandardHighScore();
	}

	ok = fget_line (fp_HighScore, HIGHSCORE_ID)
	     && fget_line (fp_HighScore, "v" HIGHSCORE_VERSION)
	     && fget_line (fp_HighScore, "")
	     && fget_line (fp_HighScore, "beginner")
	     && fget_HighScoreData (hs.beginner, fp_HighScore)
	     && fget_line (fp_HighScore, "normal")
	     && fget_HighScoreData (hs.normal,   fp_HighScore)
	     && fget_line (fp_HighScore, "expert")
	     && fget_HighScoreData (hs.expert,   fp_HighScore);
	fclose (fp_HighScore);

	return ok ? hs : getStandardHighScore();

} 



/**
 * Asciiverse: the table for a type (0 beginner, 1 normal, 2 expert)
 */
HighScoreEntry *highscoreEntries (int hsType)
{
	if (hsType == HIGHSCORE_BEGINNER) {
		return highscore.beginner;
	} else if (hsType == HIGHSCORE_EXPERT) {
		return highscore.expert;
	}
	return highscore.normal;
}


/**
 * Asciiverse: the table a skill level plays for: 0 (nightmare) the expert
 * table, 1 (the default) the normal one, 2 to 9 the beginner one
 */
int highscoreTable (int skill)
{
	if (skill <= 0) {
		return HIGHSCORE_EXPERT;
	} else if (skill == 1) {
		return HIGHSCORE_NORMAL;
	}
	return HIGHSCORE_BEGINNER;
}


/**
 * Asciiverse: whether a score earns a place in a table
 */
int highscoreQualifies (int score, int hsType)
{
	return score > 0 && score > highscoreEntries(hsType)[MAX_HIGHSCORE_ENTRIES - 1].score;
}


/**
 * adds entry to highscore (Asciiverse: kept within the table, which the
 * original overran by one entry, and by more when the score was lowest)
 */
void addEntry(char *name, int score, int hsType)
{
	HighScoreEntry *table = highscoreEntries(hsType);
	int i = 0;
	int j;

	while (i < MAX_HIGHSCORE_ENTRIES && table[i].score >= score) {
		i++;
	}
	if (i == MAX_HIGHSCORE_ENTRIES) {
		return;
	}
	for (j = MAX_HIGHSCORE_ENTRIES - 1; j > i; j--) {
		table[j] = table[j - 1];
	}
	table[i].score = score;
	snprintf(table[i].name, sizeof(table[i].name), "%s", name);
} 


/* void displayHighScore(HighScore hs){ */
	
/* 	int n; */
	
/* 	puts (hs.identifier); */
/* 	printf (" v. "); */
/* 	puts (hs.version); */
	
/* 	puts ("beginner"); */
/* 	for (n=0; n<MAX_NUMBER_HIGHSCORE_ENTRIES; n++) { */
/* 		printf ("  %2.7d ", hs.beginner[n].score); */
/* 		puts (hs.beginner[n].name); */
/* 	} */
/* 	puts ("normal"); */
/* 	for (n=0; n<MAX_NUMBER_HIGHSCORE_ENTRIES; n++) { */
/* 		printf ("  %2.7d ", hs.normal[n].score); */
/* 		puts (hs.normal[n].name); */
/* 	} */
/* 	puts ("expert");	 */
/* 	for (n=0; n<MAX_NUMBER_HIGHSCORE_ENTRIES; n++) { */
/* 		printf ("  %2.7d ", hs.expert[n].score); */
/* 		puts (hs.expert[n].name); */
/* 	} */
	
/* } */


/* int main(int argc, char **argv) */
/* { */
/* 	HighScore hs; */
/* 	HighScore hsRead; */
	
	
/* 	hs = getStandardHighScore(); */
/* 	writeHighScore(hs); */

/* 	hsRead = readHighScore(); */
/* 	displayHighScore (hsRead); */
	
	
/* 	return 0; */
/* } */
