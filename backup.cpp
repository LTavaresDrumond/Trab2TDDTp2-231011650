// Copyright 2026 Lucas Drumond - matricula 231011650
/**
 * @file backup.cpp
 * @brief Implementação do módulo de backup e restauração.
 *
 * ESTADO ATUAL: apenas stubs. A implementação será construída de forma
 * incremental, um ciclo TDD (RED -> GREEN -> REFACTOR) por coluna da tabela
 * de decisão. Roteiro em Docs/PLANO_TDD.md.
 */
#include "backup.hpp"

#include <cassert>
#include <filesystem>
#include <string>

namespace backup {

Acao DecideAcao(bool tem_parm, Operacao /*operacao*/,
                bool /*arq_no_hd*/, bool /*arq_no_pendrive*/,
                ComparacaoData /*data*/) {
  if (!tem_parm) {
    return Acao::kImpossivel;
  }
  return Acao::kNada;
}

Relatorio ExecutaBackup(const std::string& caminho_parm,
                        const std::string& dir_hd,
                        const std::string& dir_pendrive,
                        Operacao /*operacao*/) {
  // Assertivas de entrada
  assert(!caminho_parm.empty());
  assert(std::filesystem::is_directory(dir_hd));
  assert(std::filesystem::is_directory(dir_pendrive));
  assert(dir_hd != dir_pendrive);

  Relatorio relatorio;
  if (!std::filesystem::exists(caminho_parm)) {
    relatorio.resultado = Resultado::kImpossivel;
    return relatorio;
  }

  relatorio.resultado = Resultado::kSucesso;
  return relatorio;
}

}  // namespace backup
