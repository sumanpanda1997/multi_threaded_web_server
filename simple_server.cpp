/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <queue>

#include <netinet/in.h>

#include <pthread.h>

#include "http_server.hh"

int max_client = 10000;
int count=0;
pthread_t thread_pool[16];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t items_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t space_available = PTHREAD_COND_INITIALIZER;

queue<int> buffer;

void error(char *msg) {
  perror(msg);
  exit(1);
}


void serve_client(int newsockfd)
{
  char buffer[1024];
  int n;
  HTTP_Response* response;
	
	bzero(buffer, 256);
	n = read(newsockfd, buffer, sizeof(buffer));
	if (n <= 0)
	{
	  perror("ERROR reading from socket");
	  delete response;
	  close(newsockfd);
	  return;
	}

	// printf("From Client : %s\n", buffer);
	response = handle_request(buffer);
	string response_str = response->get_string();
	//printf("\n\n%s\n\n", response_str.c_str());
	
	n = write(newsockfd, response_str.c_str(), response_str.size());
	if (n <= 0)
	{
	  perror("ERROR writing to socket");
	  delete response;
	  close(newsockfd);
	  return;
	}

	delete response;
	close(newsockfd);
	return;
}


void * start_function(void *arg)
{
	int sockfd;
	while(1)
	{
		pthread_mutex_lock(&mutex);
		while(count==0)
			pthread_cond_wait(&items_available, &mutex);
		
		count--;
		sockfd = buffer.front();
		buffer.pop();
		
		pthread_cond_signal(&space_available);
		pthread_mutex_unlock(&mutex);
		
		serve_client(sockfd);
		
	}	   
	return NULL;
}


int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */
	listen(sockfd, 10000);
	clilen = sizeof(cli_addr);
	
	/* create a thread pool of 8 threads*/
	for(int i=0; i<16; i++)
	{
		pthread_create(&thread_pool[i], NULL, start_function, NULL);
	}

	while(1)
	{
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
		if (newsockfd < 0)
		  error("ERROR on accept");
		  
		//enqueueing the newly accepted connection
		pthread_mutex_lock(&mutex);
		while(count==max_client)
			pthread_cond_wait(&space_available, &mutex);
		
		count++;	   
	  	buffer.push(newsockfd);
	  
		pthread_cond_signal(&items_available);
		pthread_mutex_unlock(&mutex);
	  	
	}



  return 0;
}
