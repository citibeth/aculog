// http://stackoverflow.com/questions/32282270/c99-error-unknown-type-name-pid-t
// You could use an earlier version number here;
// 700 corresponds to Posix 2008 with XSI extensions
//#define _XOPEN_SOURCE 700

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// These are defined exernally by CMake
// #define ACURITE_INTERNET_BRIDGE_ID "24C86E064A98"
// #define TCPDUMP_EXE "/usr/sbin/tcpdump"
// #define DATA_DIR "/export/home/acurite/data"
// #define ACULOG_PID "/export/home/acurite/aculog.pid"

char *xxargv[] = {TCPDUMP_EXE, "-tttt", "-A", "-n", "-p", "-l", "-i", "eth1", "-s0", "tcp", "dst", "port", "80"};
char *xxenvp[] = {(char *)0};
char latest_ts[32];		// Last date we read from the network
FILE *cur_file = NULL;
char cur_file_date[32];		// Date of file we're writing
int state = 0;    // Parser state

// Works only on Debian/Ubuntu: http://stackoverflow.com/questions/3118582/how-do-i-find-the-current-system-timezone
void get_timezone(char *buf, size_t max)
{
	int tzf = open("/etc/timezone", O_RDONLY);
	size_t count = read(tzf, buf, max-1);
	buf[count+1] = '\0';
	// Take just first line
	char *nl = index(buf, '\n');
	if (nl != NULL) *nl = '\0';
}

void process_line0(char *tok)
{
	char buf[128];

    // See if we need to open a new file
	if ((cur_file == NULL) || (strncmp(cur_file_date, latest_ts, 10) != 0)) {
		if (cur_file != NULL) fclose(cur_file);

		// Construct new filename

		// First, squeeze out delimiters in timestamp
		char sdate[32];
		strcpy(sdate, latest_ts);
		sdate[4] = sdate[7] = sdate[10] = sdate[13] = sdate[16] = sdate[19] = '\0';

		// Open the new file
		char fname[1024];
		snprintf(fname, sizeof(fname)-1, "%s/acurite-%s%s%s.txt", DATA_DIR, &sdate[0], &sdate[5], &sdate[8]);
		fname[sizeof(fname)-1] = '\0';
		// fprintf(stderr, "Opening file: %s\n", fname);
		cur_file = fopen(fname, "a");
		strncpy(cur_file_date, latest_ts, sizeof(cur_file_date));

		// Write the timezone to it
		get_timezone(buf, sizeof(buf));
		fprintf(cur_file, "tz=%s\n", buf);
	}

	// Convert latest_ts to UTC
	// http://stackoverflow.com/questions/1764710/converting-string-containing-localtime-into-utc-in-c
	char *smillis = &latest_ts[19];		// .000000
	struct tm latest_tm;
	strptime(latest_ts, "%Y-%m-%d %H:%M:%S", &latest_tm);
	time_t latest_t = mktime(&latest_tm);
	struct tm gmcal = *gmtime(&latest_t);
    fprintf(cur_file, "\nts=%s&utc=%04d%02d%02d-%02d:%02d:%02d%s&%s",
		latest_ts,
		gmcal.tm_year+1900, gmcal.tm_mon+1, gmcal.tm_mday, gmcal.tm_hour, gmcal.tm_min, gmcal.tm_sec, smillis,
		tok);
}

void process_line(char *line)
{
	size_t len = strlen(line);
    char *tok;

	// Look for date
	if ((len >= 26) && (line[4] == '-') && (line[7] == '-') && (line[10] == ' ') && (line[13] == ':') && (line[16] == ':') && (line[19] == '.')) {
		strncpy(latest_ts, line, 26);
		latest_ts[26] = '\0';
		return;
	}

    tok = strstr(line, "id=" ACURITE_INTERNET_BRIDGE_ID);
    if (tok != NULL) {
        process_line0(tok);
    } else {
        tok = strstr(line, "humidity=");
        if (tok != NULL) {
            fprintf(cur_file, "&%s", tok);
        } else {
            tok = strstr(line, "baromin=");
            if (tok != NULL) {
                fprintf(cur_file, "&%s\n", tok);
                fflush(cur_file);
            }
        }
    }
}


void process_buffer(char *buf, size_t count)
{
	buf[count] = '\0';
	char *pch = strtok(buf, "\n");
	while (pch != NULL) {
		process_line(pch);
		pch = strtok(NULL, "\n");
	}
}

// http://www.microhowto.info/howto/capture_the_output_of_a_child_process_in_c.html
int main(int argc, char **argv)
{
	char buf[128];

	// ------- Check to see if the process is running
	// Read out of PID file
	FILE *f = fopen(ACULOG_PID, "r");
	if (f != NULL) {
		long pid;
		fscanf(f, "%ld", &pid);
		fclose(f);

		// See if that's found in /proc
		snprintf(buf, sizeof(buf)-1, "/proc/%ld", pid);
		buf[sizeof(buf)-1] = '\0';
		if (access(buf, F_OK) != -1) {
			// /proc/<pid> directory exists, so the process is running
			fprintf(stderr, "%s is already running; exiting now!\n", argv[0]);
			exit(0);
		}

	}



	int filedes[2];
	if (pipe(filedes) == -1) {
		perror("pipe");
		exit(1);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	} else if (pid == 0) {
		// Child process
		while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
		close(filedes[1]);
		close(filedes[0]);
		execve(xxargv[0], xxargv, xxenvp);
		perror("execl");
		_exit(1);
	}

	// ----------- Parent process

	// Drop superuser privileges
	setuid(getuid());
	setgid(getgid());

	// Create the data directory if it doesn't already exist.
	mode_t mask = umask(0); umask(mask);	// Get current umask
	int err = mkdir(DATA_DIR, mask);
	if ((err < 0) && (errno != EEXIST)) {
		perror(argv[0]);
		exit(EXIT_FAILURE);
	}

	// Write our PID to a file
	int pidf = open(ACULOG_PID, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	size_t count = snprintf(buf, sizeof(buf)-1, "%d\n", getpid());
	buf[sizeof(buf)-1] = '\0';
	write(pidf, buf, count);
	close(pidf);

	// Close our pipe
	close(filedes[1]);

	// Read what comes out....
	char buffer[4096];
	while (1) {
		ssize_t count = read(filedes[0], buffer, sizeof(buffer)-1);
		if (count == -1) {
			if (errno == EINTR) {
				continue;
			} else {
				perror("read");
				exit(1);
			}
		} else if (count == 0) {
			break;
		} else {
			process_buffer(buffer, count);
		}
	}
	close(filedes[0]);
	wait(0);
}
