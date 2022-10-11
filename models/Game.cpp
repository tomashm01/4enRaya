#include "Game.h"

Game::Game(Jugador *jugador1, Jugador *jugador2)
{
    this->jugador1 = jugador1;
    this->jugador2 = jugador2;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            tablero[i][j] = '-';
        }
    }
    turno = 1;
    ganador = 0;
}

bool Game::hayEspacios()
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (tablero[i][j] == '-')
            {
                return true;
            }
        }
    }
    return false;
}
string Game::imprimirTablero()
{
    string tablero = "+Ok.Empieza la partida. ";
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            tablero += this->tablero[i][j] + ",";
        }
        tablero += ";";
    }
    return tablero;
}
int Game::colocarFicha(int columna)
{
    if (columna < 1 || columna > 7)
    {
        return -1;
    }
    columna--;
    for (int i = 5; i >= 0; i--)
    {
        if (tablero[i][columna] == '-')
        {
            if (turno == 1)
            {
                tablero[i][columna] = 'X';
                turno = 2;
            }
            else
            {
                tablero[i][columna] = 'O';
                turno = 1;
            }
            return 1;
        }
    }
    return -1;
}

bool Game::ganadorPartida()
{
    // Comprobar que no hay 4 en raya en horizontal
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tablero[i][j] != '-' && tablero[i][j] == tablero[i][j + 1] && tablero[i][j] == tablero[i][j + 2] && tablero[i][j] == tablero[i][j + 3])
            {
                if (tablero[i][j] == 'X')
                {
                    ganador = 1;
                }
                else
                {
                    ganador = 2;
                }
                return true;
            }
        }
    }
    // Comprobar que no hay 4 en raya en vertical
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            if (tablero[i][j] != '-' && tablero[i][j] == tablero[i + 1][j] && tablero[i][j] == tablero[i + 2][j] && tablero[i][j] == tablero[i + 3][j])
            {
                if (tablero[i][j] == 'X')
                {
                    ganador = 1;
                }
                else
                {
                    ganador = 2;
                }
                return true;
            }
        }
    }
    // Comprobar que no hay 4 en raya en diagonal
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tablero[i][j] != '-' && tablero[i][j] == tablero[i + 1][j + 1] && tablero[i][j] == tablero[i + 2][j + 2] && tablero[i][j] == tablero[i + 3][j + 3])
            {
                if (tablero[i][j] == 'X')
                {
                    ganador = 1;
                }
                else
                {
                    ganador = 2;
                }
                return true;
            }
        }
    }
    return false;
}

int Game::getTurno()
{
    return turno;
}

int Game::getGanador()
{
    return ganador;
}

Jugador *Game::getJugador1()
{
    return jugador1;
}

Jugador *Game::getJugador2()
{
    return jugador2;
}

