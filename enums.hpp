#ifndef ENUMS_H
#define ENUMS_H

enum LOCK {
    NONE,
    SHARED,
    EXCLUSIVE
};
enum STATE {
    WAITING,   // Em espera
    ACTIVE,    // Ativa
    COMMITED,  // Transação efetivada
    ROLLBACKED // Sofreu rollback
};
enum OP {
    START,
    READ,  // Leitura
    WRITE, // Escrita
    COMMIT // Efetivação
};
#endif