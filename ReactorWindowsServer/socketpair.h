#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <WS2tcpip.h>

static int __stream_socketpair(struct addrinfo* addr_info, SOCKET sock[2]);
static int __dgram_socketpair(struct addrinfo* addr_info, SOCKET sock[2]);
int socketpair(int family, int type, int protocol, long long recv[2]);