#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <vector>

class Client
{
private:
    int sd;
    struct sockaddr_in sockname;
    char buffer[250];
    socklen_t len_sockname;

public:
    Client()
    {
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if (sd == -1)
        {
            perror("No se puede abrir el socket cliente\n");
            exit(1);
        }
    }
    int getSd()
    {
        return sd;
    }
    void setSd(int sd)
    {
        this->sd = sd;
    }
    struct sockaddr_in getSockname()
    {
        return sockname;
    }
    void setSockname(struct sockaddr_in sockname)
    {
        this->sockname = sockname;
    }
    char *getBuffer()
    {
        return buffer;
    }
    void setBuffer(char *buffer)
    {
        strcpy(this->buffer, buffer);
    }
    socklen_t getLen_sockname()
    {
        return len_sockname;
    }
    void setLen_sockname(socklen_t len_sockname)
    {
        this->len_sockname = len_sockname;
    }
    void connectToServer()
    {
        sockname.sin_family = AF_INET;
        sockname.sin_port = htons(2063);
        sockname.sin_addr.s_addr = inet_addr("127.0.0.1");
        len_sockname = sizeof(sockname);
        if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
        {
            perror("No se puede conectar\n");
            exit(1);
        }
    }
    void sendMsg(char *msg)
    {
        strcpy(buffer, msg);
        if (send(sd, msg, strlen(msg), 0) == -1)
        {
            perror("No se puede enviar mensaje\n");
            exit(1);
        }
    }
    void receiveMsg()
    {
        if (recv(sd, buffer, 100, 0) == -1)
        {
            perror("No se puede recibir mensaje\n");
            exit(1);
        }
    }
    void closeConnection()
    {
        close(sd);
    }
    void freeBuffer()
    {
        memset(buffer, 0, sizeof(buffer));
    }
};