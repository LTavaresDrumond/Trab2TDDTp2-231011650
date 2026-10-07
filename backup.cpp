// Copyright 2026 Lucas Drumond - matricula 231011650
#include "backup.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace backup {

namespace {

/**
 * @brief Função: Copiar arquivo.
 *
 * Descrição:
 *   Copia a origem para o destino, sobrescrevendo-o e preservando a data.
 *
 * @param origem arquivo existente a copiar.
 * @param destino caminho do arquivo de destino.
 * @return Não se aplica.
 * @pre Assertiva de entrada:
 *      origem é um arquivo regular e o diretório do destino existe.
 * @post Assertiva de saída:
 *       destino tem o mesmo conteúdo e a mesma data da origem.
 */
void Copia(const fs::path& origem, const fs::path& destino) {
  assert(fs::is_regular_file(origem));
  assert(fs::is_directory(destino.parent_path()));

  const auto data_origem = fs::last_write_time(origem);
  fs::copy_file(origem, destino, fs::copy_options::overwrite_existing);
  fs::last_write_time(destino, data_origem);
}

}  // namespace

Acao DecideAcao(bool tem_parm, Operacao operacao,
                bool arq_no_hd, bool arq_no_pendrive,
                ComparacaoData data) {
  if (!tem_parm) {
    return Acao::kImpossivel;
  }
  if (operacao == Operacao::kBackup && arq_no_hd && !arq_no_pendrive) {
    return Acao::kHdParaPendrive;
  }
  if (operacao == Operacao::kBackup && arq_no_hd && arq_no_pendrive &&
      data == ComparacaoData::kPendriveMaisAntigo) {
    return Acao::kHdParaPendrive;
  }
  if (operacao == Operacao::kBackup && arq_no_hd && arq_no_pendrive &&
      data == ComparacaoData::kPendriveMaisNovo) {
    return Acao::kErro;
  }
  if (operacao == Operacao::kRestauracao && arq_no_hd && !arq_no_pendrive) {
    return Acao::kErro;
  }
  if (operacao == Operacao::kRestauracao && arq_no_hd && arq_no_pendrive &&
      data == ComparacaoData::kPendriveMaisAntigo) {
    return Acao::kErro;
  }
  return Acao::kNada;
}

Relatorio ExecutaBackup(const std::string& caminho_parm,
                        const std::string& dir_hd,
                        const std::string& dir_pendrive,
                        Operacao operacao) {
  assert(!caminho_parm.empty());
  assert(fs::is_directory(dir_hd));
  assert(fs::is_directory(dir_pendrive));
  assert(dir_hd != dir_pendrive);

  Relatorio relatorio;
  if (!fs::exists(caminho_parm)) {
    relatorio.resultado = Resultado::kImpossivel;
    return relatorio;
  }

  relatorio.resultado = Resultado::kSucesso;
  std::ifstream entrada(caminho_parm);
  std::string nome;
  while (std::getline(entrada, nome)) {
    if (nome.empty()) {
      continue;
    }

    const fs::path caminho_hd = fs::path(dir_hd) / nome;
    const fs::path caminho_pendrive = fs::path(dir_pendrive) / nome;
    const bool arq_no_hd = fs::exists(caminho_hd);
    const bool arq_no_pendrive = fs::exists(caminho_pendrive);

    if (operacao == Operacao::kBackup && arq_no_hd && !arq_no_pendrive) {
      Copia(caminho_hd, caminho_pendrive);
      relatorio.acoes.push_back(Acao::kHdParaPendrive);
      continue;
    }

    if (operacao == Operacao::kBackup && arq_no_hd && arq_no_pendrive) {
      const auto hd_time = fs::last_write_time(caminho_hd);
      const auto pen_time = fs::last_write_time(caminho_pendrive);
      if (pen_time < hd_time) {
        Copia(caminho_hd, caminho_pendrive);
        relatorio.acoes.push_back(Acao::kHdParaPendrive);
        continue;
      }
      if (pen_time > hd_time) {
        relatorio.acoes.push_back(Acao::kErro);
        relatorio.mensagens_erro.push_back("Erro: pendrive mais novo");
        relatorio.resultado = Resultado::kErro;
        continue;
      }
    }

    if (!arq_no_hd && !arq_no_pendrive) {
      relatorio.acoes.push_back(Acao::kErro);
      relatorio.mensagens_erro.push_back("Erro: arquivo ausente");
      relatorio.resultado = Resultado::kErro;
      continue;
    }

    if (operacao == Operacao::kRestauracao && arq_no_hd && !arq_no_pendrive) {
      relatorio.acoes.push_back(Acao::kErro);
      relatorio.mensagens_erro.push_back(
          "Erro: restauração sem arquivo no pendrive");
      relatorio.resultado = Resultado::kErro;
      continue;
    }

    if (operacao == Operacao::kRestauracao && arq_no_hd && arq_no_pendrive) {
      const auto hd_time = fs::last_write_time(caminho_hd);
      const auto pen_time = fs::last_write_time(caminho_pendrive);
      if (pen_time < hd_time) {
        relatorio.acoes.push_back(Acao::kErro);
        relatorio.mensagens_erro.push_back("Erro: pendrive mais antigo");
        relatorio.resultado = Resultado::kErro;
        continue;
      }
      if (pen_time > hd_time) {
        Copia(caminho_pendrive, caminho_hd);
        relatorio.acoes.push_back(Acao::kPendriveParaHd);
        continue;
      }
    }

    relatorio.acoes.push_back(Acao::kNada);
  }

  return relatorio;
}

}  // namespace backup
