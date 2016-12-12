//std stuff
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifndef PORT
#define PORT 8080
#endif

#ifndef ROOT_DIR
#define ROOT_DIR "./public"
#endif

#ifndef REQ_SIZE
#define REQ_SIZE 500
#endif

#ifndef BUFF_SZ
#define BUFF_SZ 32
#endif

#ifndef EOL
#define EOL "\r\n"
#endif

#ifndef EOL_SZ
#define EOL_SZ 2
#endif

void conn_handler(int fd, struct sockaddr_in * connection);
int return_file_size(int fd);
int recv_line(int fd, unsigned char * msg);
int send_line(int fd, unsigned char * msg);
char * file_type(char *f);
int main(void){
  int sock, new_sock;
  int yes = 1;
  struct sockaddr_in host, client;
  socklen_t sin_size;

  if((sock = socket(AF_INET,SOCK_STREAM,0))==-1){
    printf("Socket creation error!\n");
    return 1;
  }

  if((setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)))==-1){
    printf("Socket options error!\n");
    return 2;
  }

  //to bind the socket information..-- basic step
  host.sin_family = AF_INET;
  host.sin_port = htons(PORT);
  host.sin_addr.s_addr = INADDR_ANY;
  memset(&host.sin_zero,'\0',8);

  //binding the socket
  if((bind(sock,(struct sockaddr *)&host,sizeof(struct sockaddr)))==-1){
    printf("Error binding the socket!\n");
    return 3;
  }

  //listen to the socket
  if (listen(sock,10)==-1){
    printf("Error listening to socket!\n");
    return 4;
  }
  printf("The server is up and running!\n");
  while(1){
    sin_size = sizeof(struct sockaddr_in); //get the size of sockaddr_in
    new_sock = accept(sock,(struct sockaddr *)&client,&sin_size);
    if (new_sock == -1){
      printf("Error accepting connection!\n");
      return 5;
    }
    conn_handler(new_sock,&client);
  }
  exit(0);
  return 6;
}


