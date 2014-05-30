#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


typedef struct _QQpacket {
	int size;
	char type;	// m - message, f - file
	char data[0];
} QQpacket;

#include <stdio.h>

int Qpacket_send(char *clientip, char *port, QQpacket *p)
{
	printf("packet_size: %d type: %c\n", p->size, p->type);
	int ret = socket(PF_INET, SOCK_STREAM, 0);
	if (ret >= 0) {
		int s = ret;
		struct sockaddr_in sa;
		memset(&sa,0,sizeof(struct sockaddr_in));
    		sa.sin_family=AF_INET;
		sa.sin_port=htons(atoi(port));
		sa.sin_addr.s_addr=inet_addr(clientip);
		if ((ret = connect(s, (struct sockaddr *)&sa, sizeof(struct sockaddr_in))) >= 0) {
			if (send(s, p, p->size, 0) >= p->size) {
				close(s);
				return 0;
			} else s = -1;
		}
		close(s);
	}
	return ret;
}

int Qmsg_send(char *clientip, char *port, char *msg)
{
	int datalen = strlen(msg) + 1;
	QQpacket *packet = (QQpacket *)malloc(sizeof(QQpacket) + datalen);
	if (packet != NULL) {
		packet->size = sizeof(QQpacket) + datalen;
		packet->type = 'm';
		memcpy(packet->data, msg, datalen-1);
		packet->data[datalen] = '\0';

		
		if (!Qpacket_send(clientip, port, packet)) {
			free(packet);
			return 0;
		}
		free(packet);
	}
	perror(__FUNCTION__);
	return -1;
}

#include <fcntl.h>

#ifndef O_BINARY
#define O_BINARY	0
#endif

int Qfile_send(char *clientip, char *port, char *fname)
{
	int fd = open(fname, O_RDONLY | O_BINARY);
	if (fd >= 0) {
		int size = lseek(fd, 0, SEEK_END);
		QQpacket *packet = (QQpacket *)malloc(sizeof(QQpacket) + size);
		if (packet != NULL) {
			packet->size = sizeof(QQpacket) + size;
			packet->type = 'f';

			lseek(fd, 0, SEEK_SET);
			if (read(fd, packet->data, size) == size) {
				printf("packet_size: %d type: %c\n", packet->size, packet->type);
				if (!Qpacket_send(clientip, port, packet)) {
					free(packet);
					close(fd);
					return 0;
				}
			}
			free(packet);
		}
		close(fd);
	}
	perror(__FUNCTION__);
	return -1;
}