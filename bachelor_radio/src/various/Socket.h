/*
 *    Copyright (C) 2018
 *    Matthias P. Braendli (matthias.braendli@mpb.li)
 *
 *    Copyright (C) 2017
 *    Albrecht Lohofener (albrechtloh@gmx.de)
 *
 *    This file is based on SDR-J
 *    Copyright (C) 2010, 2011, 2012, 2013
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *
 *
 */

#pragma once

#include <cstddef>
#include <string>

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>

    #ifndef MSG_NOSIGNAL
    # define MSG_NOSIGNAL 0
    #endif
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <sys/wait.h>
    #include <sys/types.h>
    #include <sys/time.h>
    #include <fcntl.h>

    #define INVALID_SOCKET (-1)
#endif

class Socket {
    public:
        Socket() = default;
        ~Socket();
        Socket(const Socket& other) = delete;
        Socket& operator=(const Socket& other) = delete;
        Socket(Socket&& other);
        Socket& operator=(Socket&& other);

        void close();
        bool valid() const;

        // Binds to any address
        bool bind(int port);
        bool listen();
        Socket accept();
        bool connect(const std::string& address, int port, int timeout);

        ssize_t recv(void *buffer, size_t length, int flags);
        ssize_t send(const void *buffer, size_t length, int flags);

    private:
        int sock = INVALID_SOCKET;
};
