#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>
#include "enums.hpp"

using namespace std;

struct Operation
{
    OP op;      // Operação
    int TID;    // Transação
    int opID;   // ID da operação (vem da historia de entrada)
    int itemID; // Item
};
class Transaction
{
public:
    // Identificador
    int ID;
    // TimeStamp
    int TS;
    // Estado da Transação
    STATE state;
    // Lista de Transações pelas quais esta espera
    vector<int> waitForList;
    // Lista de <Operações, Item> que já foram realizadas
    vector<Operation> operationsDone;
    // Lista de Operações em espera
    vector<Operation> operationsWaiting;
    Transaction(int transactionID, int timeStamp, STATE initialState)
    {
        ID = transactionID;
        TS = timeStamp;
        state = initialState;
    }
};
#endif