void conn_handler(int sd, struct sockaddr_in * connection){
  unsigned char request[REQ_SIZE], response[REQ_SIZE];
  unsigned char * ptr;
  unsigned char * new_ptr;
  int length = recv_line(sd,request);
  int error_length;
  int fd;
  int isGet;
  int isCGI;
  printf("Handling request from %s:%d \"%s\"\n",inet_ntoa(connection->sin_addr),
	 ntohs(connection->sin_port), request);
  ptr = strstr(request," HTTP/");
  if(ptr == NULL){
    printf("Not a valid HTTP request!\n");
  }//check if request is valid HTTP
  else{
    *ptr = '\0'; //this will make the ' ' be '\0' to terminate the string for simplicity
    ptr = NULL;
    //if the request GET
    if(strncmp(request, "GET ", 4) == 0){
      ptr = request+4;
      isGet = 1;
      printf("pointer value is:%s\n", ptr);

    }
    //if the request HEAD
    if(strncmp(request, "HEAD ", 5) == 0){
      ptr = request+5;
      isGet = 0;
      printf("pointer value is:%s\n", ptr);
    }
    //NOT an HTTP request
    if(ptr == NULL){
      printf("Unknown request!\n");
    }
    else{
      //read the actual commands
      if(isGet == 1){
        if( strcmp(file_type(ptr), "cgi") == 0 ){//cgi scrpits
          printf("reach cgi step\n");
          isCGI = 1;
          ptr = ptr + 1; // skip the "/", now ptr = xxx.cgi;
          FILE *fp;
        	fp = fdopen(sd,"w");
          fprintf(fp, "HTTP/1.0 200 OK\r\n");
          //fd = open(ptr, O_WRONLY, 0);
          //send_line(sd,"HTTP/1.1 200 OK\r\n");
        	fflush(fp);
        	dup2(sd, 1);
        	dup2(sd, 2);
          close(sd);
        	execl(ptr,ptr,NULL);
        	perror(ptr);
        }
        //TODO: else if dir / img / do ls --
        else if(type_dir(ptr+1)){// if its a dir -- do a ls command
          printf("reach dir step\n");
          FILE	*fp;
        	fp = fdopen(sd,"w");
          ptr = ptr + 1;
          fprintf(fp, "HTTP/1.0 200 OK\r\n");
          fprintf(fp, "Content-type: text/plain\r\n");
        	fprintf(fp,"\r\n");
        	fflush(fp);
        	dup2(sd,1);
        	dup2(sd,2);
        	close(sd);
        	execlp("ls","ls","-l",ptr,NULL);
        	perror(ptr);
        	exit(1);
        }

        else{ //just normal web page
          printf("%s\n", ptr);
    	    strcat(ptr,"index.html");
          printf("%s\n", ptr);
          strcpy(response,ROOT_DIR);
          //get the file string ./dir/xxx.html
          strcat(response,ptr);
          fd = open(response,O_RDONLY,0);

          if(fd == -1){
                  close(fd);
                   printf("Error 404 file not found!\n");
                  //send(int socket, const void *buffer, size_t length, int flags);
                   fd = open("./public/404.html", O_RDONLY, 0);
                   new_ptr = '/404.html';
                   if(fd == -1){
                     printf("open 404 fail\n");
                   }
                  send_line(sd,"HTTP/1.1 200 OK\r\n");
                  send_line(sd,"Server\r\n\r\n");
                  if((error_length = return_file_size(fd))==-1){
                    printf("Couldn't get file\'s size!\n");
                    exit(7);
                  }
                  if((new_ptr = (unsigned char *)malloc(length*sizeof(char)))==NULL){
                    printf("Memory could not be allocated!\n");
                    exit(8);
                  }
                  read(fd,new_ptr,error_length);
                  send(sd,new_ptr,error_length,0);
                  free(new_ptr);
                  close(fd);
                }

          else{// if file exists
          printf("Opening: \'%s\'\n", response);
          printf("%s\n", ptr);
        	printf("200 OK\n");
        	send_line(sd,"HTTP/1.1 200 OK\r\n");
        	send_line(sd,"Server\r\n\r\n");
        	  if((length = return_file_size(fd))==-1){
        	    printf("Couldn't get file\'s size!\n");
        	    exit(7);
        	  }
        	  if((ptr = (unsigned char *)malloc(length*sizeof(char)))==NULL){
        	    printf("Memory could not be allocated!\n");
        	    exit(8);
        	  }
        	  read(fd,ptr,length);
        	  send(sd,ptr,length,0);
        	  free(ptr);
        	close(fd);
        }
        }
      }
    }
  }//end of valid HTTP
  shutdown(sd, 2);
  //fclose(sd);
}




//return the file size
int return_file_size(int fd){
  struct stat file_stats;
  if(fstat(fd,&file_stats)==-1)
    return -1;
  return (int)(file_stats.st_size);
}

int recv_line(int sd, unsigned char * msg){
  unsigned char * ptr;
  int eol_counter = 0;
  ptr = msg;
  //receive messages from a socket
  while(recv(sd,ptr,1,0)==1){
    if(*ptr == EOL[eol_counter]){
      eol_counter++;
      if(eol_counter == EOL_SZ){
	       *(ptr+1-EOL_SZ) = '\0';
	        return strlen(msg);
      }
    }
    else{
      eol_counter = 0;
    }
    ptr++;
  }
  return 0;  //no EOL.. shouldnt happen
}

// function to send msg to the server continuously
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

//type checking for different file
char * file_type(char *f)
/* returns 'extension' of file */
{
	char	*cp;
	if ( (cp = strrchr(f, '.' )) != NULL )
		return cp+1;
	return "";
}
//check for directory
type_dir(char *ptr)
{
	struct stat stats;
	return ( stat(ptr, &stats) != -1 && S_ISDIR(stats.st_mode) );
}
