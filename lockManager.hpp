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

struct HistoryItem
{
    OP op;
    int TID;
    int item;
    string text;
    HistoryItem(OP operation, int transactionID, int itemData, string txt)
    {
        op = operation;
        TID = transactionID;
        item = itemData;
        text = txt;
    }
};
class LockManager
{
public:
    int protocolo;
    // Tabela de bloqueios sobre os itens de dados
    LockTable lockTable;
    // Mapeamento dos dados para inteiro
    map<string, int> dataID;
    vector<string> dataName;
    vector<HistoryItem> history;
    // Lista que contém, para cada item de dado, uma fila de transação e modo de bloqueio
    vector< queue< pair<int, LOCK> > > waitForDataList;
    vector<int> waitCount;
    // Lida com operações COMMIT, READ ou WRITE
    void handleCRW(Transaction &Tx, int D, OP op)
    {
        if (Tx.state == WAITING || Tx.state == ROLLBACKED)
        {
            outlog << "Transacao " << Tx.ID << " tem operacao " 
                << getOpName(op) << "(" << ((op == COMMIT) ? "" : dataName[D]) << ") postergada" << endl;
            Tx.operationsWaiting.emplace_back(make_pair(op, D));
            showGraph();
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
            for (auto &T : TransactionManager::transactions)
            {
                int index = -1, i = 0;
                while (i < T.waitForList.size())
                {
                    if (T.waitForList[i] == Tx.ID)
                    {
                        T.waitForList.erase(T.waitForList.begin() + i);
                    }
                    else i++;
                }
            }
            showGraph();
            // E liberamos um por um e executamos operações das transações que esperavam por este lock
            for (auto L : locks)
            {
                if (waitCount[L.item] <= 0) continue;
                auto waitQ = queue(waitForDataList[L.item]);
                string result;
                while (!waitQ.empty())
                {
                    result += to_string(waitQ.front().first) + ", ";
                    waitQ.pop();
                }
                if (waitCount[L.item] < 0)
                {
                    waitForDataList[L.item] = queue< pair<int, LOCK> >();
                    waitCount[L.item] == 0;
                }
                while (waitCount[L.item] > 0)// && waitForDataList[L.item].front().second != EXCLUSIVE)
                {
                    auto itemTr = waitForDataList[L.item].front();
                    Transaction &T = TransactionManager::getTransaction(itemTr.first);
                    waitForDataList[L.item].pop();
                    waitCount[L.item]--;
                    if (T.ID != Tx.ID)
                    {
                        outlog << "Transacao " << T.ID << " agora esta ativa apos lock ser removido!" << endl;
                        T.state = ACTIVE;
                        // Executar handleCRW para cada op de operationsWaiting
                        for (auto opw : T.operationsWaiting)
                            handleCRW(T, opw.second, opw.first);
                    }
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
                        handleCRW(T, opw.second, opw.first);
                    }
                }
            }
            
        }
        // Leitura
        else if (op == READ)
        {
            outlog << "Transacao " << Tx.ID << " solicita READ " << dataName[D] << endl;
            LS(Tx, D, op);
        }
        // Escrita
        else if (op == WRITE)
        {
            outlog << "Transacao " << Tx.ID << " solicita WRITE " << dataName[D] << endl;
            LX(Tx, D, op);
        }

    }
    // Insere um bloqueio no modo compartilhado na Lock Table sobre o item D para a transação Tr se puder,
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tr
    void LS(Transaction &Tx, int D, OP op)
    {
        auto locktr = lockTable.getLock(D);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se o lock não for exclusivo, adiciono na lockTable e na lista de operações feitas
        if (locktr.transactionID == Tx.ID || locktr.lock != EXCLUSIVE)
        {
            if (locktr.transactionID != Tx.ID)
                lockTable.addLock(D, SHARED, Tx.ID);
            Tx.operationsDone.emplace_back(make_pair(op, D));
            addHistory(op, Tx.ID, D);
            showGraph();
        }
        else
        {
            Transaction Ty = TransactionManager::getTransaction(locktr.transactionID);
            handleConflict(Tx, Ty, D, op, EXCLUSIVE);
        }
    }
    // Insere um bloqueio no modo exclusivo na Lock_Table sobre o item D para a transação Tx
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tx 
    void LX(Transaction &Tx, int D, OP op)
    {
        LockTransaction locktr = lockTable.getLock(D, Tx.ID);
        logLock(D, locktr.lock, locktr.transactionID);
        // Se não houver lock, adiciono na lockTable e na lista de operações feitas
        if (locktr.lock == NONE)
        {
            //outlog << "Adiciona Lock. D = " << D << ", L = X, T = " <<  Tx.ID << endl; 
            lockTable.addLock(D, EXCLUSIVE, Tx.ID);
            Tx.operationsDone.emplace_back(make_pair(op, D));
            addHistory(op, Tx.ID, D);
            showGraph();
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
            Tx.operationsDone.emplace_back(make_pair(op, D));
            addHistory(op, Tx.ID, D);
        }
        else
        {
            Transaction &Ty = TransactionManager::getTransaction(locktr.transactionID);
            handleConflict(Tx, Ty, D, op, EXCLUSIVE);
        }
    }
    // Apaga o bloqueio da transação Tr sobre o item D
    void U(Transaction &Tr, int D)
    {
        lockTable.removeLock(D, Tr.ID);
    }
    // Tx tenta acessar um dado bloqueado por Ty
    void handleConflict(Transaction &Tx, Transaction &Ty, int D, OP op, LOCK lock)
    {
        if (protocolo == WAIT_DIE)
        {
            // Tx espera por Ty
            if (Tx.TS < Ty.TS)
            {
                outlog << "Transacao " << Tx.ID << " espera" << endl;
                Tx.state = WAITING;
                waitForDataList[D].emplace(make_pair(Tx.ID, lock));
                waitCount[D]++;
                Tx.operationsWaiting.emplace_back(make_pair(op, D));
                //Tx.waitForList.emplace_back(Ty.ID);
                auto locks = lockTable.getAllLocksFromData(D);
                for (auto L : locks)
                    if (L.transactionID != Tx.ID)
                        Tx.waitForList.emplace_back(L.transactionID);
                showGraph();
            }
            else
            {
                // Rollback Tx
                Tx.state = ROLLBACKED;
                // Remover tudo o que estava no Tx.operationsDone do escalonamento
                deleteTransactionFromHistory(Tx.ID);
                // Tx.operationsDone passa para o waiting
                Tx.operationsWaiting.emplace_back(make_pair(op, D));
                for (auto opWait : Tx.operationsWaiting)
                    Tx.operationsDone.emplace_back(opWait);
                Tx.operationsWaiting = vector(Tx.operationsDone);
                Tx.operationsDone.clear();
                auto waitQ = queue(waitForDataList[D]);
                string result;
                while (!waitQ.empty())
                {
                    result += to_string(waitQ.front().first) + ", ";
                    waitQ.pop();
                }
                outlog << "Transacao " << Tx.ID << " sofreu rollback" << endl << "Fila de " << dataName[D] << " = " << result << endl;
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
                        pair <int, LOCK> trLock = waitForDataList[L.item].front();
                        int TID = trLock.first;
                        outlog << "Transacao " << TID << " quer esse lock no item " << dataName[D] << endl;
                        LOCK TLock = trLock.second;
                        if (TID != Tx.ID)
                        {
                            Transaction &T = TransactionManager::getTransaction(TID);
                            T.state = ACTIVE;
                            // Executar handleCRW para cada op de operationsWaiting
                            for (auto opwaiting : T.operationsWaiting)
                            {
                                outlog << "Executando operacao que estava aguardando\n";
                                handleCRW(T, opwaiting.second, opwaiting.first);
                            }
                        }
                        waitForDataList[L.item].pop();
                        waitCount[L.item]--;
                    }
                }
            }
        }
    }
    // Retorna o índice do item de dado no mapeamento
    int getDataID(string item)
    {
        // Adicione o item no map, se ainda não estiver mapeado
        if (dataID.find(item) == dataID.end())
        {
            waitForDataList.emplace_back(queue< pair<int, LOCK> >());
            dataID[item] = dataID.size();
            waitCount.emplace_back(0);
            dataName.emplace_back(item);
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
    LockManager(int protocolo)
    {
        this->protocolo = protocolo;
    }
    void showGraph()
    {
        out << "Grafo: ";
        for (auto &T : TransactionManager::transactions)
        {
            for (auto waitingTID : T.waitForList)
            {
                out << T.ID << "->" << waitingTID << "; ";
            }
        }
        out << endl;
    }
    void addHistory(OP op, int transactionID, int item)
    {
        string text;
        if (op == COMMIT)
            text = "C(" + to_string(transactionID) + ")";
        else
            text = ((op == READ) ? "r" : "w") + to_string(transactionID) + "(" + dataName[item] + ")";
        outlog << "H add " << text << endl;
        history.emplace_back(HistoryItem(op, transactionID, item, text));
    }
    void deleteTransactionFromHistory(int TID)
    {
        auto it = history.begin();
        while(it != history.end())
        {
            if((*it).TID == TID)
            {
                outlog << "H remove " << (*it).text << endl;
                it = history.erase(it);
            }
            else
                it++;
        }
    }
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