#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef PORT
#define PORT 8080
#endif


int main(int argc, char *argv[]) {// sample socket code
		char * file_type(char *ptr);
	  int return_file_size(int new_sd);
	  int sock_opt_val = 1;
	  int cli_len;
	  char data[800];		/* Our receive data buffer. */

    void recv_line(FILE *fp);
    void servConn( char *request, int new_sd );
	  struct sockaddr_in name, cli_name;
    int 	sd, new_sd;
    FILE	*fd;
    char	request[BUFSIZ];

		if ((sd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("(servConn): socket() error");
	    exit (-1);
	  }
		fprintf(stderr,"Server socket new_sd: %d\n", sd);
	  if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, (char *) &sock_opt_val,
			  sizeof(sock_opt_val)) < 0) {
	    perror ("(servConn): Failed to set SO_REUSEADDR on INET socket");
	    exit (-1);
	  }

		memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    name.sin_port = htons(PORT);

    bind(sd, (struct sockaddr *) &name, sizeof(name));
    listen(sd, 5);
		fprintf(stderr,"Server is up and running!!!\n");
    /* main loop here */
    while(1) {
        /* take a call and buffer it */

        new_sd = accept (sd, (struct sockaddr *) &cli_name, &cli_len);
				if (new_sd == -1){
		      printf("Error accepting connection!\n");
		      return 5;
		    }
        fd = fdopen(new_sd, "r" );
        fgets(request, BUFSIZ, fd); //read the request
        printf("request is = %s", request);
        recv_line(fd);
        servConn(request, new_sd);
        fclose(fd);
    }
}


void recv_line(FILE *fp) {
    char	buf[BUFSIZ]; //BUFSIZ is a build in value for stream
    while( fgets(buf,BUFSIZ,fp) != NULL && strcmp(buf,"\r\n") != 0 );
}




void servConn(char *request, int fd) {
    int isExist(char *ptr);
    int isCGI(char *ptr);

    char	cmd[BUFSIZ], ptr[BUFSIZ];
		//child process
    if ( fork() != 0 )
        return;

    strcpy(ptr, "./");
    if ( sscanf(request, "%s%s", cmd, ptr+2) != 2 )
		//pass the request to cmd variable
        return;

    if ( strcmp(cmd,"GET") != 0 ){
			FILE	*fp = fdopen(fd,"w");
			fprintf(fp, "HTTP/1.0 501 Unknown command\r\n");
			fprintf(fp, "Content-type: text/plain\r\n");
			fprintf(fp, "\r\n");
			fprintf(fp, "Unknown request\r\n");
			fclose(fp);
		}
    else if (isExist(ptr)){
			FILE	*fp = fdopen(fd,"w");
	    fprintf(fp, "HTTP/1.0 404 Not Found\r\n");
	    fprintf(fp, "Content-type: text/plain\r\n");
	    fprintf(fp, "\r\n");
	    fprintf(fp, "Your file %s\r\nis not found\r\n",ptr);
	    fclose(fp);
		}
    else if (type_dir(ptr)){
			FILE	*fp ;
	    fp = fdopen(fd,"w");
	    fprintf(fp, "HTTP/1.0 200 OK\r\n");
			fprintf(fp, "Content-type: %s\r\n", "text/plain");
	    fprintf(fp,"\r\n");
	    fflush(fp);
	    dup2(fd,1);
	    dup2(fd,2);
	    close(fd);
	    execlp("ls","ls","-l",ptr,NULL);
	    perror(ptr);
	    exit(1);
		}
    else if ( strcmp(file_type(ptr), "cgi") == 0  ) {
			FILE	*fp ;
	    fp = fdopen(fd,"w");
			fprintf(fp, "HTTP/1.0 200 OK\r\n");
	    fflush(fp);
	    dup2(fd, 1);
	    dup2(fd, 2);
	    close(fd);
	    execl(ptr,ptr,NULL);
	    perror(ptr);
		}
    else {
			char	*extension = file_type(ptr);
      //default
	    char	*header_type = "text/plain";
	    FILE	*fpsock, *fpfile;
	    int	c;
      int fdis;
      int length;
      unsigned char * new_ptr;
      new_ptr = "./public/index.html";
      if (fdis = open("./public/index.html",O_RDONLY,0) == -1)
          printf("fail to open file\n" );
	    if ( strcmp(extension,"html") == 0 )
	        header_type = "text/html";
      printf("200 OK\n");
      send_line(fd,"HTTP/1.1 200 OK\r\n");
      printf("%s\n", new_ptr);
      send_line(fd, "Content-type: text/html\r\n");
      if((length = return_file_size(fdis))==-1){
        printf("Couldn't get file\'s size!\n");
        exit(7);
      }
      if((new_ptr = (unsigned char *)malloc(length*sizeof(char)))==NULL){
        printf("Memory could not be allocated!\n");
        exit(8);
      }
      read(fdis,new_ptr,length);
      send(fd,new_ptr,length,0);
      free(new_ptr);
      close(fdis);
		}
}



int isExist(char *ptr) {
    struct stat info;
    return( stat(ptr,&info) == -1 );
}


char *file_type(char *ptr) {
    char	*cp;
    if ( (cp = strrchr(ptr, '.' )) != NULL )
        return cp+1;
    return "";
}

int type_dir(char *ptr)
{
	struct stat stats;
	return ( stat(ptr, &stats) != -1 && S_ISDIR(stats.st_mode) );
}

int send_line(int sd, unsigned char  *msg){
  int sent_bytes, bytes_to_send;
  bytes_to_send = strlen(msg);
  while(bytes_to_send>0){
    sent_bytes = send(sd,msg,bytes_to_send,0);
    if(sent_bytes==-1)
      return 0;
    bytes_to_send -= sent_bytes;
    msg += sent_bytes;
  }
}

//return the file size
int return_file_size(int fd){
  struct stat file_stats;
  if(fstat(fd,&file_stats)==-1)
    return -1;
  return (int)(file_stats.st_size);
}