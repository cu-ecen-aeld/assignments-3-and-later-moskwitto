#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Open connection to syslog
    openlog("writer", LOG_PID | LOG_CONS, LOG_USER);

    if (argc < 3) {
        syslog(LOG_ERR, "Usage: %s filepath writestr", argv[0]);
        closelog();
        return 1;
    }

    char *writefile = argv[1];
    char *writestr = argv[2];

    int fd = open(writefile, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        syslog(LOG_ERR, "open failed for %s: %s", writefile, strerror(errno));
        closelog();
        return 1;
    }
    syslog(LOG_DEBUG, "File '%s' opened successfully in append mode", writefile);

    ssize_t nr = write(fd, writestr, strlen(writestr));
    if (nr == -1) {
        syslog(LOG_ERR, "write failed: %s", strerror(errno));
        close(fd);
        closelog();
        return 1;
    }
    syslog(LOG_DEBUG, "String written successfully: '%s'", writestr);

    if (lseek(fd, 0, SEEK_SET) == -1) {
        syslog(LOG_ERR, "lseek failed: %s", strerror(errno));
        close(fd);
        closelog();
        return 1;
    }

    char buf[64] = {0};
    nr = read(fd, buf, sizeof(buf) - 1);
    if (nr == -1) {
        syslog(LOG_ERR, "read failed: %s", strerror(errno));
        close(fd);
        closelog();
        return 1;
    }

    syslog(LOG_DEBUG, "File reads: %s", buf);

    close(fd);
    closelog();
    return 0;
}
