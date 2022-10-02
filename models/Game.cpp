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

using namespace std;

class Game{
    private:
        char tablero[6][7];
        int turno;
        int ganador;
    public:
    Game(){
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                tablero[i][j]='-';
            }
        }
        turno=1;
        ganador=0;
    }
    bool hayEspacios(){
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                if(tablero[i][j]=='-'){
                    return true;
                }
            }
        }
        return false;
    }
    void imprimirTablero(){
        cout<<"|1|2|3|4|5|6|7|"<<endl;
        for(int i=0;i<6;i++){
            for(int j=0;j<7;j++){
                cout<<"|"<<tablero[i][j];
            }
            cout<<"|"<<endl;
        }
    }
    int colocarFicha(int columna){
        if(columna<1 || columna>7){
            return -1;
        }
        columna--;
        for(int i=5;i>=0;i--){
            if(tablero[i][columna]=='-'){
                if(turno==1){
                    tablero[i][columna]='X';
                    turno=2;
                }else{
                    tablero[i][columna]='O';
                    turno=1;
                }
                return 1;
            }
        }
        return -1;
    }
    
    bool ganadorPartida(){
        //Comprobar que no hay 4 en raya en horizontal
        for(int i=0;i<6;i++){
            for(int j=0;j<4;j++){
                if(tablero[i][j]!='-' && tablero[i][j]==tablero[i][j+1] && tablero[i][j]==tablero[i][j+2] && tablero[i][j]==tablero[i][j+3]){
                    if(tablero[i][j]=='X'){
                        ganador=1;
                    }else{
                        ganador=2;
                    }
                    return true;
                }
            }
        }
        //Comprobar que no hay 4 en raya en vertical
        for(int i=0;i<3;i++){
            for(int j=0;j<7;j++){
                if(tablero[i][j]!='-' && tablero[i][j]==tablero[i+1][j] && tablero[i][j]==tablero[i+2][j] && tablero[i][j]==tablero[i+3][j]){
                    if(tablero[i][j]=='X'){
                        ganador=1;
                    }else{
                        ganador=2;
                    }
                    return true;
                }
            }
        }
        //Comprobar que no hay 4 en raya en diagonal
        for(int i=0;i<3;i++){
            for(int j=0;j<4;j++){
                if(tablero[i][j]!='-' && tablero[i][j]==tablero[i+1][j+1] && tablero[i][j]==tablero[i+2][j+2] && tablero[i][j]==tablero[i+3][j+3]){
                    if(tablero[i][j]=='X'){
                        ganador=1;
                    }else{
                        ganador=2;
                    }
                    return true;
                }
            }
        }
        return false;
    }
};