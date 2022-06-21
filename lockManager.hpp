#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include "transactionManager.hpp"
#include "lockTable.hpp"
#include "log.hpp"
#include <queue>
#include <map>

using namespace std;

#define WAIT_DIE 0
#define WOUND_WAIT 1

class LockManager
{
public:
    int protocolo;
    // Tabela de bloqueios sobre os itens de dados
    LockTable lockTable;
    // Mapeamento dos dados para inteiro
    map<string, int> dataID;
    // Lista que contém, para cada item de dado, uma fila de transação e modo de bloqueio
    vector< queue< pair<int, LOCK> > > waitForDataList;
    // Lida com operações COMMIT, READ ou WRITE
    void handleCRW(Transaction Tx, int D, OP op)
    {
        if (Tx.state == WAITING || Tx.state == ROLLBACKED)
        {
            Tx.operationsWaiting.emplace_back(make_pair(op, D));
        }
        else if (op == COMMIT && Tx.state == ACTIVE)
        {
            for (auto T : TransactionManager::transactions)
            {
                if (T.state == ROLLBACKED)
                {
                    T.state == ACTIVE;
                    // Executar handleCRW para cada op de operationsWaiting
                    for (auto opwaiting : Tx.operationsWaiting)
                    {
                        handleCRW(Tx, opwaiting.second, OP(opwaiting.first));
                    }
                }
            }
            vector<LockTransaction> locks = lockTable.getAllLocks(Tx.ID);
            for (auto L : locks)
            {
                auto itemTr = waitForDataList[L.item].front();
                int TID = itemTr.first;
                LOCK itemLock(itemTr.second);
                LockTransaction lt = lockTable.getLock(L.item, TID);
                if (itemLock == EXCLUSIVE)
                {
                    
                }
                else
                {
                    
                }
                U(Tx, L.item);
            }
            outlog << "Commitar Transacao " << Tx.ID << endl;
        }
        // Leitura
        else if (op == READ)
        {
            outlog << "Transacao " << Tx.ID << " lendo dado " << D << endl;
            LS(Tx, D, op);
        }
        // Escrita
        else if (op == WRITE)
        {
            outlog << "Transacao " << Tx.ID << " escrevendo dado " << D << endl;
            LX(Tx, D, op);
        }
    }
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
        lockTable.removeLock(D, Tr.ID);
    }
    // Retorna o índice do item de dado no mapeamento
    int getDataID(string item)
    {
        // Adicione o item no map, se ainda não estiver mapeado
        if (dataID.find(item) == dataID.end())
            dataID[item] = dataID.size();
        return dataID[item];
    }
    LockManager(int protocolo)
    {
        this->protocolo = protocolo;
    }
};
#endif