#include "transaction.hpp"

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
        TrTs++;
        // Adiciona no vetor transactions
    }
};