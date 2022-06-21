#include "transactionManager.hpp"
#include "lockManager.hpp"
#include <map>
#include <iostream>

using namespace std;

LockManager LM(WAIT_DIE);
map<string, int> dataID;
vector<string> result;

void handleInput(string operation, int transactionID, string item = "")
{
    if (operation == "BT")
    {
        TransactionManager::addTransaction(transactionID);
    }
    else if (operation == "C")
    {
        /*
        Transaction Tx = TransactionManager::getTransaction(transactionID);
        if (Tx.state == WAITING || Tx.state == ROLLBACKED)
        {
            Tx.operationsWaiting.emplace_back(make_pair(COMMIT, 0));
        }
        else if (Tx.state == ACTIVE)
        {
            for (auto T : TransactionManager::transactions)
            {
                if (T.state == ROLLBACKED)
                {
                    // Executar handleInput para cada op de operationsWaiting
                }
            }
            vector<LockTransaction> locks = LM.lockTable.removeLocks(Tx.ID);
            for (auto lock : locks)
            {
                auto itemTr = LM.waitForDataList[lock.item].front();
                int TID = itemTr.first;
                LOCK itemLock(itemTr.second);
                LockTransaction lt = LM.lockTable.getLock(lock.item, TID);
                if (itemLock == EXCLUSIVE)
                {
                    
                }
                else
                {
                    
                }
            }
        }*/
        outlog << "Commitar Transacao " << transactionID << endl;
    }
    else
    {
        OP op = (operation == "r") ? READ : WRITE; 
        // Adicione o item no map, se ainda não estiver mapeado
        if (dataID.find(item) == dataID.end())
            dataID[item] = dataID.size();
        int D = dataID[item];
        Transaction Tx = TransactionManager::getTransaction(transactionID);
        if (Tx.state == WAITING || Tx.state == ROLLBACKED)
        {
            Tx.operationsWaiting.emplace_back(make_pair(op, D));
        }
        // Leitura
        else if (op == READ)
        {
            outlog << "Transacao " << transactionID << " lendo dado " << D << endl;
            LM.LS(Tx, D, op);
        }
        // Escrita
        else
        {
            outlog << "Transacao " << transactionID << " escrevendo dado " << D << endl;
            LM.LX(Tx, D, op);
        }
    }
}
int main() {
    // Ler história
    /**
     * Ler linha
     * Caso 1: BT(X)
     * Instanciar objeto de transação addTransaction(X): Dúvida sobre o sentido do X
     * 
     * Caso 2: opX(item)
     * Verificar chave item no map dataID
     * Se existir, use itemID = dataID[item]
     * Senão, itemID = dataID.size()
     * Agora, vamos na LM.lockTable e fazemos checkLock(itemID)
     * 
     * Caso 2.1: checkLock retornou false
     * addLock(X, (op == READ) ? SHARED : EXCLUSIVE, itemID)
     * Adiciona op em X.operationsDone
     * Adiciona operação no escalonamento (result)
     * 
     * Caso 2.2: checkLock retornou true
     * Se você quiser fazer um read e tiver um shared lock no item de dado
     * addLock(X, SHARED, itemID)
     * Se você quiser fazer um read e tiver um exclusive lock no item de dado
     * Se o Protocolo for Wait-Die, se X.TS < Y.TS
     *  X entra em modo de espera
     *  waitForDataList[itemID].emplace_back(make_pair(X, SHARED)) adiada
     *  X.operationsWaiting.emplace_back(make_pair(op, itemID))
     * 
     **/
    TransactionManager::Tr = 0;
    ifstream in("in.txt");
    char c; 
    int paramID = 0;
    string param[3] = {"", "", ""};
    while (in >> c)
    {
        if (c == '(')
            paramID = 2;
        else if (c == ')')
        {
            paramID = 0;
            // BT(X) ou C(X)
            if (param[1].empty())
                handleInput(param[0], stoi(param[2]));
            else
                handleInput(param[0], stoi(param[1]), param[2]);
            // Reseta parâmetros para a próxima linha de entrada
            for (int i = 0; i < 3; i++)
                param[i] = "";
        }
        else
        {
            param[paramID] += c;
            if (paramID == 0 && (param[0] == "r" || param[0] == "w"))
                paramID++;
        }
    }   
    return 0;
}