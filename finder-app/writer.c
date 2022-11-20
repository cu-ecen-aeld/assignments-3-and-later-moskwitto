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
    return 1;}
     else{
         char writerfile[100];
         char writerstr[100];
         strcpy(writerfile,argv[1]);
         strcpy(writerstr,argv[2]);
         printf("%s", writerfile);}
    //opening file to write
    FILE *file_pointer;
    //file_pointer=fopen(argv[1],"w");
    fd=fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd == -1){
    printf("file not found!");}
    file_pointer=fdopen(fd,"w");
   
    char debug_message[100];
    strcpy(debug_message,"writing");
    strcpy(debug_message,argv[2]);
    strcpy(debug_message,"to");
    strcpy(debug_message,argv[1]);
    //void syslog(int priority, const char *format, ...);
    //openlog(system,LOG_DEBUG,LOG_SYSLOG);
    syslog(LOG_DEBUG,"%s",debug_message);
    fprintf(file_pointer,"%s",argv[2]);
    fclose(file_pointer);
    return 0;
    
}
