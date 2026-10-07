// Copyright 2026 Lucas Drumond - matricula 231011650
/**
 * @file testa_backup.cpp
 * @brief Testes do módulo de backup (Google Test).
 *
 * Um teste por coluna da tabela de decisão (slides 23-26 de caixa fechada),
 * mais testes de repetição (0, 1 e n arquivos no Backup.parm) conforme o
 * slide 16 de caixa aberta. Roteiro em Docs/PLANO_TDD.md.
 *
 * Cada teste trabalha num diretório temporário próprio com a estrutura:
 * @verbatim
 *   <tmp>/tp2_backup_<NomeDoTeste>/
 *     Backup.parm
 *     hd/
 *     pendrive/
 * @endverbatim
 */
#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "backup.hpp"

namespace fs = std::filesystem;

namespace {

using backup::Acao;
using backup::Operacao;
using backup::Relatorio;
using backup::Resultado;

/** @brief Nome do arquivo ArqX usado nos testes de coluna. */
constexpr char kArqX[] = "arqX.txt";

/**
 * @brief Fixture: cria e remove o ambiente HD/pendrive de cada teste.
 */
class BackupTest : public ::testing::Test {
 protected:
  /** @brief Cria os diretórios hd/ e pendrive/ vazios (sem Backup.parm). */
  void SetUp() override {
    const ::testing::TestInfo* info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    raiz_ = fs::temp_directory_path() /
            (std::string("tp2_backup_") + info->name());
    fs::remove_all(raiz_);
    hd_ = raiz_ / "hd";
    pendrive_ = raiz_ / "pendrive";
    parm_ = raiz_ / "Backup.parm";
    fs::create_directories(hd_);
    fs::create_directories(pendrive_);
    agora_ = fs::file_time_type::clock::now();
  }

  /** @brief Remove todo o ambiente do teste. */
  void TearDown() override { fs::remove_all(raiz_); }

  /** @brief Data "antiga" (2 horas atrás) para arquivos de teste. */
  fs::file_time_type Antiga() const { return agora_ - std::chrono::hours(2); }

  /** @brief Data "recente" (1 hora atrás) para arquivos de teste. */
  fs::file_time_type Recente() const { return agora_ - std::chrono::hours(1); }

  /**
   * @brief Cria o Backup.parm listando os arquivos dados.
   * @param nomes nomes dos arquivos, um por linha.
   */
  void EscreveParm(const std::vector<std::string>& nomes) const {
    std::ofstream saida(parm_);
    for (const std::string& nome : nomes) {
      saida << nome << '\n';
    }
  }

  /**
   * @brief Cria um arquivo com conteúdo e data de modificação definidos.
   * @param dir      diretório (hd_ ou pendrive_).
   * @param nome     nome do arquivo.
   * @param conteudo texto gravado no arquivo.
   * @param data     data de modificação a atribuir.
   */
  void CriaArquivo(const fs::path& dir, const std::string& nome,
                   const std::string& conteudo,
                   fs::file_time_type data) const {
    {
      std::ofstream saida(dir / nome);
      saida << conteudo;
    }
    fs::last_write_time(dir / nome, data);
  }

  /**
   * @brief Lê todo o conteúdo de um arquivo.
   * @param caminho arquivo a ler.
   * @return conteúdo do arquivo.
   */
  static std::string LeConteudo(const fs::path& caminho) {
    std::ifstream entrada(caminho);
    std::stringstream buffer;
    buffer << entrada.rdbuf();
    return buffer.str();
  }

  /**
   * @brief Chama ExecutaBackup() com o ambiente deste teste.
   * @param operacao backup ou restauração.
   * @return relatório da execução.
   */
  Relatorio Executa(Operacao operacao) const {
    return backup::ExecutaBackup(parm_.string(), hd_.string(),
                                 pendrive_.string(), operacao);
  }

  fs::path raiz_;              /**< Diretório temporário do teste. */
  fs::path hd_;                /**< Diretório que simula o HD. */
  fs::path pendrive_;          /**< Diretório que simula o pendrive. */
  fs::path parm_;              /**< Caminho do Backup.parm. */
  fs::file_time_type agora_;   /**< Instante de referência das datas. */
};

TEST_F(BackupTest, Coluna01_SemBackupParm_Impossivel) {
  CriaArquivo(hd_, kArqX, "hd", Recente());
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kImpossivel);
  EXPECT_TRUE(r.acoes.empty());
  EXPECT_FALSE(fs::exists(pendrive_ / kArqX));
}

TEST_F(BackupTest, Coluna02_Backup_SoNoHd_CopiaParaPendrive) {
  EscreveParm({kArqX});
  CriaArquivo(hd_, kArqX, "versao HD", Recente());
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kSucesso);
  EXPECT_EQ(r.acoes, std::vector<Acao>{Acao::kHdParaPendrive});
  EXPECT_EQ(LeConteudo(pendrive_ / kArqX), "versao HD");
  EXPECT_EQ(fs::last_write_time(pendrive_ / kArqX),
            fs::last_write_time(hd_ / kArqX));
}

TEST_F(BackupTest, Coluna03_Backup_PenDriveMaisAntigo_Copia) {
  EscreveParm({kArqX});
  CriaArquivo(hd_, kArqX, "novo", Recente());
  CriaArquivo(pendrive_, kArqX, "velho", Antiga());
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kSucesso);
  EXPECT_EQ(r.acoes, std::vector<Acao>{Acao::kHdParaPendrive});
  EXPECT_EQ(LeConteudo(pendrive_ / kArqX), "novo");
}

TEST_F(BackupTest, Coluna04_Backup_DatasIguais_Nada) {
  EscreveParm({kArqX});
  CriaArquivo(hd_, kArqX, "hd", Recente());
  CriaArquivo(pendrive_, kArqX, "pen", Recente());
  Relatorio r = Executa(Operacao::kBackup);
  EXPECT_EQ(r.resultado, Resultado::kSucesso);
  EXPECT_EQ(r.acoes, std::vector<Acao>{Acao::kNada});
  EXPECT_EQ(LeConteudo(pendrive_ / kArqX), "pen");
}

}  // namespace
