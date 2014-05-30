#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#include "cfg.h"

typedef struct _QQpacket {
	int size;
	char type;	// m - message, f - file
	char data[0];
} QQpacket;

#include <stdlib.h>

int QQpacket_handle(char *clientip, char *buf, int sz)
{
	static char *stream = NULL;
	static int p = 0, maxsize = 0;
	if (p + sz > maxsize)
		stream = realloc(stream, (p+sz)*1.5);
	memcpy(&stream[p], buf, sz);
	p += sz;
	QQpacket *k = (QQpacket *)stream;

	if (k->size > p) return 0;
	if (k->type == 'm') Qmsg_put(clientip, k->data, k->size-sizeof(QQpacket));
	else if (k ->type == 'f')
		Qfile_put(clientip, k->data, k->size-sizeof(QQpacket));
	memmove(stream, &stream[k->size], p - k->size), p -= k->size;

}

void *server_init()
{
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror(__FUNCTION__);
		return NULL;
	}
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(atoi(QPORT));

	if (bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		perror(__FUNCTION__);
		return NULL;
	}
	listen(fd, 3);
	int s;
	struct sockaddr_storage remote_sa;
#define BUFSIZE	1024
	char buf[BUFSIZE];
	char clientip[64];
	socklen_t slen = sizeof(struct sockaddr_storage);

	while((s = accept(fd, (struct sockaddr *)&remote_sa,
		&slen)) >= 0) {
		strcpy(clientip, inet_ntoa(((struct sockaddr_in *)&remote_sa)->sin_addr));
		int g;		
		while((g = recv(s, buf, BUFSIZE, 0)) > 0)
			QQpacket_handle(clientip, buf, g);
		close(s);
	}
	close(fd);
	return (void *)1;
}