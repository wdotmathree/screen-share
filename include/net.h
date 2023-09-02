#pragma once
#ifndef _NET_H
#define _NET_H

#include "include.h"

#ifdef _WIN32
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

/**
 * @brief Connect to a TCP server.
 * @param host The hostname or IP address of the server
 * @param port The port number of the server
 * @return The sockfd of the connected socket, or -1 on error.
 */
int tcp_connect(const char *, short);

/**
 * @brief Send data to a TCP peer.
 * @param sockfd The socket file descriptor
 * @param buf The buffer to send
 * @param len The length of the buffer
 * @return Number of bytes sent, or -1 on error.
 */
ssize_t tcp_send(int, const char *, int);

/**
 * @brief Receive data from a TCP peer.
 * @param sockfd The socket file descriptor
 * @param buf The buffer to store the data
 * @param len The length of the buffer
 * @return Number of bytes received, or -1 on error.
 */
ssize_t tcp_recv(int, char *, int);

/**
 * @brief Close a socket.
 * @param sockfd The socket file descriptor
 */
void sock_close(int);

/**
 * @brief Generate a sockaddr_in6 struct from a hostname and port.
 * @return 0 on success, or -1 on error.
 */
int gen_addr(const char *, int, struct sockaddr_in6 *);

/**
 * @brief Create a UDP socket and bind it to a port.
 * @param port The port number to bind to
 * @return The sockfd of the socket, or -1 on error.
 */
int udp_bind(int);

/**
 * @brief Send data to a UDP peer.
 * @param sockfd The socket file descriptor
 * @param buf The buffer to send
 * @param len The length of the buffer
 * @param addr The address of the peer
 * @return Number of bytes sent, or -1 on error.
 */
ssize_t udp_send(int, const char *, int, const struct sockaddr_in6 *);

/**
 * @brief Receive data from a UDP peer.
 * @param sockfd The socket file descriptor
 * @param buf The buffer to store the data
 * @param len The length of the buffer
 * @return Number of bytes received, or -1 on error.
 */
ssize_t udp_recv(int, char *, int);

#endif
