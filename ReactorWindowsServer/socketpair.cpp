#pragma once
#include"socketpair.h"

static int __stream_socketpair(struct addrinfo* addr_info, SOCKET sock[2]) {
    SOCKET listener, client, server;
    int opt = 1;

    listener = server = client = INVALID_SOCKET;
    listener = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol); //创建服务器socket并进行绑定监听等
    if (INVALID_SOCKET == listener)
        goto fail;

    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if (SOCKET_ERROR == bind(listener, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(listener, addr_info->ai_addr, (int*)&addr_info->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == listen(listener, 5))
        goto fail;

    client = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol); //创建客户端socket，并连接服务器

    if (INVALID_SOCKET == client)
        goto fail;

    if (SOCKET_ERROR == connect(client, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    server = accept(listener, 0, 0);

    if (INVALID_SOCKET == server)
        goto fail;

    closesocket(listener);

    sock[0] = client;
    sock[1] = server;

    return 0;
fail:
    if (INVALID_SOCKET != listener)
        closesocket(listener);
    if (INVALID_SOCKET != client)
        closesocket(client);
    return -1;
}

static int __dgram_socketpair(struct addrinfo* addr_info, SOCKET sock[2])
{
    SOCKET client, server;
    struct addrinfo addr, * result = NULL;
    const char* address;
    int opt = 1;

    server = client = INVALID_SOCKET;

    server = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (INVALID_SOCKET == server)
        goto fail;

    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    if (SOCKET_ERROR == bind(server, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(server, addr_info->ai_addr, (int*)&addr_info->ai_addrlen))
        goto fail;

    client = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (INVALID_SOCKET == client)
        goto fail;

    memset(&addr, 0, sizeof(addr));
    addr.ai_family = addr_info->ai_family;
    addr.ai_socktype = addr_info->ai_socktype;
    addr.ai_protocol = addr_info->ai_protocol;

    if (AF_INET6 == addr.ai_family)
        address = "0:0:0:0:0:0:0:1";
    else
        address = "127.0.0.1";

    if (getaddrinfo(address, "0", &addr, &result))
        goto fail;

    setsockopt(client, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if (SOCKET_ERROR == bind(client, result->ai_addr, result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == getsockname(client, result->ai_addr, (int*)&result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == connect(server, result->ai_addr, result->ai_addrlen))
        goto fail;

    if (SOCKET_ERROR == connect(client, addr_info->ai_addr, addr_info->ai_addrlen))
        goto fail;

    freeaddrinfo(result);
    sock[0] = client;
    sock[1] = server;
    return 0;

fail:
    if (INVALID_SOCKET != client)
        closesocket(client);
    if (INVALID_SOCKET != server)
        closesocket(server);
    if (result)
        freeaddrinfo(result);
    return -1;
}

int socketpair(int family, int type, int protocol, long long recv[2]) {
    const char* address;
    struct addrinfo addr_info, * p_addrinfo;
    int result = -1;

    memset(&addr_info, 0, sizeof(addr_info));
    addr_info.ai_family = family;
    addr_info.ai_socktype = type;
    addr_info.ai_protocol = protocol;
    if (AF_INET6 == family)
        address = "0:0:0:0:0:0:0:1";
    else
        address = "127.0.0.1";

    if (0 == getaddrinfo(address, "0", &addr_info, &p_addrinfo)) {
        if (SOCK_STREAM == type)
            result = __stream_socketpair(p_addrinfo, (SOCKET*)recv);   //use for tcp
        else if (SOCK_DGRAM == type)
            result = __dgram_socketpair(p_addrinfo, (SOCKET*)recv);    //use for udp
        freeaddrinfo(p_addrinfo);
    }
    return result;
}