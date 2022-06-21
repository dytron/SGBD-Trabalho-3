#ifndef LOCKTABLE_H
#define LOCKTABLE_H

#include "log.hpp"
#include <fstream>

enum LOCK { NONE, SHARED, EXCLUSIVE };
// Estrutura que representa uma linha da LockTable
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
        LT << D << " " << getLockString(lock) << " " << transactionID << endl;
    }
    // Atualiza uma linha na LockTable
    void updateLock(int D, LOCK lock, int transactionID)
    {
        ifstream in(file);
        vector<LockTransaction> table;
        int itemID, TID;
        string lockStr;
        while (in >> itemID >> lockStr >> TID)
        {
            if (itemID == D && transactionID == TID)
                lockStr = getLockString(lock);
            table.emplace_back(LockTransaction(getLockEnum(lockStr), TID, itemID));
        }
        in.close();
        ofstream LT;
        LT.open(file);
        for (auto line : table)
            LT << line.item << " " << getLockString(line.lock) << " " << line.transactionID << endl;
        LT.close();
    }
    // Retorna ID da transação e tipo de bloqueio do item D
    LockTransaction getLock(int D, int transactionID = -1)
    {
        ifstream LT(file);
        int itemID, TID;
        LOCK lock;
        string lockStr;
        LockTransaction result(NONE);
        while (LT >> itemID >> lockStr >> TID)
        {
            lock = getLockEnum(lockStr);
            if (D == itemID)
            {            
                // Há um lock de outra transação para o mesmo dado (conflito!)
                if (TID != transactionID)
                    return LockTransaction(lock, TID);
                // Lock desta transação, vamos esperar até o fim para checar se é o único
                else
                    result = LockTransaction(lock, TID);
            }
        }
        return result;
    }
    // Retorna todos os locks feitos pela transação
    vector<LockTransaction> getAllLocks(int transactionID = -1)
    {
        ifstream LT(file);
        int itemID, TID;
        string lockStr;
        vector<LockTransaction> result;
        while (LT >> itemID >> lockStr >> TID)
            if (TID == transactionID)
                result.emplace_back(LockTransaction(getLockEnum(lockStr), TID, itemID));
        return result;
    }
    // Remove o lock sobre D feito pela transação
    void removeLock(int D, int transactionID)
    {
        ifstream in(file);
        vector<LockTransaction> table;
        string lockStr;
        int itemID, TID;
        while (in >> itemID >> lockStr >> TID)
        {
            // Se não for o lock que eu quero remover, continua na LockTable
            if (itemID != D || transactionID != TID)
                table.emplace_back(LockTransaction(getLockEnum(lockStr), TID, itemID));
        }
        in.close();
        ofstream LT;
        LT.open(file);
        for (auto line : table)
            LT << line.item << " " << getLockString(line.lock) << " " << line.transactionID << endl;
        LT.close();
    }
    string getLockString(LOCK lock)
    {
        if (lock == SHARED)
            return "S";
        return "X";
    }
    LOCK getLockEnum(string c)
    {
        if (c == "S")
            return SHARED;
        return EXCLUSIVE;
    }
    LockTable()
    {
        ofstream LT(file);
    }
};
#endif