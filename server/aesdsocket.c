#define _POSIX_C_SOURCE 200809L

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>

#define PORT 9000
#define FILEPATH "/var/tmp/aesdsocketdata"
#define BACKLOG 5
#define RECV_CHUNK 1024

static int server_fd = -1;
static volatile sig_atomic_t exit_flag = 0;

static void signal_handler(int sig)
{
    syslog(LOG_INFO, "Caught signal, exiting");
    exit_flag = 1;

    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }

    /* remove file here as spec requires file deletion on shutdown */
    unlink(FILEPATH);
}

static ssize_t send_all(int fd, const void *buf, size_t len)
{
    size_t total = 0;
    const char *p = buf;
    while (total < len) {
        ssize_t s = send(fd, p + total, len - total, 0);
        if (s < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += (size_t)s;
    }
    return (ssize_t)total;
}

static int append_to_file(const char *buf, size_t len)
{
    int fd = open(FILEPATH, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return -1;

    ssize_t written = 0;
    size_t total = 0;
    while (total < len) {
        written = write(fd, buf + total, len - total);
        if (written < 0) {
            if (errno == EINTR) continue;
            close(fd);
            return -1;
        }
        total += (size_t)written;
    }
    close(fd);
    return 0;
}

/* Send full contents of FILEPATH to client_fd. Returns 0 on success */
static int send_file_contents(int client_fd)
{
    int fd = open(FILEPATH, O_RDONLY);
    if (fd < 0) {
        /* If file doesn't exist, it's not necessarily an error; send nothing */
        if (errno == ENOENT) return 0;
        return -1;
    }

    char buf[RECV_CHUNK];
    ssize_t r;
    while ((r = read(fd, buf, sizeof(buf))) > 0) {
        if (send_all(client_fd, buf, (size_t)r) < 0) {
            close(fd);
            return -1;
        }
    }
    close(fd);
    return (r < 0) ? -1 : 0;
}

int main(int argc, char *argv[])
{
    int daemon_mode = 0;
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        daemon_mode = 1;
    } else if (argc > 1) {
        fprintf(stderr, "Usage: %s [-d]\n", argv[0]);
        return EXIT_FAILURE;
    }

    openlog("aesdsocket", LOG_PID | LOG_CONS, LOG_USER);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1 ||
        sigaction(SIGTERM, &sa, NULL) == -1) {
        syslog(LOG_ERR, "Failed to install signal handlers: %s", strerror(errno));
        closelog();
        return EXIT_FAILURE;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        syslog(LOG_ERR, "socket() failed: %s", strerror(errno));
        closelog();
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        syslog(LOG_ERR, "setsockopt() failed: %s", strerror(errno));
        close(server_fd);
        closelog();
        return EXIT_FAILURE;
    }

    /* Bind */
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        syslog(LOG_ERR, "bind() failed: %s", strerror(errno));
        close(server_fd);
        closelog();
        return EXIT_FAILURE;
    }

    if (listen(server_fd, BACKLOG) == -1) {
        syslog(LOG_ERR, "listen() failed: %s", strerror(errno));
        close(server_fd);
        closelog();
        return EXIT_FAILURE;
    }

    if (daemon_mode) {
        pid_t pid = fork();
        if (pid < 0) {
            syslog(LOG_ERR, "fork() failed: %s", strerror(errno));
            close(server_fd);
            closelog();
            return EXIT_FAILURE;
        } else if (pid > 0) {
            /* parent exits */
            close(server_fd);
            closelog();
            return EXIT_SUCCESS;
        }
       
        if (setsid() == -1) {
            syslog(LOG_ERR, "setsid() failed: %s", strerror(errno));
        }
      
    }

    while (!exit_flag) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            syslog(LOG_ERR, "accept() failed: %s", strerror(errno));
            break;
        }

        char client_ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        char recv_buf[RECV_CHUNK];
        char *pending = NULL;
        size_t pending_len = 0;

        ssize_t r;
        int connection_closed = 0;
        while ((r = recv(client_fd, recv_buf, sizeof(recv_buf), 0)) > 0) {
            char *newp = realloc(pending, pending_len + (size_t)r);
            if (!newp) {
                syslog(LOG_ERR, "malloc/realloc failed while receiving");
                free(pending);
                pending = NULL;
                pending_len = 0;
                connection_closed = 1;
                break;
            }
            pending = newp;
            memcpy(pending + pending_len, recv_buf, (size_t)r);
            pending_len += (size_t)r;

            size_t start = 0;
            for (size_t i = 0; i < pending_len; ++i) {
                if (pending[i] == '\n') {
                    size_t packet_len = i - start + 1; 

                    if (append_to_file(pending + start, packet_len) != 0) {
                        syslog(LOG_ERR, "Failed to append to %s: %s", FILEPATH, strerror(errno));
                    }

                    if (send_file_contents(client_fd) != 0) {
                        syslog(LOG_ERR, "Failed to send file contents to client %s", client_ip);
                    }

                    start = i + 1;
                }
            }

            if (start > 0) {
                size_t left = pending_len - start;
                if (left > 0) {
                    memmove(pending, pending + start, left);
                    pending_len = left;
                    /* shrink */
                    char *shrink = realloc(pending, pending_len);
                    if (shrink || pending_len == 0) pending = shrink;
                } else {
                    free(pending);
                    pending = NULL;
                    pending_len = 0;
                }
            }

            
        }

        if (r == 0) {
            /* client closed connection */
            connection_closed = 1;
        } else if (r < 0) {
            if (errno != EINTR) {
                syslog(LOG_ERR, "recv() failed from %s: %s", client_ip, strerror(errno));
            }
        }

        free(pending);
        pending = NULL;
        pending_len = 0;

        /* Close client */
        close(client_fd);
        syslog(LOG_INFO, "Closed connection from %s", client_ip);

        if (exit_flag) break;
    }

    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }

    unlink(FILEPATH);

    closelog();
    return EXIT_SUCCESS;
}
