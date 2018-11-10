#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>		//optarg

#include <sys/socket.h>		//socket
#include <sys/types.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>
//------------directory exists or not library -----------------
#include <dirent.h>
#include <errno.h>


void reverse_array (char rev_this[], int size)
{
  char temp;
  int i = 0;
  while (i < size / 2)
    {
      temp = rev_this[i];
      rev_this[i] = rev_this[size - i - 1];
      rev_this[size - i - 1] = temp;
      i++;
    }
}

void copy_array (char to_copy[], char to_fill[], int size)
{
  int i = 0;
  while (i < size && to_copy[i] != '\0')
    {
      to_fill[i] = to_copy[i];
      i++;
    }
}

int main (int argc, char *argv[])
{

  char *address = argv[argc-1];
  

  int sockfd;
  struct addrinfo hints, *ai;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
//------------------------------------getopt------------------------------------------------------	
  int opt;
  char *filepath_d = NULL;
  char *filepath_o = NULL;
  char *port_option = "80";
  while ((opt = getopt (argc, argv, "p:o:d:")) != -1)
    {
      switch (opt)
	{
	case 'p':
	  port_option = strdup(optarg);
	  break;
	case 'o':
	  filepath_o = strdup(optarg);
	  break;
	case 'd':
	  filepath_d = strdup(optarg);
	  break;
	default:		/* '?' */
	  fprintf (stderr, "Usage: %s [-p PORT] [-d PATH| -o PATH] \n",
		   argv[0]);
	  exit (EXIT_FAILURE);
	}
    }
	  if (optind >= argc){
      fprintf (stderr, "Expected argument after options\n");
      exit (EXIT_FAILURE);
    }
//-------------------------------------------------------------------------------------------------	

//-------------------------------------Adresse zuschneiden tokens----------------------------------
	char* host;
  	char* format;
  	char* rest;
    if((format = strtok(address, "/"))!=NULL);
    if((host = strtok(NULL, "/"))!=NULL);
    if((rest = strtok(NULL," ")) == NULL){rest = "";}; //damit die request nicht anfängt zu spinnen
    printf("this is the format: %s\n",format);
    printf("this is the host: %s\n",host);
    printf("this is the rest of the URL: %s\n\n",rest);
//-------------------------------------------------------------------------------------------------	  

//------------------------------------Adresse bestimmen--------------------------------------------
	int res = getaddrinfo(host,port_option, &hints, &ai);
	if (res != 0) {
		printf("address error\n");
	}
	sockfd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);
	if(sockfd<0){
		printf("socket error\n");
	}
//-------------------------------------------------------------------------------------------------		

//------------------------------------Connect------------------------------------------------------	
	int test = connect(sockfd, ai->ai_addr, ai->ai_addrlen);
	if (test< 0) {
		printf("connection error\n"); 
	}
//-------------------------------------------------------------------------------------------------

//------------------------------------Request Senden-----------------------------------------------
	FILE *sockfile = fdopen(sockfd, "r+");
	char request[1024];
	sprintf(request,"GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",rest,host);
	fputs(request,sockfile);
	fflush(sockfile); // send all buffered data
//-------------------------------------------------------------------------------------------------

//------------------------------------Empfangen & Ausgeben-----------------------------------------
/*	char buf[1024];
	while (fgets(buf, sizeof(buf), sockfile) != NULL){
		fputs(buf, stdout);
	}
	fclose(sockfile);
	close(socket);*/
//-------------------------------------------------------------------------------------------------

//---------------------------------------Option Handling-------------------------------------------
	if(filepath_d!=NULL){
  		char outname[2048];
  		printf("Hi I was here\n");
 	 	if (filepath_d[((int)strlen (filepath_d)) - 1] == '/') //integer casten damit ich berechnungen drauf machen kann
    		{
    			int size = (strlen(filepath_d)-1);
    			printf("%i\n",size );
    			printf("Hi I was here 2\n");
      			sprintf (outname, "%s%s", filepath_d, "index.html");
    		}
  		else if(filepath_d[((int)strlen (filepath_d)) -1] == NULL)
    		{
    			printf("Hi I was here 3\n");
    			sprintf (outname, "%s%s", filepath_d, "index.html");
     	//	 	strcpy(filepath_d, outname); string concatinate  and str copy important functiona
    		}
    	else{	
    			printf("yes that's the file!\n");
    			int rest_size = strlen(rest);
    			reverse_array(rest,rest_size);
    			if((format = strtok(rest, "/"))!=NULL);
    			int rest_size_update = strlen(rest);
    			reverse_array(rest,rest_size_update);
    			sprintf(outname,"%s%s",filepath_d,rest);
    		}

    	FILE *flptr_1 = fopen(outname,"w+");
    	if(flptr_1==NULL){
    		exit(EXIT_FAILURE);
    	}

    	char buf[1024];
    	rewind(sockfile);
		while (fgets(buf, sizeof(buf), sockfile) != NULL){
			fputs(buf, flptr_1);
		}
		fclose(sockfile);
		
	}else if(filepath_o!=NULL){

		char outname[2048];
		strcat(outname,filepath_o);
		FILE *flptr_1 = fopen(outname,"a+");
    	if(flptr_1==NULL){
    		exit(EXIT_FAILURE);
    	}

    	char buf[1024];
    	rewind(sockfile);
		while (fgets(buf, sizeof(buf), sockfile) != NULL){
			fputs(buf, flptr_1);
		}
		fclose(sockfile);
		
	}
//---------------------------------------Option Handling-------------------------------------------

  	return 0;
} 
