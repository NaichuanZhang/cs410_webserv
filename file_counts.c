#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *); 

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

static char * pathname;
static char * String;
static char * fileType;
static int f_flag =0;
static int l_flag = 0;

static char *fullpath;
static size_t pathlen;

int
main(int argc, char *argv[])
{
  int ret, x;
  char str [15];   

  if (argc != 2)
    printf("usage: ftw <starting-pathname>");

  ret = myftw(argv[1], myfunc); /* does it all */
  
   char *filename = "file_counter_output.txt";
   

   x = open(filename, O_WRONLY | O_CREAT, 0666);

/*	
   write(x, nreg, 1);
   write(x, ndir, 1);
   write(x, nchr, 1); 
   write(x, nfifo, 1);
   write(x, nslink, 1);
   write(x, nsock, 1);
*/


printf("regular files = %7ld", nreg);
  printf("directories = %7ld %%\n", ndir);
  printf("block special = %7ld %%\n", nblk);
  printf("char special = %7ld, %%\n", nchr);
  printf("FIFOs = %7ld,  %%\n", nfifo);
  printf("symbolic links = %7ld %%\n", nslink);
  printf("sockets = %7ld %%\n", nsock);
  exit(ret);


}

#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4 /* file that we can't stat */

static int
myftw(char *pathname, Myfunc *func)
{
  	int len;
  	fullpath = malloc(PATH_MAX+1); /* malloc's for PATH_MAX+1 bytes */
  /*	
	if (pathlen <= strlen(pathname)) {
                pathlen = strlen(pathname) * 2;
                if ((fullpath = realloc(fullpath, pathlen)) == NULL)
                        return 0;//err_sys("realloc failed");
        }*/
	strncpy(fullpath, pathname, (PATH_MAX+1));
        fullpath[(PATH_MAX-1)] =0;
        return(dopath(func));
}

static int                      /* we return whatever func() returns */
dopath(Myfunc* func)
{
	struct stat statbuf;
  	struct dirent   *dirp;
  	DIR *dp;
  	int ret;
	char *ptr;
 		
 	if (lstat(fullpath, &statbuf) < 0) {

                return(func(fullpath, &statbuf, FTW_NS));
        }
        if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
                return(func(fullpath, &statbuf, FTW_F));

        if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
                return(ret);
	
	ptr = fullpath + strlen(fullpath); /* point to end of fullpath */
        *ptr++ = '/';
        *ptr = 0;

        if ((dp = opendir(fullpath)) == NULL) /* can't read directory */
                return(func(fullpath, &statbuf, FTW_DNR));

        while ((dirp = readdir(dp)) != NULL) {
                if (strcmp(dirp->d_name, ".") == 0 ||
                   strcmp(dirp->d_name, "..") == 0)
                        continue; /* ignore dot and dot-dot */
                strcpy(ptr, dirp->d_name); /* append name after slash */
                if ((ret = dopath(func)) != 0) /* recursive */
                         break; /* time to leave */
        }
        ptr[-1] = 0; /* erase everything from slash onwards */

        if (closedir(dp) < 0){
                printf("can't close directory %s", fullpath);
        }
        return(ret);


}

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
 	switch (type) {
 	case FTW_F:
 		switch (statptr->st_mode & S_IFMT) {
 		case S_IFREG: nreg++; break;
 		case S_IFBLK: nblk++; break;
 		case S_IFCHR: nchr++; break;
 		case S_IFIFO: nfifo++; break;
 		case S_IFLNK: nslink++; break;
 		case S_IFSOCK: nsock++; break;
		case S_IFDIR:
 			printf("for S_IFDIR for %s", pathname);
 	/* directories should have type = FTW_D */
 	}

	break;
 
	case FTW_D:
	  ndir++;
	  break;
	case FTW_DNR:
	  printf("can't read directory %s", pathname);
	  break;
	case FTW_NS:
	  printf("stat error for %s", pathname);
	  break;
	default:
	  printf("unknown type %d for pathname %s", type, pathname);
	}
	return(0);
}
