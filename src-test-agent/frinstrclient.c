/*
 * frinstrclient.c
 *
 *  Created on: Mar 12, 2014
 *      Author: luigi
 */

/*
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>

#include "frlog.h"
#include "frjvmti.h"

static void _SendData(int sockfd, const void* data, int datalen) {
	int sent = 0;

	while (sent != datalen) {

		int res = send(sockfd, ((unsigned char *) data) + sent,
				(datalen - sent), 0);
		check_std_error(res, "Error while sending data to server");
		sent += res;
	}
}

static void _ReceiveData(int sockfd, void* data, int datalen) {
	int received = 0;

	while (received != datalen) {

		int res = recv(sockfd, ((unsigned char *) data) + received,
				(datalen - received), 0);
		check_std_error(res, "Error while receiving data from server");

		received += res;
	}
}

static int _ReceiveInt(int sockfd) {
	int data;
	_ReceiveData(sockfd, &data, sizeof(int));

	return ntohl(data);
}

static int _FrConnect() {
	struct addrinfo * addrinfo;
	int gai_res = getaddrinfo("localhost", "11357", NULL, &addrinfo);
	check_std_error(gai_res, "error getaddrinfo");

	int sockfd = socket(addrinfo->ai_family, SOCK_STREAM, 0);
	check_std_error(sockfd, "error socket");

	int conn_res = connect(sockfd, addrinfo->ai_addr, addrinfo->ai_addrlen);
	check_std_error(conn_res, "error connect");

	int flag = 1;
	int set_res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag,
			sizeof(int));
	check_std_error(set_res, "error setsockopt");

	freeaddrinfo(addrinfo);

	return sockfd;
}

static void _StartServer() {
	fprintf(stderr, "asdfasdfasdf\n");
	//int res =
	system("java");

	//fprintf(stderr, "res: %d\n", res);
//	java -cp ${FR}/bin:${FRLIB}/log4j-1.2.17.jar:${FRLIB}/asm-debug-all-4.0.jar:$FR/resources:$JARS \
//		ch.usi.inf.sape.frheap.server.FrHeapInstrumentServer > logs/server-output.log &

}

void FrInstrClassFileClientServer(jvmtiEnv* jvmti,
		const unsigned char* class_data, int class_data_len, const char* name,
		int* new_class_data_len, unsigned char** new_class_data) {

	int sockfd;

	if (tldget()->socketfd == -1) {
		_StartServer();
		tldget()->socketfd = _FrConnect();
	}

	sockfd = tldget()->socketfd;

	jint classNameLen = htonl(strlen(name));
	jint dl = htonl(class_data_len);

	_SendData(sockfd, &classNameLen, sizeof(jint));
	_SendData(sockfd, &dl, sizeof(jint));

	_SendData(sockfd, name, strlen(name));
	_SendData(sockfd, class_data, class_data_len);

	size_t newClassNameLen = _ReceiveInt(sockfd);
	jint newClassBytesLen = _ReceiveInt(sockfd);

	ASSERT(newClassNameLen == strlen(name), "");
	ASSERT(newClassBytesLen > 0, "");

	char* newClassName = malloc(newClassNameLen + 1);
	_ReceiveData(sockfd, newClassName, newClassNameLen);
	newClassName[newClassNameLen] = '\0';

	ASSERT(strcmp(newClassName, name) == 0, "");

	free(newClassName);

	*new_class_data_len = newClassBytesLen;
	FrAllocate(jvmti, newClassBytesLen, new_class_data);

	_ReceiveData(sockfd, *new_class_data, newClassBytesLen);
}
