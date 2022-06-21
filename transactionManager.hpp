#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "transaction.hpp"
#include "log.hpp"

using namespace std;

class TransactionManager
{
public:
    // TimeStamp (Contador de Transações)
    static inline int Tr;
    // Lista de Transações
    static inline vector<Transaction> transactions;
    // Adicionar Transação
    static void addTransaction(int id)
    {
        // Criar uma transação com id Tr, estado ACTIVE, incrementa Tr e adiciona no vetor transactions
        transactions.emplace_back(Transaction(id, Tr++, ACTIVE));
        outlog << "Adicionei Transacao " << id << " com TS = " << Tr - 1 << " no vetor de transacoes\n";
    }
    // Obter transação pelo ID
    static Transaction getTransaction(int id)
    {
        for (auto T : transactions)
            if (T.ID == id)
                return T;
        outlog << "ID de Transação Inválido! (" << id << ")" << endl;
        exit(1);
    }
};

#endif