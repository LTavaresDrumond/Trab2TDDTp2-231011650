# Plano TDD — TP2 Trabalho 2 (Sistema de Backup)

> Documento de contexto. Quem abrir o repositório (pessoa ou agente de IA)
> deve começar por aqui. Fonte: `Docs/TP2_trab_2.pdf` e os prints dos slides
> em `Docs/`.

## 1. O que o sistema faz

- O arquivo `Backup.parm` lista os arquivos (um por linha) que devem ser espelhados.
- **Backup**: copia do HD para o pendrive. **Restauração**: copia do pendrive para o HD.
- "HD" e "pendrive" são **diretórios** quaisquer (o enunciado permite).
- A ação para cada arquivo `ArqX` vem da **tabela de decisão** (slides 23–26).

## 2. Tabela de decisão (slide 26, transcrita)

`B` = backup (HD → PenD) · `R` = restauração (PenD → HD) · `-` = não se aplica

| Col | Tem parm | Operação | ArqX ∈ HD | ArqX ∈ PenD | Data PenD vs HD | Ação             | `Acao` no código    |
|----:|:--------:|:--------:|:---------:|:-----------:|:---------------:|------------------|---------------------|
| 1   | F        | -        | -         | -           | -               | **Impossível**   | `kImpossivel`       |
| 2   | V        | B        | V         | F           | -               | HD → Pen-drive   | `kHdParaPendrive`   |
| 3   | V        | B        | V         | V           | PenD < HD       | HD → Pen-drive   | `kHdParaPendrive`   |
| 4   | V        | B        | V         | V           | PenD == HD      | Faz nada         | `kNada`             |
| 5   | V        | B        | V         | V           | PenD > HD       | Erro             | `kErro`             |
| 6   | V        | R        | V         | F           | -               | Erro             | `kErro`             |
| 7   | V        | R        | V         | V           | PenD < HD       | Erro             | `kErro`             |
| 8   | V        | R        | V         | V           | PenD == HD      | Faz nada         | `kNada`             |
| 9   | V        | R        | V         | V           | PenD > HD       | Pen-drive → HD   | `kPendriveParaHd`   |
| 10  | V        | B        | F         | F           | -               | Erro             | `kErro`             |
| 11  | V        | B        | F         | V           | -               | Faz nada         | `kNada`             |
| 12  | V        | R        | F         | F           | -               | Erro             | `kErro`             |
| 13  | V        | R        | F         | V           | -               | Pen-drive → HD   | `kPendriveParaHd`   |

Observações:
- A ação **Excluir** aparece na lista do slide 25, mas **nenhuma coluna a marca**.
  Ela existe no `enum Acao` apenas por fidelidade; `DecideAcao` nunca a retorna.
- A contagem do rodapé do slide (64+8+8+8+8+16+16 = 128 = 2⁷) confirma que as
  13 colunas cobrem todas as combinações das 7 condições.
- "Impossível deve cancelar a execução": `ExecutaBackup` retorna
  `Resultado::kImpossivel` sem tocar em nenhum arquivo (sem `abort()`, para ser testável).
- "Erro (idErro)": o ArqX não é copiado e uma mensagem é adicionada em
  `Relatorio::mensagens_erro`. Os demais arquivos continuam sendo processados.

## 3. Interface (já definida em `backup.hpp`)

```cpp
Acao DecideAcao(bool tem_parm, Operacao op, bool arq_no_hd,
                bool arq_no_pendrive, ComparacaoData data);   // tabela pura
Relatorio ExecutaBackup(const std::string& caminho_parm,
                        const std::string& dir_hd,
                        const std::string& dir_pendrive,
                        Operacao op);                         // usa arquivos reais
```

`Relatorio` = `{ resultado, acoes (uma por ArqX), mensagens_erro }`.
Os stubs atuais devolvem `kNada` / `{kErro, {}, {}}`, de modo que **todo teste
de coluna começa vermelho**.

## 4. Regras do ciclo (cada teste = 3 commits, no mínimo)

| Fase         | O que fazer                                                                 | Mensagem de commit                          |
|--------------|-----------------------------------------------------------------------------|---------------------------------------------|
| **RED**      | Adicionar só o novo `TEST_F` em `testa_backup.cpp`. `make test` deve **falhar**. | `RED: coluna N - <descrição>`              |
| **GREEN**    | Código **mínimo** em `backup.cpp` para passar. `make test` passa.            | `GREEN: coluna N - <descrição>`            |
| **REFACTOR** | Limpar código, assertivas, comentários. `make verifica` passa inteiro.       | `REFACTOR: coluna N - <o que melhorou>`    |

