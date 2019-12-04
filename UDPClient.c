/*
 ============================================================================
 Name        : client_2ESONERO.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#define BUFFERSIZE 512
//#define PROTOPORT 27015
//DESKTOP-085SF43

void ClearWinSock() {
	#if defined WIN32
	WSACleanup();
	#endif
}

//Il parametro sock serve a far sì che sia la funzione a chiudere la socket
int ErrorHandler(char *s, int sock){
	printf("%s\n", s);
	closesocket(sock);
	ClearWinSock();
	return -1;
}

int main(void) {
	char name[100];
	char buf[BUFFERSIZE];
	char word[BUFFERSIZE];
	int PROTOPORT;
	int numVoc; 	//numero di vocali
	int servLen;	//dimensione dell'indirizzo del server
	struct hostent *hostip;
	struct sockaddr_in sad;

	printf("Inserisci un messaggio da mandare al server: ");
	fflush(stdin);
	fgets(buf, BUFFERSIZE, stdin);

	#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if (iResult != 0) {
		printf ("error at WSASturtup\n");
		return -1;
	}
	#endif

	printf("Inserire il nome del server al quale connettersi: "); //Il nome del server a cui connettersi è localhost
	scanf("%s", &name);
	hostip = gethostbyname(name);
	if (hostip == NULL) {
		printf("gethostbyname() failed.\n");
		exit(EXIT_FAILURE);
	}
	else {
		struct in_addr* ina = (struct in_addr*) hostip->h_addr_list[0];
		char *serverAddr = inet_ntoa(*ina);
		printf("Inserire il numero della porta a cui connettersi: ");
		scanf("%d", &PROTOPORT);

		int cSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(cSocket < 0){
			return ErrorHandler("Errore nella creazione della socket", cSocket);
		}

		// COSTRUZIONE DELL’INDIRIZZO DEL SERVER
		memset(&sad, 0, sizeof(sad));
		sad.sin_family = PF_INET;
		sad.sin_addr.s_addr = inet_addr(serverAddr);
		sad.sin_port = htons(PROTOPORT); // Server port
		servLen = sizeof(sad);

		if(sendto(cSocket, buf, sizeof(buf), 0, (struct sockaddr *)&sad, sizeof(sad)) != sizeof(buf)){
			ErrorHandler("Errore nell'invio del primo messaggio al server", cSocket);
		}

		recvfrom(cSocket, buf, BUFFERSIZE, 0, (struct sockaddr *) &sad, &servLen);
		printf("Il server ha inviato: %s",buf);
		strcpy(buf, " ");

		printf("\nInserisci una stringa alfanumerica: ");
		fflush(stdin);
		fgets(word, BUFFERSIZE, stdin);

		int i;
		for(i = 0; i < strlen(word); i++)
			word[i] = tolower(word[i]);

		numVoc = 0;
		printf("\nIl server ha inviato:");
		for(i = 0; i < strlen(word); i++){
			if((word[i] == 'a') || (word[i] == 'e') || (word[i] == 'i') || (word[i] == 'o') || (word[i] == 'u')){
				numVoc++;
				buf[0] = word[i];
				if(sendto(cSocket, buf, sizeof(buf), 0, (struct sockaddr *)&sad, sizeof(sad)) != sizeof(buf)){
					ErrorHandler("Errore nell'invio della vocale al server", cSocket);
				}
				recvfrom(cSocket, buf, BUFFERSIZE, 0, (struct sockaddr *) &sad, &servLen);
				printf("\n%s", buf);
			}
		}
		strcpy(buf, "c");
		sendto(cSocket, buf, sizeof(buf), 0, (struct sockaddr *)&sad, sizeof(sad));
		strcpy(buf, " ");
		closesocket(cSocket);
		ClearWinSock();
	}

	printf("\n");
	system("PAUSE");
	return EXIT_SUCCESS;
}



