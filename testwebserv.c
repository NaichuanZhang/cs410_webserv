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
#include <curl/curl.h>
#include <curl/multi.h>

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

    char	requests[BUFSIZ], ptr[BUFSIZ];
		//child process
    if ( fork() != 0 )
        return;

    strcpy(ptr, "./");
    if ( sscanf(request, "%s%s", requests, ptr+2) != 2 )
		//pass the request to requests variable
        return;

    if ( strcmp(requests,"GET") != 0 ){
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
		else if( strcmp(file_type(ptr), "py") == 0){// use gcc -o testwebserv testwebserv.c -lcurl to compile
			if (system("python weather.py") < 0) // output temp in C in the console.
				fprintf(fp, "Can't get weather"); 
			CURL *hnd = curl_easy_init();
			curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
			curl_easy_setopt(hnd,CURLOPT_URL,"http://api.openweathermap.org/data/2.5/weather?zip=02215%2Cus&appid=11dbf4b98c1530853a5ff0671245754f");
			struct curl_slist *headers = NULL;
			headers = curl_slist_append(headers, "postman-token: 268f5cd0-8b1c-f77e-51a0-5fed88743047");
			headers = curl_slist_append(headers, "cache-control: no-cache");
			curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
			CURLcode ret = curl_easy_perform(hnd);
		}
    else {
			char	*type_ext = file_type(ptr);
      //default
	    char	*header_type = "text/plain";
	    FILE	*fp_sd, *new_fp;
	    int	c,fdis,length;
      unsigned char * new_ptr;
      if ( strcmp(type_ext,"html") == 0 )
	        header_type = "text/html";
	    fp_sd = fdopen(fd, "w");
	    new_fp = fopen( ptr , "r");
	    if ( fp_sd != NULL && new_fp != NULL ) {
	        fprintf(fp_sd, "HTTP/1.0 200 OK\r\n");
					fprintf(fp_sd, "Content-type: %s\r\n", header_type );
	        fprintf(fp_sd, "\r\n");
	        while( (c = getc(new_fp) ) != EOF )
	            putc(c, fp_sd);
	        fclose(new_fp);
	        fclose(fp_sd);
	    }
	    exit(0);
		}
}



int isExist(char *ptr) {
    struct stat info;
    return( stat(ptr,&info) == -1 );
}


char *file_type(char *ptr) { //this only works for direct IP lookup, wouldn't work for DNS, or named domains
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