- Nunca implementar comportamento de uma coluna antes do teste dela existir.
- Se um teste novo passar direto (já coberto), registrar isso no commit RED
  (`RED: coluna N - passou sem mudança, já coberto por ...`) e ainda assim fazer o REFACTOR.
- Meta: 16 testes × 3 = **48 commits** (mínimo exigido: 30).

## 5. Roteiro dos testes

Todos usam a fixture `BackupTest` (ambiente temporário com `hd/`, `pendrive/`
e `Backup.parm`). `Antiga()` < `Recente()`.

> Dica: dentro de `EXPECT_EQ`, um `std::vector<Acao>{a, b, c}` com vírgulas
> quebra a macro. Envolva em parênteses: `EXPECT_EQ(r.acoes, (std::vector<Acao>{a, b, c}));`

### Teste 1 — Coluna 1: sem Backup.parm → Impossível
```cpp
TEST_F(BackupTest, Coluna01_SemBackupParm_Impossivel) {
  CriaArquivo(hd_, kArqX, "hd", Recente());          // parm NÃO é criado
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kImpossivel);
  EXPECT_TRUE(r.acoes.empty());
  EXPECT_FALSE(fs::exists(pendrive_ / kArqX));
}
```
GREEN: `if (!fs::exists(caminho_parm))` → devolver relatório `kImpossivel`.

### Teste 2 — Coluna 2: backup, só no HD → copia para o pendrive
```cpp
TEST_F(BackupTest, Coluna02_Backup_SoNoHd_CopiaParaPendrive) {
  EscreveParm({kArqX});
  CriaArquivo(hd_, kArqX, "versao HD", Recente());
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kSucesso);
  EXPECT_EQ(r.acoes, std::vector<Acao>{Acao::kHdParaPendrive});
  EXPECT_EQ(LeConteudo(pendrive_ / kArqX), "versao HD");
  EXPECT_EQ(fs::last_write_time(pendrive_ / kArqX),
            fs::last_write_time(hd_ / kArqX));       // data preservada
}
```
GREEN: ler o parm linha a linha, copiar com `fs::copy_file` + `fs::last_write_time`.

### Teste 3 — Coluna 3: backup, PenD mais antigo → copia
Arrange: parm `{kArqX}`; HD `"novo"` em `Recente()`; PenD `"velho"` em `Antiga()`.
Esperado: `kSucesso`, `{kHdParaPendrive}`, conteúdo do PenD == `"novo"`.
GREEN: introduzir comparação de datas. REFACTOR: extrair `ComparaDatas()`.

### Teste 4 — Coluna 4: backup, datas iguais → nada
Arrange: HD `"hd"` e PenD `"pen"`, ambos em `Recente()`.
Esperado: `kSucesso`, `{kNada}`, PenD continua `"pen"`.

### Teste 5 — Coluna 5: backup, PenD mais novo → Erro
Arrange: HD `"hd"` em `Antiga()`; PenD `"pen"` em `Recente()`.
Esperado: `kErro`, `{kErro}`, `mensagens_erro.size() == 1`, PenD continua `"pen"`.

### Teste 6 — Coluna 6: restauração, só no HD → Erro
Arrange: só HD. Esperado: `kErro`, `{kErro}`, HD inalterado, PenD sem o arquivo.

### Teste 7 — Coluna 7: restauração, PenD mais antigo → Erro
Arrange: HD `Recente()`, PenD `Antiga()`. Esperado: `kErro`, `{kErro}`, HD inalterado.

### Teste 8 — Coluna 8: restauração, datas iguais → nada
Esperado: `kSucesso`, `{kNada}`, HD inalterado.

### Teste 9 — Coluna 9: restauração, PenD mais novo → copia para o HD
Arrange: HD `"velho"` em `Antiga()`, PenD `"novo"` em `Recente()`.
Esperado: `kSucesso`, `{kPendriveParaHd}`, HD == `"novo"` com a data do PenD.
REFACTOR sugerido: extrair `DecideAcao()` (tabela pura) e uma função `Copia()`.

### Teste 10 — Coluna 10: backup, em nenhum lugar → Erro
Arrange: parm `{kArqX}`, nenhum arquivo. Esperado: `kErro`, `{kErro}`.

### Teste 11 — Coluna 11: backup, só no PenD → nada
Esperado: `kSucesso`, `{kNada}`, HD continua sem o arquivo.

