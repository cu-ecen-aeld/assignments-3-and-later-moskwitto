#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
<<<<<<< HEAD
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
=======
>>>>>>> assignments-base/assignment6

bool do_system(const char *command);

bool do_exec(int count, ...);

bool do_exec_redirect(const char *outputfile, int count, ...);
