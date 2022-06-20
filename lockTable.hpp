#include <fstream>
#include <sstream>

enum LOCK { SHARED, EXCLUSIVE };
class LockTable
{
public:
    void addLock(int D, LOCK lock)
    {

    }
    bool checkLock(int D)
    {
        return false;
    }
    void removeLocks(int Tid)
    {

    }
};