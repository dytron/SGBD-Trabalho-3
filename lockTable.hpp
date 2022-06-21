#ifndef LOCKTABLE_H
#define LOCKTABLE_H

#include "log.hpp"
#include <fstream>

enum LOCK { NONE, SHARED, EXCLUSIVE };

struct LockTransaction
{
    int item;
    LOCK lock;
    int transactionID;
    LockTransaction(LOCK lockType, int TID = -1, int D = -1)
    {
        lock = lockType;
        transactionID = TID;
        item = D;
    }
};
class LockTable
{
public:
    string file = "LockTable.txt";
    // Adiciona uma linha na LockTable
    void addLock(int D, LOCK lock, int transactionID)
    {
        ofstream LT(file, ios::app);
        LT << D << " " << lock << " " << transactionID << endl;
    }
    // Atualiza uma linha na LockTable
    void updateLock(int D, LOCK lock, int transactionID)
    {
        ifstream in(file);
        vector<LockTransaction> table;
        int itemID, lockInt, TID;
        while (in >> itemID >> lockInt >> TID)
        {
            if (itemID == D && transactionID == TID)
                lockInt = lock;
            table.emplace_back(LockTransaction(LOCK(lockInt), TID, itemID));
        }
        in.close();
        ofstream LT;
        LT.open(file);
        for (auto line : table)
            LT << line.item << " " << line.lock << " " << line.transactionID << endl;
        LT.close();
    }
    // Retorna ID da transação e tipo de bloqueio do item D
    LockTransaction getLock(int D, int transactionID = -1)
    {
        ifstream LT(file);
        int itemID, lockInt, TID;
        LOCK lock;
        LockTransaction result(NONE);
        while (!LT.eof())
        {
            LT >> itemID >> lockInt >> TID;
            lock = LOCK(lockInt);
            if (D == itemID)
            {            
                // Há um lock de outra transação para o mesmo dado (conflito!)
                if (TID != transactionID)
                    return LockTransaction(lock, TID);
                else
                    result = LockTransaction(lock, TID);
            }
        }
        return result;
    }
    vector<LockTransaction> removeLocks(int transactionID)
    {
        return vector<LockTransaction>();
    }
    LockTable()
    {
        ofstream LT(file);
    }
};
#endif