#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include "transactionManager.hpp"
#include "lockTable.hpp"
#include "log.hpp"
#include "enums.hpp"
#include <queue>
#include <map>

using namespace std;

#define WAIT_DIE 0
#define WOUND_WAIT 1

struct HistoryItem
{
    OP op;
    int TID;
    int item;
    string text;
};
class LockManager
{
public:
    int scheme;
    // Tabela de bloqueios sobre os itens de dados
    LockTable lockTable;
    // Mapeamento dos dados para inteiro
    map<string, int> dataID;
    vector<string> dataName;
    vector<HistoryItem> history;
    // Lista que contém, para cada item de dado, uma fila de transação e modo de bloqueio
    vector< queue< LockTransaction > > waitForDataList;
    vector<int> waitCount;
    LockManager(int scheme)
    {
        this->scheme = scheme;
    }
    // Lida com operações COMMIT, READ ou WRITE
    void handleCRW(Transaction &Tx, int D, OP op, int opID)
    {
        if (Tx.state == WAITING || Tx.state == ROLLBACKED)
        {
            outlog << "Transacao " << Tx.ID << " tem operacao " 
                << getOpName(op) << "(" << ((op == COMMIT) ? "" : dataName[D]) << ") postergada" << endl;
            Tx.operationsWaiting.push_back({op, Tx.ID, opID, D});
            showGraph(opID);
        }
        else if (op == COMMIT && Tx.state == ACTIVE)
        {
            outlog << "Commitar Transacao " << Tx.ID << endl;
            // Pegamos todos os locks feitos por esta transação
            vector<LockTransaction> locks = lockTable.getAllLocks(Tx.ID);
            // E liberamos um por um
            for (auto L : locks)
            {
                outlog << "Liberando Lock: " << dataName[L.item] << " " << lockTable.getLockString(L.lock) << endl;
                U(Tx, L.item);
            }
            Tx.state = COMMITED;
            addHistory(COMMIT, Tx.ID, 0);
            removeTransactionFromGraph(Tx.ID);
            showGraph(opID);
            // E liberamos um por um e executamos operações das transações que esperavam por este lock
            for (auto L : locks)
            {
                if (waitCount[L.item] < 0)
                    waitCount[L.item] = 0;
                while (waitCount[L.item] > 0)
                {
                    auto itemTr = waitForDataList[L.item].front();
                    Transaction &T = TransactionManager::getTransaction(itemTr.transactionID);
                    LockTransaction lockFound = lockTable.getLock(L.item, T.ID);
                    if (itemTr.transactionID != Tx.ID && itemTr.lock == EXCLUSIVE && (lockFound.lock == NONE || (lockFound.lock == SHARED && lockFound.transactionID == T.ID)))
                    {
                        waitForDataList[L.item].pop();
                        waitCount[L.item]--;
                        outlog << "Transacao " << T.ID << " agora esta ativa apos lock ser removido!" << endl;
                        T.state = ACTIVE;
                        // Executar handleCRW para cada op de operationsWaiting
                        for (auto opw : T.operationsWaiting)
                        {
                            handleCRW(T, opw.itemID, opw.op, opw.opID);
                        }
                    } else break;
                }
            }
            //outlog << "Checando todas as transacoes " << endl;
            for (auto &T : TransactionManager::transactions)
            {
                //outlog << "Estado da transacao " << T.ID << " = " << getStateName(T.state) << endl;
                if (T.state == ROLLBACKED)
                {
                    T.state = ACTIVE;
                    // Executar handleCRW para cada op de operationsWaiting
                    outlog << "Executando operacoes em espera da Transacao " << T.ID << endl;
                    vector repeatOperations(T.operationsWaiting);
                    T.operationsWaiting.clear();
                    for (auto opw : repeatOperations)
                    {
                        handleCRW(T, opw.itemID, opw.op, opw.opID);
                    }
                }
            }
        }
        // Leitura
        else if (op == READ)
        {
            outlog << "Transacao " << Tx.ID << " solicita READ " << dataName[D] << endl;
            LS(Tx, D, op, opID);
        }
        // Escrita
        else if (op == WRITE)
        {
            outlog << "Transacao " << Tx.ID << " solicita WRITE " << dataName[D] << endl;
            LX(Tx, D, op, opID);
        }

    }
    void handleCRW(Transaction &Tx, Operation o)
    {
        handleCRW(Tx, o.itemID, o.op, o.opID);
    }
    // Insere um bloqueio no modo compartilhado na Lock Table sobre o item D para a transação Tr se puder,
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tr
    void LS(Transaction &Tx, int D, OP op, int opID)
    {
        auto locktr = lockTable.getLock(D);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se o lock não for exclusivo, adiciono na lockTable e na lista de operações feitas
        if (locktr.transactionID == Tx.ID || locktr.lock != EXCLUSIVE)
        {
            if (locktr.transactionID != Tx.ID)
                lockTable.addLock(D, SHARED, Tx.ID);
            Tx.operationsDone.push_back({op, Tx.ID, opID, D});
            addHistory(op, Tx.ID, D);
            showGraph(opID);
        }
        else
        {
            Transaction Ty = TransactionManager::getTransaction(locktr.transactionID);
            handleConflict(Tx, Ty, D, op, EXCLUSIVE, opID);
        }
    }
    // Insere um bloqueio no modo exclusivo na Lock_Table sobre o item D para a transação Tx
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tx 
    void LX(Transaction &Tx, int D, OP op, int opID)
    {
        LockTransaction locktr = lockTable.getLock(D, Tx.ID);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se não houver lock, adiciono na lockTable e na lista de operações feitas
        if (locktr.lock == NONE)
        {
            //outlog << "Adiciona Lock. D = " << D << ", L = X, T = " <<  Tx.ID << endl; 
            lockTable.addLock(D, EXCLUSIVE, Tx.ID);
            Tx.operationsDone.push_back({op, Tx.ID, opID, D});
            addHistory(op, Tx.ID, D);
            showGraph(opID);
        }
        // Se houver um shared lock, mas for da mesma transação
        else if (locktr.transactionID == Tx.ID)
        {
            if (locktr.lock == SHARED)
            {
                outlog << "Atualiza lock\n";
                //outlog << "Atualiza Lock. D = " << D << ", L = X, T = " <<  Tx.ID << endl; 
                lockTable.updateLock(D, EXCLUSIVE, Tx.ID);
            }
            Tx.operationsDone.push_back({op, Tx.ID, opID, D});
            addHistory(op, Tx.ID, D);
            showGraph(opID);
        }
        else
        {
            if (scheme == WOUND_WAIT)
            {
                vector<LockTransaction> locks = lockTable.getAllLocksFromData(D);
                for (auto L : locks)
                {
                    Transaction &Ty = TransactionManager::getTransaction(L.transactionID);
                    if (Tx.ID != L.transactionID)
                        handleConflict(Tx, Ty, D, op, EXCLUSIVE, opID);
                }
                if (Tx.state == ACTIVE)
                    handleCRW(Tx, D, op, opID);
            }
            else
            {
                Transaction &Ty = TransactionManager::getTransaction(locktr.transactionID);
                handleConflict(Tx, Ty, D, op, EXCLUSIVE, opID);
            }
        }
    }
    // Apaga o bloqueio da transação Tr sobre o item D
    void U(Transaction &Tr, int D)
    {
        lockTable.removeLock(D, Tr.ID);
    }
    // Realiza rollback na transação (isMe é true se a op for de Tx)
    void rollback(Transaction &Tx, int D, OP op, int opID, bool isMe)
    {
        outlog << "Transacao " << Tx.ID << " sofre Rollback" << endl;
        // Rollback Tx
        Tx.state = ROLLBACKED;
        // Remover tudo o que estava no Tx.operationsDone do escalonamento
        deleteTransactionFromHistory(Tx.ID);
        if (isMe)
            Tx.operationsWaiting.push_back({op, Tx.ID, opID, D});
        // Tx.operationsDone passa para o waiting
        for (auto opw : Tx.operationsWaiting)
            Tx.operationsDone.push_back(opw);
        Tx.operationsWaiting.clear();
        for (auto opw : Tx.operationsDone)
            Tx.operationsWaiting.push_back(opw);
        Tx.operationsDone.clear();
        removeTransactionFromGraph(Tx.ID);
        string result = "";
        if (isMe && waitCount[D] > 0)
        {
            auto waitQ = queue(waitForDataList[D]);
            while (!waitQ.empty())
            {
                result += to_string(waitQ.front().transactionID) + ", ";
                waitQ.pop();
            }
            outlog << "Transacao " << Tx.ID << " sofreu rollback" << endl << "Fila de " << dataName[D] << " = " << result << endl;
        }
        showGraph(opID);
        out << "Lista do item de dado " << dataName[D] << " rollback = " << result << endl;
        // Libera todos os locks
        auto transactionLocks = lockTable.getAllLocks(Tx.ID);
        for (auto L : transactionLocks)
        {
            U(Tx, L.item);
        }
        for (auto L : transactionLocks)
        {
            if (waitCount[L.item] > 0)
            {
                LockTransaction TL = waitForDataList[L.item].front();
                int TID = TL.transactionID;
                outlog << "Transacao " << TID << " quer esse lock no item " << dataName[L.item] << endl;
                LockTransaction locktr = lockTable.getLock(L.item, TID);
                LOCK TLock = TL.lock;
                if (TID != Tx.ID)
                {
                    Transaction &T = TransactionManager::getTransaction(TID);
                    T.state = ACTIVE;
                    // Executar handleCRW para cada op de operationsWaiting
                    for (auto opw : T.operationsWaiting)
                    {
                        outlog << "Executando operacao que estava aguardando\n";
                        handleCRW(T, opw.itemID, opw.op, opw.opID);
                    }
                }
                waitForDataList[L.item].pop();
                waitCount[L.item]--;
            }
        }
    }
    // Transação Tx espera pelo dado D ser liberado
    void transactionWait(Transaction &Tx, int D, OP op, LOCK lock, int opID)
    {
        outlog << "Transacao " << Tx.ID << " espera" << endl;
        Tx.state = WAITING;
        waitForDataList[D].push(LockTransaction(lock, Tx.ID));
        waitCount[D]++;
        Tx.operationsWaiting.push_back({op, Tx.ID, opID, D});
        auto locks = lockTable.getAllLocksFromData(D);
        for (auto L : locks)
            if (L.transactionID != Tx.ID)
                Tx.waitForList.push_back(L.transactionID);
        showGraph(opID);
    }
    // Tx tenta acessar um dado bloqueado por Ty
    void handleConflict(Transaction &Tx, Transaction &Ty, int D, OP op, LOCK lock, int opID)
    {
        // Wait Die
        if (scheme == WAIT_DIE)
        {
            // Se Tx for a mais velha, espera por Ty
            if (Tx.TS < Ty.TS)
            {
                transactionWait(Tx, D, op, lock, opID);
            }
            // Se Tx for a mais nova, reinicia
            else
            {
                rollback(Tx, D, op, opID, true);
            }
        }
        // Wound Wait
        else
        {
            // Se Tx for a mais velha, dá rollback na mais nova
            if (Tx.TS < Ty.TS)
            {
                rollback(Ty, D, op, opID, false);
            }
            // Se Tx for a mais nova, espera
            else 
            {
                transactionWait(Tx, D, op, lock, opID);
            }
        }
    }
    // Retorna o índice do item de dado no mapeamento
    int getDataID(string item)
    {
        // Adicione o item no map, se ainda não estiver mapeado
        if (dataID.find(item) == dataID.end())
        {
            waitCount.push_back(0);
            waitForDataList.emplace_back(queue<LockTransaction>());
            dataID[item] = dataID.size();
            dataName.push_back(item);
        }
        return dataID[item];
    }
    // Obter nome da operação
    string getOpName(OP op)
    {
        if (op == COMMIT) return "COMMIT";
        if (op == READ) return "READ";
        return "WRITE";
    }
    // Obter nome do estado
    string getStateName(STATE s)
    {
        if (s == ACTIVE) return "ACTIVE";
        if (s == WAITING) return "WAITING";
        if (s == ROLLBACKED) return "ROLLBACKED";
        return "COMMITED";
    }
    // Mostrar Grafo
    void showGraph(int opID)
    {
        out << "Grafo apos comando " << opID << " = ";
        for (auto &T : TransactionManager::transactions)
        {
            for (auto waitingTID : T.waitForList)
            {
                out << T.ID << "->" << waitingTID << "; ";
            }
        }
        out << endl;
    }
    // Remover Transação do grafo
    void removeTransactionFromGraph(int TID)
    {
        TransactionManager::getTransaction(TID).waitForList.clear();
        for (auto &T : TransactionManager::transactions)
        {
            int index = -1, i = 0;
            while (i < T.waitForList.size())
            {
                if (T.waitForList[i] == TID)
                {
                    T.waitForList.erase(T.waitForList.begin() + i);
                }
                else i++;
            }
        }
    }
    // Adiciona operação na história realizada
    void addHistory(OP op, int transactionID, int item)
    {
        string text;
        if (op == COMMIT)
            text = "C(" + to_string(transactionID) + ")";
        else
            text = ((op == READ) ? "r" : "w") + to_string(transactionID) + "(" + dataName[item] + ")";
        history.push_back({op, transactionID, item, text});
    }
    // Remove todas as operações dessa transação na história até o momento
    void deleteTransactionFromHistory(int TID)
    {
        auto it = history.begin();
        while (it != history.end())
        {
            if ((*it).TID == TID)
            {
                it = history.erase(it);
            }
            else
                it++;
        }
    }
    // Mostrar História Realizada
    void showHistory()
    {
        out << "Historia realizada:\n";
        for (auto H : history)
        {
            out << H.text << endl;
        }
    }
};
#endif