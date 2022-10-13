#include "models/Game.h"
#include "models/Jugador.h"
int main(){
    Jugador* j1 = new Jugador(1,"pepe","123",Estado::LOGGED);
    Jugador* j2 = new Jugador(2,"juan","123",Estado::LOGGED);
    Game* g = new Game(j1,j2);

    g->tableroMierda();
    g->imprimirTablero();
    if(g->ganadorPartida()){
        cout<<"Ganador: "<<g->getGanador()<<endl;
    }else{
        cout<<"No hay ganador"<<endl;
    }
}