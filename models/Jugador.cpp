#include "Jugador.h"

Jugador::Jugador(int idSocket, string user, string pass, Estado state){
    socketfd = idSocket;
    usuario = user;
    password = pass;
    estado = state;
}

Jugador::Jugador(int idSocket,string user,Estado state){
    socketfd = idSocket;
    usuario = user;
    estado = state;
}

Jugador::Jugador(int idSocket,Estado state){
    socketfd = idSocket;
    estado = state;
}

int Jugador::getIdSocket(){
    return socketfd;
}

string Jugador::getUser(){
    return usuario;
}

string Jugador::getPass(){
    return password;
}

Estado Jugador::getState(){
    return estado;
}

void Jugador::setIdSocket(int idSocket){
    socketfd = idSocket;
}

void Jugador::setUser(string user){
    usuario = user;
}

void Jugador::setPass(string pass){
    password = pass;
}

void Jugador::setState(Estado state){
    estado = state;
}