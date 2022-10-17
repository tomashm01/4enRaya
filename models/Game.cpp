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
void Game::imprimirTablero()
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            cout << tablero[i][j] << " ";
        }
        cout << endl;
    }
}
int Game::colocarFicha(int columna)
{
    if (columna < 1 || columna > 8)
    {
        return -1;
    }
    columna--;
    for (int i = 5; i >= 0; i--)
    {
        if (tablero[i][columna] == '-')
        {
            if (this->getTurno() == 1)
            {
                tablero[i][columna] = 'o';
                this->setTurno(2);
            }
            else
            {
                tablero[i][columna] = 'x';
                this->setTurno(1);
            }
            return 1;
        }
    }
    return -1;
}

void Game::setTurno(int turno)
{
    this->turno = turno;
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
                if (tablero[i][j] == 'o')
                {
                    this->setGanador(1);
                }
                else
                {
                    this->setGanador(2);
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
                if (tablero[i][j] == 'o')
                {
                    this->setGanador(1);
                }
                else
                {
                    this->setGanador(2);
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
                if (tablero[i][j] == 'o')
                {
                    this->setGanador(1);
                }
                else
                {
                    this->setGanador(2);
                }
                return true;
            }
        }
    }
    // Comprobar que no hay 4 en raya en diagonal inversa
    for (int i = 3; i < 6; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (tablero[i][j] != '-' && tablero[i][j] == tablero[i - 1][j + 1] && tablero[i][j] == tablero[i - 2][j + 2] && tablero[i][j] == tablero[i - 3][j + 3])
            {
                if (tablero[i][j] == 'o')
                {
                    this->setGanador(1);
                }
                else
                {
                    this->setGanador(2);
                }
                return true;
            }
        }
    }
    return false;
}

bool Game::columnaLlena(int col)
{
    // Comprobar que la columna no está llena
    for (int i = 0; i < 6; i++)
    {
        if (tablero[i][col] == '-')
        {
            return false;
        }
    }
    return true;
}

void Game::setGanador(int ganador)
{
    this->ganador = ganador;
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

char Game::getTablero(int i, int j)
{
    return tablero[i][j];
}

void Game::setTablero(char tablero[6][7])
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            this->tablero[i][j] = tablero[i][j];
        }
    }
}
