#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstdio>
#include <bitset>
#include <vector>
#include <math.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

using namespace std;
// duvidas
// sempre que uma transacao e criada ela recebe um numero - mas sempre que uma transacao e criada ela recebe um timestamp - esses valores nao vao ser sempre iguais?
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
    int Id;
    int Ts;
    string state;
    vector<int> waitForList;
};

class LockManager
{
public:
};