### Teste 12 — Coluna 12: restauração, em nenhum lugar → Erro
Esperado: `kErro`, `{kErro}`.

### Teste 13 — Coluna 13: restauração, só no PenD → copia para o HD
Esperado: `kSucesso`, `{kPendriveParaHd}`, HD criado com conteúdo/data do PenD.

### Testes 14–16 — Repetição (slide 16: 0, 1 e n ≥ arrasto+1 iterações)
O laço sobre os ArqX tem **arrasto 1** (o `resultado` global só depende das iterações anteriores).
- **14 — Parm vazio (0 iterações):** `kSucesso`, `acoes` vazio.
- **15 — Linhas em branco são ignoradas:** parm `{"", kArqX, ""}` → `acoes.size() == 1`.
- **16 — Vários arquivos (n = 3) misturando colunas:** ex.: `a.txt` (col 2),
  `b.txt` (col 4), `c.txt` (col 5) → `{kHdParaPendrive, kNada, kErro}`,
  resultado `kErro`, e `a.txt` **foi copiado** mesmo havendo erro em `c.txt`.

## 6. Forma esperada após as refatorações (referência, NÃO colar antes dos testes)

```cpp
Acao DecideAcao(bool tem_parm, Operacao op, bool no_hd, bool no_pen,
                ComparacaoData data) {
  if (!tem_parm) return Acao::kImpossivel;                       // col 1
  if (op == Operacao::kBackup) {
    if (no_hd && !no_pen) return Acao::kHdParaPendrive;          // col 2
    if (no_hd && no_pen) {
      if (data == ComparacaoData::kPendriveMaisAntigo)
        return Acao::kHdParaPendrive;                            // col 3
      if (data == ComparacaoData::kIguais) return Acao::kNada;   // col 4
      return Acao::kErro;                                        // col 5
    }
    return no_pen ? Acao::kNada : Acao::kErro;                   // col 11 / 10
  }
  if (no_hd && !no_pen) return Acao::kErro;                      // col 6
  if (no_hd && no_pen) {
    if (data == ComparacaoData::kPendriveMaisAntigo) return Acao::kErro;  // 7
    if (data == ComparacaoData::kIguais) return Acao::kNada;               // 8
    return Acao::kPendriveParaHd;                                          // 9
  }
  return no_pen ? Acao::kPendriveParaHd : Acao::kErro;           // col 13 / 12
}
```
Funções auxiliares internas (namespace anônimo em `backup.cpp`), cada uma com
cabeçalho no estilo do slide 14 e assertivas:
- `std::vector<std::string> LeArquivoParm(const fs::path&)`
- `ComparacaoData ComparaDatas(const fs::path& hd, const fs::path& pen)`
- `void Copia(const fs::path& origem, const fs::path& destino)`: copia e preserva a data.
  Assertiva de saída: conteúdo e data iguais.

## 7. Ferramentas (todas pelo Makefile)

| Comando          | Ferramenta | Exigência do enunciado                               |
|------------------|------------|------------------------------------------------------|
| `make test`      | gtest      | framework de teste                                   |
| `make cpplint`   | cpplint    | estilo Google, desde o início                        |
| `make cppcheck`  | cppcheck   | `cppcheck --enable=warning .`                        |
| `make valgrind`  | valgrind   | verificador dinâmico, desde o início                 |
| `make coverage`  | gcov       | `-ftest-coverage -fprofile-arcs`, ≥ 80% por módulo   |
| `make doc`       | doxygen    | comentários Javadoc; framework excluído              |
| `make gdb`       | gdb        | depuração                                            |
| `make verifica`  | todas      | rodar antes de todo commit REFACTOR                  |
| `make zip`       | zip        | `231011650_Lucas.zip` com `.git` e `leiame.txt`      |

## 8. Pendências / dúvidas em aberto

- [ ] **Checklists de revisão C/C++** enviados pelo professor: colocar em `Docs/`
      e registrar a revisão (ex.: `Docs/revisao_checklist.md`).
- [ ] **"Cobertura usando expressões regulares"** (enunciado): o significado não
      está claro. Hoje o `make coverage` usa regex (`awk`/`grep '#####'`) sobre a
      saída do gcov. Confirmar com o professor/slides.
- [ ] **Evidência de uso do gdb**: salvar uma sessão (ex.: `Docs/sessao_gdb.txt`).
- [ ] Gerar a documentação Doxygen e conferir `doxygen/html/index.html`.
- [ ] `leiame.txt` final + `make zip`.
