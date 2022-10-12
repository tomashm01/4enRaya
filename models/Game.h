#ifndef game
#define game
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
#include "Jugador.h"

using namespace std;

class Game{
    private:
        char tablero[6][7];
        int turno;
        int ganador;
        Jugador* jugador1;
        Jugador* jugador2;
    public:
        Game(Jugador* jugador1, Jugador* jugador2);
        bool hayEspacios();
        void imprimirTablero();
        int colocarFicha(int columna);
        int getTurno();
        int getGanador();
        Jugador* getJugador1();
        Jugador* getJugador2();
        bool ganadorPartida();
        char getTablero(int i, int j);
};
#endif