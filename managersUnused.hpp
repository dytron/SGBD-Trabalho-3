/*
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
#include <map>

using namespace std;
// duvidas
// sempre que uma transacao e criada ela recebe um numero - mas sempre que uma transacao e criada ela recebe um timestamp
// esses valores nao vao ser sempre iguais?
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
    void addLock(int D, LOCK lock)
    {

    }
    bool checkLock(int D)
    {

    }
    void removeLocks(int Tid)
    {

    }
};
*/