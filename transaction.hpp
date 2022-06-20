#include <vector>

enum STATE { WAITING, ACTIVE, COMMITED };
enum OP { READ, WRITE, COMMIT };

using namespace std;

class Transaction
{
public:
    // Identificador
    int Id;
    // Estado da Transação
    STATE state;
    // Lista de Transações pelas quais esta espera
    vector<int> waitForList;
    // Lista de <Operações, Item> que já foram realizadas
    vector< pair<OP, int> > operationsDone;
    // Lista de Operações em espera
    vector< pair<OP, int> > operationsWaiting;
};
