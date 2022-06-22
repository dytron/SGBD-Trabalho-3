#include "lockManager.hpp"
#include <iostream>

using namespace std;

LockManager LM(WAIT_DIE);
vector<string> result;
// Lida com uma entrada da história
void handleInput(string operation, int transactionID, string item, int opID)
{
    // Início da Transação
    if (operation == "BT")
    {
        LM.showGraph(opID);
        return TransactionManager::addTransaction(transactionID);
    }
    Transaction &Tx = TransactionManager::getTransaction(transactionID);
    // Commit
    if (operation == "C")
        return LM.handleCRW(Tx, 0, COMMIT, opID);
    // Read ou Write
    OP op = (operation == "r") ? READ : WRITE;
    return LM.handleCRW(Tx, LM.getDataID(item), op, opID);
}
int main(int argc, char** args) {
    TransactionManager::Tr = 0;
    // Obter Esquema (Wait Die ou Wound Wait)
    ifstream schemeIn;
    schemeIn.open("scheme.txt");
    string schemeString;
    if (!(schemeIn >> schemeString))
        schemeString = "WAIT_DIE";
    schemeIn.close();
    ofstream schemeOut;
    schemeOut.open("scheme.txt");
    schemeOut << schemeString << endl;
    schemeOut.close();
    if (schemeString != "WAIT_DIE")
        LM.scheme = WOUND_WAIT;
    // Ler entrada
    ifstream in((argc > 1) ? args[1] : "in.txt");
    char c; 
    int paramID = 0;
    string param[3] = {"", "", ""};
    int opID = 1;
    while (in >> c)
    {
        if (c == '(')
            paramID = 2;
        else if (c == ')')
        {
            paramID = 0;
            // BT(X) ou C(X)
            if (param[1].empty())
                handleInput(param[0], stoi(param[2]), "", opID);
            else
                handleInput(param[0], stoi(param[1]), param[2], opID);
            // Reseta parâmetros para a próxima linha de entrada
            for (int i = 0; i < 3; i++)
                param[i] = "";
            opID++;
        }
        else
        {
            param[paramID] += c;
            if (paramID == 0 && (param[0] == "r" || param[0] == "w"))
                paramID++;
        }
    }  
    LM.showHistory();
    outlog << "Fim\n";
    return 0;
}