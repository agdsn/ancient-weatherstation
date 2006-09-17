/*

   config.h      -- Part of Chart-generator for the weatherstation

   Copyright (C) 2006 Jan Losinski

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/




/* Datenstruktur zum Auswerten des Config-Files */
typedef struct config_keyword_t {
	const char *keyword;
	int (* const handler)(const char *line, void *var);
	void *var;
	const char *def;
} config_keyword; 

/* Config lesen */
int read_config(char *, int , const config_keyword *);

/* Config-Zeilen auswerten */
int read_str(const char *, void *);	/* Einen String lesen */
int read_int(const char *, void *);	/* Einen int lesen */
int read_yn(const char *, void *);	/* Ein yes/no in 1/0 umwandeln */
