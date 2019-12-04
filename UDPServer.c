/*
 ============================================================================
 Name        : server_2ESONERO.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define PROTOPORT 27015 // default protocol port number
#define QLEN 6 // size of request queue
#define BUFFERSIZE 512

int ErrorHandler(char *s, int sock){
	printf("%s\n", s);
	closesocket(sock);
	ClearWinSock();
	return -1;
}

void ClearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

int main(int argc, char *argv[]) {
	int port;
	int clientSocket;
	int msgSize;  //dimensione del messaggio ricevuto dal client
	int clientLen;
	struct sockaddr_in cad;
	struct sockaddr_in sad;
	char buf[BUFFERSIZE];


	if (argc > 1)
		port = atoi(argv[1]);
	else
		port = PROTOPORT;

	if (port < 0) {
		printf("bad port number %s \n", argv[1]);
		return 0;
	}

	#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != 0) {
			printf("Error at WSAStartup()\n");
			return 0;
		}
	#endif

	//CREAZIONE SOCKET
	int mySocket;
	mySocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mySocket < 0)
		ErrorHandler("Errore nella creazione della socket", mySocket);

	//COSTRUZIONE DELL'INDIRIZZO DEL SERVER
	memset(&sad, 0, sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(port);
	sad.sin_addr.s_addr = inet_addr("127.0.0.1");

	//BIND
	if((bind(mySocket, (struct sockaddr *) &sad, sizeof(sad))) < 0){
		ErrorHandler("Errore nel bind", mySocket);
	}


	while(1){
		clientLen = sizeof(cad);
		recvfrom(mySocket, buf, BUFFERSIZE, 0, (struct sockaddr*) &cad, &clientLen);

		//Viene ricavato il nome del client
		struct hostent *host;
		char *clientName;
		host = gethostbyaddr((char*)&cad.sin_addr.s_addr, 4, PF_INET);
		clientName = host->h_name;

		printf("Messaggio ricevuto: %s", buf);
		printf("Nome host mittente: %s", clientName);
		strcpy(buf, " ");

		strcpy(buf, "OK");
		if(sendto(mySocket, buf, sizeof(buf), 0, (struct sockaddr *)&cad, &clientLen) != sizeof(buf)){
			ErrorHandler("Errore nell'invio del messaggio di conferma al client", mySocket);
		}

		int flag;
		flag = 0;
		strcpy(buf, " "); 	//Il buffer viene resettato
		do{
			msgSize = recvfrom(mySocket, buf, BUFFERSIZE, 0, (struct sockaddr*) &cad, &clientLen);
			if((buf[0] == 'a') || (buf[0] == 'e') || (buf[0] == 'i') || (buf[0] == 'o') || (buf[0] == 'u')){
				buf[0] = toupper(buf[0]);
				if(sendto(mySocket, buf, sizeof(buf), 0, (struct sockaddr *)&cad, &clientLen) != msgSize){
					ErrorHandler("Errore nell'invio dei dati al client", mySocket);
				}
			}
			else
				flag = 1;
		}while(flag == 0);
		strcpy(buf, " ");
		printf("\n\n");
	}

	system("PAUSE");
	return EXIT_SUCCESS;
}
