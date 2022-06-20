#include "transactionManager.hpp"
#include "lockTable.hpp"
#include <queue>

using namespace std;

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