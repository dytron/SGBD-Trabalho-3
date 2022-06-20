#include "managers.hpp"
#include <map>
#include <iostream>

using namespace std;

int main() {
    TransactionManager TM();
    LockManager LM();
    map<string, int> dataID;
    vector<string> result;
    // Ler história
    /**
     * Ler linha
     * Caso 1: BT(X)
     * Instanciar objeto de transação addTransaction(X): Dúvida sobre o sentido do X
     * 
     * Caso 2: opX(item)
     * Verificar chave item no map dataID
     * Se existir, use dataID[item]
     * Senão, dataID[item] = dataID.size()
     * Agora, vamos na LM.lockTable e fazemos checkLock(dataID[item])
     * 
     * Caso 2.1: checkLock retornou false
     * addLock(X, (op == READ) ? SHARED : EXCLUSIVE, dataID[item])
     * Adiciona op em X.operationsDone
     * Adiciona operação no escalonamento (result)
     * 
     * Caso 2.2: checkLock retornou true
     * 
     **/
    return 0;
}