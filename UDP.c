#ifdef _WIN32 //windows
	#include <stdio.h>
	#define _WiN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <string.h>
	int OSInit(void)
	{
		WSADATA wsaData;
		int WSAError = WSAStartup(MAKEWORD(2,0), &wsaData);
		if (WSAError != 0)
		{
			fprintf(stderr, "WSAStartup errno = %d\n", WSAError);
			exit(-1);
		}
	}
	int OSCleanup(void)
	{
		WSACleanup();
	}
	#define perror(string) fprintf(stderr, string ":WSA errno = %d\n", WSAGetLastError())
#else //linux
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <string.h>
	OSInit(void){}
	OSCleanup(void){}
#endif

int initialization(struct sockaddr ** internet_address, socklen_t * internet_address_length);

void execution(int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length);

void cleanup(int internet_socket, struct sockaddr * internet_address);


int main(int argc, char * argv[])
{
	//INITITALIZATION
	
	OSInit();
	
	struct sockaddr * internet_address = NULL;
	socklen_t internet_address_length = 0;
	int internet_socket = initialization(&internet_address, &internet_address_length);
	
	//EXECUTION
	
	execution(internet_socket, internet_address, internet_address_length);
	
	//CLEAN UP
	
	cleanup(internet_socket, internet_address);
	OSCleanup();
	
	printf("compiler works !\n");
	return 0;
}

int initialization(struct sockaddr ** internet_address, socklen_t * internet_address_length)
{
	//step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo *internet_address_result = NULL;
	memset(&internet_address_setup, 0, sizeof internet_address_setup);
	internet_address_setup.ai_family = AF_UNSPEC;
	internet_address_setup.ai_socktype = SOCK_DGRAM;
	int getaddrinfo_return = getaddrinfo("::1", "24042", &internet_address_setup, &internet_address_result);
	if(getaddrinfo_return != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_return));
		exit(1);
	}
	
	//step 1.2
	int internet_socket;
	internet_socket = socket(internet_address_result->ai_family, internet_address_result->ai_socktype, internet_address_result->ai_protocol);
	if(internet_socket == -1)
	{
		perror("socket");
		freeaddrinfo(internet_address_result);
		exit(2);
	}
	
	//step 1.3
	*internet_address_length = internet_address_result->ai_addrlen;
	*internet_address = (struct sockaddr*) malloc(internet_address_result->ai_addrlen);
	memcpy(*internet_address, internet_address_result->ai_addr, internet_address_result->ai_addrlen);
	
	freeaddrinfo(internet_address_result);
	
	return internet_socket;
}

void execution(int internet_socket, struct sockaddr * internet_address, socklen_t internet_address_length)
{
	//step 2.1
	int number_of_bytes_send = 0;
	number_of_bytes_send = sendto(internet_socket, "hello UDP world", 16, 0, internet_address->ai_addr, internet_address->ai_addrlen);
	if(number_of_bytes_send == -1)
	{
		perror("sendto");
	}
	
	//step 2.2
	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recvfrom(internet_socket, buffer, (sizeof buffer) -1, 0, internet_address, &internet_address_length);
	if(number_of_bytes_received)
	{
		perror("recvfrom");
	}
	else
	{
		buffer[number_of_bytes_received] = '\0';
		printf("Received : %s\n", buffer);
	}
	
}
void cleanup(int internet_socket, struct sockaddr * internet_address)
{
	//step 3.2
	free (internet_address);
	
	//step 3.1
	close(internet_socket);
}