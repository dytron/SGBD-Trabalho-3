#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "transaction.hpp"
#include "log.hpp"

using namespace std;

class TransactionManager
{
public:
    // TimeStamp (Contador de Transações)
    int TrTs = 1;
    vector<Transaction> transactions;
    void addTransaction(int id)
    {
        // Criar uma transação com id TrTs, estado ACTIVE
        // Incrementa TrTs
        // Adiciona no vetor transactions
        transactions.push_back(Transaction((TrTs++), ACTIVE));
        outlog << "adicionei no vetor de transacoes\n";
    }
};
#endif