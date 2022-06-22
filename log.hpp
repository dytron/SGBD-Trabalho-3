#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "enums.hpp"

int contador = 1;
ofstream out("out.txt");

// Log com cout
// #define outlog cout << contador++ << ". "

// Log no arquivo log.txt
// Comete essas duas linhas se quiser usar o cout
ofstream outfile("log.txt");
#define outlog outfile << contador++ << ". "
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