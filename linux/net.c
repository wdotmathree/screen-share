#include "net.h"

int tcp_connect(const char *host, short port) {
	int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}
	struct sockaddr_in6 addr;
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	struct addrinfo *res;
	int ret = getaddrinfo(host, NULL, NULL, &res);
	if (ret < 0) {
		perror("getaddrinfo");
		return -1;
	}
	memcpy(
		&addr.sin6_addr, &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, sizeof(struct in6_addr)
	);
	if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("connect");
		return -1;
	}
	return sockfd;
}

ssize_t tcp_send(int sockfd, const char *buf, int len) {
	ssize_t ret = send(sockfd, buf, len, 0);
	if (ret < 0) {
		perror("send");
		return -1;
	}
	return ret;
}

ssize_t tcp_recv(int sockfd, char *buf, int len) {
	ssize_t ret = recv(sockfd, buf, len, 0);
	if (ret < 0) {
		perror("recv");
		return -1;
	}
	return ret;
}

void tcp_close(int sockfd) {
	shutdown(sockfd, SHUT_RDWR);
}

int gen_addr(const char *host, int port, struct sockaddr_in6 *out) {
	out->sin6_family = AF_INET6;
	out->sin6_port = htons(port);
	struct addrinfo *res;
	int ret = getaddrinfo(host, NULL, NULL, &res);
	if (ret < 0) {
		perror("getaddrinfo");
		return -1;
	}
	memcpy(
		&out->sin6_addr, &((struct sockaddr_in6 *)res->ai_addr)->sin6_addr, sizeof(struct in6_addr)
	);
	return 0;
}

int udp_bind(int port) {
	int sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("socket");
		return -1;
	}
	struct sockaddr_in6 addr;
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_addr = in6addr_any;
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return -1;
	}
	return sockfd;
}

ssize_t udp_send(int sockfd, const char *buf, int len, const struct sockaddr_in6 *addr) {
	ssize_t ret = sendto(sockfd, buf, len, 0, (struct sockaddr *)addr, sizeof(*addr));
	if (ret < 0) {
		perror("sendto");
		return -1;
	}
	return ret;
}

ssize_t udp_recv(int sockfd, char *buf, int len) {
	struct sockaddr_in6 addr;
	socklen_t addrlen = sizeof(addr);
	ssize_t ret = recvfrom(sockfd, buf, len, 0, (struct sockaddr *)&addr, &addrlen);
	if (ret < 0) {
		perror("recvfrom");
		return -1;
	}
	return ret;
}
