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
#include <fstream>

class Server
{
private:
    int sd, new_sd;
    struct sockaddr_in sockname, from;
    char buffer[250];
    socklen_t from_len;
    struct hostent *host;

public:
    Server()
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
    int getNew_sd()
    {
        return new_sd;
    }
    void setNew_sd(int new_sd)
    {
        this->new_sd = new_sd;
    }
    struct sockaddr_in getSockname()
    {
        return sockname;
    }

    void setSockname(struct sockaddr_in sockname)
    {
        this->sockname = sockname;
    }
    struct sockaddr_in getFrom()
    {
        return from;
    }
    void setFrom(struct sockaddr_in from)
    {
        this->from = from;
    }
    char *getBuffer()
    {
        return buffer;
    }
    void setBuffer(char *buffer)
    {
        strcpy(this->buffer, buffer);
    }
    socklen_t getFrom_len()
    {
        return from_len;
    }
    void setFrom_len(socklen_t from_len)
    {
        this->from_len = from_len;
    }
    struct hostent *getHost()
    {
        return host;
    }
    void setHost(struct hostent *host)
    {
        this->host = host;
    }
    void bindListen(int port, int queue)
    {
        sockname.sin_family = AF_INET;
        sockname.sin_port = htons(port);
        sockname.sin_addr.s_addr = INADDR_ANY;

        if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
        {
            perror("Error en la operación bind");
            exit(1);
        }

        from_len = sizeof(from);

        if (listen(sd, queue) == -1)
        {
            perror("Error en la operación de listen");
            exit(1);
        }
    }
    void acceptRequest()
    {
        if ((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
        {
            perror("Error aceptando peticiones");
            exit(1);
        }
    }
    void closeSD()
    {
        close(sd);
    }
    void closeNewSD()
    {
        close(new_sd);
    }
    void sendRequest(char *buffer)
    {
        if (send(new_sd, buffer, strlen(buffer), 0) == -1)
        {
            perror("Error enviando peticiones");
            exit(1);
        }
    }
    void receiveRequest()
    {
        if (recv(new_sd, buffer, 100, 0) == -1)
        {
            perror("Error recibiendo peticiones");
            exit(1);
        }
    }
    void freeBuffer()
    {
        memset(buffer, 0, sizeof(buffer));
    }
};