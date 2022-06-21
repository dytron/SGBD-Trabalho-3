#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include "transactionManager.hpp"
#include "lockTable.hpp"
#include "log.hpp"
#include <queue>

using namespace std;

#define WAIT_DIE 0
#define WOUND_WAIT 1

class LockManager
{
public:
    int protocolo;
    // Tabela de bloqueios sobre os itens de dados
    LockTable lockTable;
    // Lista que contém, para cada item de dado, uma fila de transação e modo de bloqueio
    vector< queue< pair<int, LOCK> > > waitForDataList;
    // Insere um bloqueio no modo compartilhado na Lock Table sobre o item D para a transação Tr se puder,
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tr
    void LS(Transaction Tx, int D, OP op)
    {
        auto locktr = lockTable.getLock(D);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se o lock não for exclusivo, adiciono na lockTable e na lista de operações feitas
        if (locktr.lock != EXCLUSIVE)
        {
            lockTable.addLock(D, SHARED, Tx.ID);
            Tx.operationsDone.emplace_back(make_pair(op, D));
        }
        else
        {
            Transaction Ty = TransactionManager::getTransaction(locktr.transactionID);
            if (protocolo == WAIT_DIE)
            {
                if (Tx.TS < Ty.TS)
                {
                    waitForDataList[D].push(make_pair(Tx.ID, SHARED));
                    Tx.operationsWaiting.emplace_back(make_pair(op, D));
                    Tx.state = WAITING;
                }
                else
                {
                    // Rollback Tx (STATE)
                    // Remover tudo o que estava no Tx.operationsDone do escalonamento
                    // Tx.operationsDone passa para o waiting
                }
            }
        }
    }
    // Insere um bloqueio no modo exclusivo na Lock_Table sobre o item D para a transação Tx
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tx 
    void LX(Transaction Tx, int D, OP op)
    {
        auto locktr = lockTable.getLock(D, Tx.ID);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se não houver lock, adiciono na lockTable e na lista de operações feitas
        if (locktr.lock == NONE)
        {
            outlog << "Adiciona Lock. D = " << D << ", L = X, T = " <<  Tx.ID << endl; 
            lockTable.addLock(D, EXCLUSIVE, Tx.ID);
            Tx.operationsDone.emplace_back(make_pair(op, D));
        }
        // Se houver um shared lock, mas for da mesma transação
        else if (locktr.lock == SHARED && locktr.transactionID == Tx.ID)
        {
            outlog << "Atualiza Lock. D = " << D << ", L = X, T = " <<  Tx.ID << endl; 
            lockTable.updateLock(D, EXCLUSIVE, Tx.ID);
            Tx.operationsDone.emplace_back(make_pair(op, D));
        }
        else
        {
            Transaction Ty = TransactionManager::getTransaction(locktr.transactionID);
            if (protocolo == WAIT_DIE)
            {
                if (Tx.TS < Ty.TS)
                {
                    waitForDataList[D].push(make_pair(Tx.ID, SHARED));
                    Tx.operationsWaiting.emplace_back(make_pair(op, D));
                }
            }
            
        }
    }
    // Apaga o bloqueio da transação Tr sobre o item D
    void U(Transaction Tr, int D)
    {

    }
    LockManager(int protocolo)
    {
        this->protocolo = protocolo;
    }
};
#endif