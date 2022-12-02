#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
int main(int argc,char *argv[]){
    int counter=0;
    int fd;
    openlog("writer-log", LOG_PID, LOG_USER);
    if(argc != 3) {printf("Not enough arguments specified");
    syslog(LOG_ERR, "Incorrect number of arguments. Received %d arguments instead of 2", argc-1);
    exit (1);}
    const char *writerfile=argv[1];
    const char *writerstr=argv[2];
    //*or alternatively*//
    //strcpy(writerfile,argv[1]);
    //strcpy(writerstr,argv[2]);
    printf("%s", writerfile);
    //opening file to write
    FILE *file_pointer;
    //file_pointer=fopen(argv[1],"w");
    fd= open(writerfile, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd == -1){
    printf("file not found!");
    exit(1);}
   
    char debug_message[100];
    strcpy(debug_message,"writing");
    strcpy(debug_message,writerstr);
    strcpy(debug_message,"to");
    strcpy(debug_message,writerfile);
    //void syslog(int priority, const char *format, ...);
    //openlog(system,LOG_DEBUG,LOG_SYSLOG);
    size_t count;
    count=strlen(argv[2]);
    syslog(LOG_DEBUG,"%s",debug_message);
    write(fd,writerstr,count);
    close(fd);
    return 0;
    
}
