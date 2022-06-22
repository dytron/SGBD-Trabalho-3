#include "transactionManager.hpp"
#include "lockManager.hpp"
#include <iostream>

using namespace std;

LockManager LM(WAIT_DIE);
vector<string> result;
// Lida com uma entrada da história
void handleInput(string operation, int transactionID, string item = "")
{
    // Início da Transação
    if (operation == "BT")
        return TransactionManager::addTransaction(transactionID);
    Transaction &Tx = TransactionManager::getTransaction(transactionID);
    // Commit
    if (operation == "C")
        return LM.handleCRW(Tx, 0, COMMIT);
    // Read ou Write
    OP op = (operation == "r") ? READ : WRITE;
    return LM.handleCRW(Tx, LM.getDataID(item), op);
}
int main(int argc, char** args) {
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
    ifstream in((argc > 1) ? args[1] : "in.txt");
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
    outlog << "Fim\n";
    return 0;
}