#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <bitset>
#include <vector>
#include <queue>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

enum LOCK { SHARED, EXCLUSIVE };
enum STATE { ACTIVE, COMMITED, ABORTED };

using namespace std;
// duvidas
// sempre que uma transacao e criada ela recebe um numero - mas sempre que uma transacao e criada ela recebe um timestamp
// esses valores nao vao ser sempre iguais?
class TransactionManager
{
public:
    int Tr = 0;
    int globalTimeStamp = 0;
    vector<Transaction> transactions;
    // O timestamp será um inteiro. Sempre que uma transação for criada, registra-se na transação o Ts atual e incrementa-se o Ts.
    void incTimeStamp()
    {
        this->globalTimeStamp++;
    }
    void incTr()
    {
        this->Tr++;
    }
};

class Transaction
{
public:
    // Identificador
    int Id;
    // Timestamp
    int Ts;
    // Estado da Transação
    STATE state;
    // Lista de Transações pelas quais esta espera
    vector<int> waitForList;
};

class LockManager
{
public:
    // Tabela de bloqueios sobre os itens de dados
    LockTable lockTable;
    // Lista que contém, para cada item de dado, uma fila de transação e modo de bloqueio
    vector< queue< pair<int, LOCK> > > waitForDataList;
    // Insere um bloqueio no modo compartilhado na Lock Table sobre o item D para a transação Tr se puder,
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tr
    void LS(Transaction Tr, int D)
    {

    }
    // Insere um bloqueio no modo exclusivo na Lock_Table sobre o item D para a transação Tr
    // Caso contrário cria/atualiza a Wait Q de D com a transação Tr 
    void LX(Transaction Tr, int D)
    {

    }
    // Apaga o bloqueio da transação Tr sobre o item D
    void U(Transaction Tr, int D)
    {

    }
};
class LockTable
{
public:
    void addLock()
    {

    }
    void removeLock()
    {

    }
};
