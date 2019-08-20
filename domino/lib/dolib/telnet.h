#ifndef TELNET_H_INCLUDED
#define TELNET_H_INCLUDED

#define	IAC	255
#define	DONT	254
#define	DO	253
#define	WONT	252
#define	WILL	251
#define	SB	250
#define	GA	249
#define	EL	248
#define	EC	247
#define	AYT	246
#define	AO	245
#define	IP	244
#define	BREAK	243
#define	DM	242
#define	NOP	241
#define	SE	240
#define EOR     239
#define	ABORT	238
#define	SUSP	237
#define	xEOF	236

#define TELOPT_BINARY	0
#define TELOPT_ECHO	1
#define	TELOPT_RCP	2
#define	TELOPT_SGA	3
#define	TELOPT_NAMS	4
#define	TELOPT_STATUS	5
#define	TELOPT_TM	6

#define CH_BACKSPACE 0x08
#define CH_ESCAPE    0x1b
#define CH_F16       0x7F
#define CH_SPACE     0x20

#define CH_CR '\r'
#define CH_LF '\n'

#define COLOR_RED   "\x1b[01;31m"
#define COLOR_BLUE  "\x1b[01;34m"
#define COLOR_GREEN "\x1b[01;32m"

#define FONT_DEFAULT "\x1b[00m"
#define FONT_UNDER   "\x1b[01;4m"
#define FONT_BOLD    "\x1b[01;1m"

#endif // TELNET_H_INCLUDED
