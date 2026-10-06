# Instruções para agentes de IA (Copilot, Antigravity, etc.)

Trabalho acadêmico de **TP2 (CIC0198/UnB)**: sistema de backup em **C++17**
desenvolvido com **TDD estrito** e **Google Test**. O histórico do `.git` é
avaliado pelo professor, então a ordem dos commits importa.

## Leia primeiro
1. `Docs/PLANO_TDD.md`: tabela de decisão, roteiro dos 16 testes e convenções.
2. `Docs/TP2_trab_2.pdf`: enunciado oficial.
3. `backup.hpp`: interface já documentada.

## Regras obrigatórias
- **Um teste por vez**, ciclo RED → GREEN → REFACTOR, **um commit por fase**
  (`RED: coluna N - ...`, `GREEN: ...`, `REFACTOR: ...`).
- **Nunca** implementar em `backup.cpp` comportamento cujo teste ainda não exista.
- No GREEN, escrever o **mínimo** necessário.
- Antes do commit REFACTOR, `make verifica` deve passar (cpplint, cppcheck,
  testes, valgrind, cobertura ≥ 80%).
- **Pare ao fim de cada ciclo** e peça aprovação do usuário antes do próximo.
- Comentários de função: bloco Javadoc (`/** ... */`) no formato do slide 14
  (Função / Descrição / Parâmetros / Valor retornado / Assertiva de entrada /
  Assertiva de saída). Assertivas de entrada com `assert()`.
- Estilo Google C++ (`cpplint`), linhas ≤ 80 colunas, comentários em português.
- Arquivos fixos na raiz: `backup.cpp`, `backup.hpp`, `testa_backup.cpp`.

## Ambiente
Linux (GitHub Codespaces). Se o devcontainer não instalou as ferramentas:
`bash scripts/setup_ambiente.sh`.
