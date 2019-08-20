#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#define MAX_PATHNAME 1024

static int
check_dir(const pid_t pid, const char *dirname, const char *name)
{
	DIR *dirp;
	struct dirent *direntry;
	char dirpath[MAX_PATHNAME];
	char filepath[MAX_PATHNAME];
	char filename[MAX_PATHNAME];
    int res = -1;
	snprintf(dirpath, MAX_PATHNAME, "/proc/%d/%s", pid, dirname);
	if ((dirp = opendir(dirpath)) == NULL)
		return res;

	while ((direntry = readdir(dirp)) != NULL) {
		if (direntry->d_name[0] < '0' || direntry->d_name[0] > '9')
			continue;
        int n;

		snprintf(filepath, MAX_PATHNAME, "/proc/%d/%s/%s",
			 pid, dirname, direntry->d_name);
 		n=readlink(filepath, filename, MAX_PATHNAME-1);
 		filename[n]='\0';
        fprintf(stdout, "%s\n", filename);
        if ( strlen(filename) >= strlen(name) && !strcmp(filename+(strlen(filename)-strlen(name)), name) ) {
            res = pid;
            break;
        }
	}
	closedir(dirp);
	return res;
}
static int
scan_procs(const char *name)
{
	DIR *topproc_dir;
	struct dirent *topproc_dent;
	pid_t pid, my_pid;

	if ((topproc_dir = opendir("/proc")) == NULL) {
		fprintf(stderr, "Cannot open /proc directory: %s\n",
			strerror(errno));
		return -1;
	}
	my_pid = getpid();
	int res = -1;
	while ((topproc_dent = readdir(topproc_dir)) != NULL) {

		if (topproc_dent->d_name[0] < '0' || topproc_dent->d_name[0] > '9')	// Not a process
			continue;
		pid = atoi(topproc_dent->d_name);
		// Dont print myself
		if (pid == my_pid)
			continue;
		//uid = getpiduid(pid);
		fprintf(stdout," %d\n", pid);
		res = check_dir(pid, "fd", name);
		if ( res != -1 )
            break;

	}
	closedir(topproc_dir);
	return res;
}

int main()
{
    fprintf(stdout, "%d\n", scan_procs("1c.rar"));
    return 0;
}
