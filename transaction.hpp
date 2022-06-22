#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <vector>

//
enum STATE {
    WAITING,   // Em espera
    ACTIVE,    // Ativa
    COMMITED,  // Transação efetivada
    ROLLBACKED // Sofreu rollback
};
enum OP {
    READ,  // Leitura
    WRITE, // Escrita
    COMMIT // Efetivação
};

using namespace std;

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
    vector< pair<OP, int> > operationsDone;
    // Lista de Operações em espera
    vector< pair<OP, int> > operationsWaiting;
    Transaction(int transactionID, int timeStamp, STATE initialState)
    {
        ID = transactionID;
        TS = timeStamp;
        state = initialState;
    }
};
#endif