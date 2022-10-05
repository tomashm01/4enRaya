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
    NOLOGGED, //El usuario aun no ha sido logeado. Puede haber introducido su usuario pero no su password
    LOGGED, //El usuario ha sido logeado con exito. Pero aun no ha iniciado partida 
    JUGANDO, //El usuario esta jugando una partida. Puede haber colocado ficha o no
    ESPERANDO //El usuario esta esperando una partida, porque no hay suficientes jugadores
};
class Cliente
{
private:
    int idSocket;
    string user;
    string pass;
    Estado state;
public:
    Cliente(int idSocket, string user, string pass, Estado state)
    {
        this->idSocket = idSocket;
        this->user = user;
        this->pass = pass;
        this->state = state;
    }
    Cliente(int idSocket,Estado state){
        this->idSocket = idSocket;
        this->user = "";
        this->pass = "";
        this->state = state;
    }
    int getIdSocket()
    {
        return this->idSocket;
    }
    string getUser()
    {
        return this->user;
    }
    string getPass()
    {
        return this->pass;
    }
    Estado getState()
    {
        return this->state;
    }
    void setIdSocket(int idSocket)
    {
        this->idSocket = idSocket;
    }
    void setUser(string user)
    {
        this->user = user;
    }
    void setPass(string pass)
    {
        this->pass = pass;
    }
    void setState(Estado state)
    {
        this->state = state;
    }
};