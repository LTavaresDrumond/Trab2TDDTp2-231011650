# Checklist de Revisão C/C++ - TP2 (Sistema de Backup)

A revisão do código foi realizada ao final dos ciclos TDD, garantindo que a implementação atenda aos padrões exigidos pela disciplina.

## 1. Padrões de Codificação e Estilo
- [X] **Google C++ Style Guide:** O código foi verificado e validado com a ferramenta `cpplint`, não apresentando nenhum aviso ou erro de estilo. Linhas possuem no máximo 80 caracteres.
- [X] **Nomenclatura:** Variáveis e métodos estão com a nomenclatura correta (métodos em PascalCase, variáveis em snake_case).
- [X] **Comentários e Documentação:** Funções e stubs possuem bloco Javadoc (`/** ... */`) no formato do slide 14, contendo Descrição, Parâmetros, Retorno, Assertivas de Entrada (`@pre`) e de Saída (`@post`).

## 2. Robustez e Assertivas
- [X] **Assertivas de Entrada:** As funções possuem `assert()` para garantir pré-condições (ex: `assert(!caminho_parm.empty());`, `assert(fs::is_directory(dir_hd));`).
- [X] **Ausência de Comportamento Indefinido (UB):** A ferramenta `cppcheck` não acusou erros sintáticos ou lógicos na base de código.
- [X] **Manipulação de Arquivos:** Verificação da existência de arquivos usando `std::filesystem::exists` e cópias seguras com `fs::copy_file`.

## 3. Gerenciamento de Memória
- [X] **Ausência de Vazamentos (Leaks):** Executado `valgrind --leak-check=full`. Nenhum vazamento de memória "definite", "indirect", ou "possible" foi detectado. Todas as alocações dinâmicas padrão (via `std::string` e `std::vector`) foram tratadas corretamente pelo RAII do C++.

## 4. Testes e TDD
- [X] **Cobertura de Testes:** Executado `gcov` com `make coverage`. A cobertura foi confirmada acima do mínimo exigido de 80% (o resultado foi **83,72%**).
- [X] **Fluxo TDD (RED-GREEN-REFACTOR):** Histórico de commits (`git log`) comprova o desenvolvimento incremental. Cada coluna da Tabela de Decisão originou 3 commits isolados.
- [X] **Casos de Teste (Caixa Fechada):** Todos os testes definidos nas tabelas de decisão (13 colunas) + testes repetitivos iterativos (3 colunas) executaram e passaram no Google Test.

## Conclusão da Revisão
Código limpo, seguro, modularizado e totalmente verificado pelo pipeline de integração (`make verifica`). Nenhuma anomalia técnica foi encontrada.
