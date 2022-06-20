#ifndef LOCKTABLE_H
#define LOCKTABLE_H

#include <fstream>
#include <sstream>

enum LOCK { NONE, SHARED, EXCLUSIVE };
class LockTable
{
public:
    void addLock(int D, LOCK lock)
    {

    }
    LOCK checkLock(int D)
    {
        return NONE;
    }
    void removeLocks(int Tid)
    {

    }
};
#endif