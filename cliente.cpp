#include <cstdio>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char **argv)
{
    string usuario, password;
    int c;

    if (argc < 3)
    {
        cout << "Error: No se ha introducido los datos de entrada correctos" << endl;
        return -1;
    }

    while ((c = getopt(argc, argv, "u:p:")) != -1)
    {
        switch (c)
        {
        case 'u':
            usuario = optarg;
            break;
        case 'p':
            password = optarg;
            break;
        case '?':
            cout << "Error: Opcion invalida" << endl;
            break;
        default:
            cout << "Opcion no valida" << endl;
            break;
        }
    }

    // Enviar usuario y contraseña al servidor por TCP (socket)
    int sd;
    struct sockaddr_in sockname;
    char buffer[100];
    socklen_t len_sockname;

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2000);
    sockname.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Enviar usuario y contraseña al servidor
    if (connect(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
    {
        perror("Error en la conexion\n");
        exit(1);
    }

    // Enviar usuario al servidor
    string data = usuario + ";" + password;
    strcpy(buffer, data.c_str());
    if (send(sd, buffer, strlen(buffer), 0) == -1)
    {
        perror("Error en el envio\n");
        exit(1);
    }

    // Recibir respuesta del servidor
    if (recv(sd, buffer, sizeof(buffer), 0) == -1)
    {
        perror("Error en la recepcion\n");
        exit(1);
    }
    cout << buffer << endl;

    close(sd);

    return 0;
}