#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#if !defined(_WIN32) && !defined(_LINUX)
#error You must define one of the following: _WIN32, _LINUX
#endif

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#elif defined (_LINUX)
#include <unistd.h>
#endif

#include "../config/config.h"
#include <dolib.h>


void usage(void)
{
	printf("usage: dopasswd [options] username\n");
	printf("options:\n");
	printf("  -a                   add user to admin group\n");
	printf("  -d                   delete user\n");
	printf("  -f <filedb>          file\n");
#ifdef _LINUX
    printf("You must be root !!!\n");
#endif
    exit(1);
}
int process_options(int argc, char **argv, char* user_name, char* filename)
{
	int result;
	result = 0;
	char ch;
	while ((ch = getopt(argc, argv, "f:ad")) != EOF) {
		switch(ch) {
		case 'a':
            result =  1;
			break;
		case 'd':
            result =  2;
			break;
        case 'f':
			strcpy(filename, optarg);
            break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	switch(argc) {
	case 0:
        usage();
	case 1:
		strcpy(user_name, argv[0]);
		break;
	default:
		usage();
	}
    return result;
}

#ifdef _WIN32
char* getpass(const char* prompt)
{
     printf(prompt);
     char *ch, *pass;
     pass = (char*)malloc(200);
     ch = pass;
     for(;;){
        *ch = getch();
        if(*ch == '\r' || *ch == '\n') {
            *ch = '\0';
            break;
        }
        ch++;
    }
    printf("\n");
    return pass;
}
#endif

#ifdef _LINUX
#endif
int main(int argc, char **argv)
{
#ifdef _LINUX
	if (getuid() != 0) {
	    usage();
	}
#endif
#ifdef _WIN32
   char *path = strdup(argv[0]);
   char *ch = path + strlen(path) - 1;
   for (; ch > path ; ch -- )
     if ( *ch == '\\' ) {
         *ch = '\0';
        chdir(path);
        break;
     }
   free(path);
#endif

    int mode;
    char username[80];
    char *filename;
    const char *name = "dopasswd";
    char *args[2];
    filename = DOMINO_CONFIG(DOMINO_PASSWD_FILE);
    args[0] = (char*)name;
    args[1] = filename;
    char *password, *password1;
    mode = process_options(argc, argv, username, filename);
    do_set_passwd_filename(filename);
    if (!(mode & 2)) {
        printf("Warning. Low security password !!! \n");
        password = getpass("New SERVICE password:");
        password1 = getpass("Retype new SERVICE password:");
        if (strcmp(password, password1)) {
            printf("Sorry, passwords do not match\n");
            exit(1);
        }
        if (!do_useradd(username, password, mode & 1))
            exit(2);
    }
    else {
        if (!do_userdel(username, password, 0))
            exit(2);
    }
    printf("Success\n");
    return 0;
}
