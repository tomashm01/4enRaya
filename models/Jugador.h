#ifndef jugador
#define jugador
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
using namespace std;

enum Estado
{
    LOGGEDWITHOUTPASSWORD, //Introduce usuario pero no contraseña
    NOLOGGED, //El usuario aun no ha introducido usuario ni password
    LOGGED, //El usuario ha sido logeado con exito 
    JUGANDO, //El usuario esta jugando una partida. Puede haber colocado ficha o no
    ESPERANDO //El usuario esta esperando una partida, porque no hay suficientes jugadores
};

class Jugador{
    private:
        string usuario;
        string password;
        int socketfd;
        Estado estado;
    public:
        Jugador(int idSocket, string user, string pass, Estado state);
        Jugador(int idSocket,string user,Estado state);
        Jugador(int idSocket,Estado state);
        int getIdSocket();
        string getUser();
        string getPass();
        Estado getState();
        void setIdSocket(int idSocket);
        void setUser(string user);
        void setPass(string pass);
        void setState(Estado state);
};
#endif