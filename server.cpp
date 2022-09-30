#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

bool validateUser(string file_name, string user, string password)
{
    string line;
    // Pasa a char* el nombre del archivo
    ifstream myfile(file_name.c_str());
    if (myfile.is_open())
    {
        while (getline(myfile, line, ','))
        {
            if (line == user)
            {
                getline(myfile, line, ',');
                if (password == line)
                {
                    cout << "Usuario encontrado:" << password << ";" << endl;
                    return true;
                }
            }
        }
    }
    myfile.close();
    return false;
}

int main()
{
    string usuario, password;

    int sd, new_sd;
    struct sockaddr_in sockname, from;
    char buffer[100];
    socklen_t from_len;

    struct hostent *host;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("No se puede abrir el socket cliente\n");
        exit(1);
    }

    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2000);
    sockname.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&sockname, sizeof(sockname)) == -1)
    {
        perror("Error en la operación bind");
        exit(1);
    }

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        if (listen(sd, 1) == -1)
        {
            perror("Error en la operación listen");
            exit(1);
        }

        // Recibir usuario del cliente
        from_len = sizeof(from);
        new_sd = accept(sd, (struct sockaddr *)&from, &from_len);
        if (new_sd == -1)
        {
            perror("Error en la operación accept");
            exit(1);
        }

        recv(new_sd, buffer, 100, 0);

        // Separar usuario y contraseña del buffer y pasarlo a string
        string str(buffer);
        usuario = str.substr(0, str.find(";"));
        password = str.substr(str.find(";") + 1, str.length());

        memset(buffer, 0, sizeof(buffer));

        if (!validateUser("users.txt", usuario, password))
        {
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, "Usuario no encontrado");
            send(new_sd, buffer, 100, 0);
            
        }
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, "Usuario encontrado");
        send(new_sd, buffer, 100, 0);
        close(new_sd);
    }

    close(sd);
    return 0;
}