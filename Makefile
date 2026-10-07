# Makefile - TP2 (CIC0198) - Trabalho 2: sistema de backup com TDD
#
# Alvos principais:
#   make            compila o executável de testes (com flags de cobertura)
#   make test       compila e roda os testes (gtest)
#   make cpplint    verificação de estilo (Google C++ Style)
#   make cppcheck   análise estática (cppcheck --enable=warning .)
#   make valgrind   análise dinâmica de memória
#   make coverage   cobertura com gcov (falha se < 80% em backup.cpp)
#   make verifica   roda TODOS os verificadores acima (usar antes de commitar)
#   make doc        gera a documentação Doxygen em doxygen/html
#   make gdb        abre os testes no depurador gdb
#   make zip        gera o pacote de entrega (inclui .git)
#   make clean      remove artefatos gerados

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -O0
COVFLAGS := -ftest-coverage -fprofile-arcs
LDLIBS   := -lgtest -lgtest_main -pthread

MODULO   := backup
BIN      := testa_backup
FONTES   := backup.cpp backup.hpp testa_backup.cpp
ZIP      := 231011650_Lucas.zip
COB_MIN  := 80

.PHONY: all test cpplint cppcheck valgrind coverage verifica doc gdb zip clean

all: $(BIN)

# O módulo é compilado com instrumentação de cobertura; os testes não.
backup.o: backup.cpp backup.hpp
	$(CXX) $(CXXFLAGS) $(COVFLAGS) -c backup.cpp -o $@

testa_backup.o: testa_backup.cpp backup.hpp
	$(CXX) $(CXXFLAGS) -c testa_backup.cpp -o $@

$(BIN): backup.o testa_backup.o
	$(CXX) $(CXXFLAGS) $(COVFLAGS) $^ -o $@ $(LDLIBS)

test: $(BIN)
	./$(BIN)

cpplint:
	cpplint $(FONTES)

cppcheck:
	cppcheck --enable=warning --error-exitcode=1 --quiet \
	  --suppress=syntaxError .

valgrind: $(BIN)
	valgrind --leak-check=full --show-leak-kinds=all \
	  --errors-for-leak-kinds=definite --error-exitcode=1 ./$(BIN)

coverage: $(BIN)
	rm -f *.gcda *.gcov
	./$(BIN)
	gcov -r backup.cpp | tee gcov_resumo.txt
	@awk -F'[:%]' '/Lines executed/ { \
	  if ($$2 + 0 < $(COB_MIN)) { \
	    print ">> Cobertura " $$2 "% abaixo de $(COB_MIN)%"; exit 1 \
	  } else { print ">> Cobertura OK: " $$2 "%" } }' gcov_resumo.txt
	@echo ">> Linhas nao executadas (#####) em backup.cpp.gcov:"
	@grep -n '#####' backup.cpp.gcov || echo "   nenhuma"

verifica: cpplint cppcheck test valgrind coverage
	@echo ">> Todas as verificacoes passaram."

doc:
	doxygen Doxyfile
	@echo ">> Abra doxygen/html/index.html"

gdb: $(BIN)
	gdb ./$(BIN)

zip: clean
	rm -f $(ZIP)
	zip -r $(ZIP) . -x '$(ZIP)' '.vscode/*'
	@echo ">> Pacote gerado: $(ZIP)"

clean:
	rm -f *.o *.gcda *.gcno *.gcov gcov_resumo.txt $(BIN)
	rm -rf doxygen
