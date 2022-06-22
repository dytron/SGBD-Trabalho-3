#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "lockTable.hpp"

// Log no console
int contador = 1;
#define outlog cout << contador++ << ". "
ofstream out("out.txt");
// Log no arquivo log.txt (Comente o outro se usar este)
// ofstream outlog("log.txt");
void logLock(int D, LOCK L, int T)
{
    if (L == NONE)
    {
    //    outlog << "Nenhum Bloqueio encontrado. D = " << D << endl;
    }
    else {
            // outlog << "Bloqueio encontrado" << endl;
            //<< ", D = " << D
            //<< ", L = " << ((L == SHARED) ? 'S' : 'X')
            //<< ", T = " << T << endl;
    }
}
#endif