**Comando para compilar no Windows:**
c++ -o main.exe main.cpp -std=c++17
**Executar:**
./main.exe in.txt

**Comando para compilar no Linux:**
c++ -o main.exe main.cpp -std=c++17
**Executar:**
./main in.txt

O Esquema (WAIT_DIE ou WOUND_WAIT) está no arquivo scheme.txt
A saída fica no arquivo out.txt
Você pode ver o log em log.txt
Se não passar um arquivo como parâmetro, será usado in.txt como padrão
**Observação:**
Precisa de um compilador que suporte essa versão C++17