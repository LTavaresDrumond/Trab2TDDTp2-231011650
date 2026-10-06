// Copyright 2026 Lucas Drumond - matricula 231011650
/**
 * @file backup.hpp
 * @brief Interface do módulo de backup e restauração (TP2 - Trabalho 2).
 *
 * O sistema copia os arquivos listados em "Backup.parm" do HD para o
 * pendrive (backup) ou do pendrive para o HD (restauração). A ação aplicada
 * a cada arquivo ArqX é definida pela tabela de decisão dos slides 23-26
 * da aula de testes de caixa fechada (transcrita em Docs/PLANO_TDD.md).
 *
 * O "HD" e o "pendrive" são representados por diretórios quaisquer.
 */
#ifndef BACKUP_HPP_
#define BACKUP_HPP_

#include <string>
#include <vector>

/** @brief Espaço de nomes do módulo de backup. */
namespace backup {

/** @brief Operação solicitada (condição "Faz backup" da tabela). */
enum class Operacao {
  kBackup,      /**< Faz backup = V: copiar do HD para o pendrive. */
  kRestauracao  /**< Faz backup = F: copiar do pendrive para o HD. */
};

/**
 * @brief Comparação entre Data(Pen-drive, ArqX) e Data(HD, ArqX).
 *
 * Corresponde às três últimas condições da tabela de decisão.
 */
enum class ComparacaoData {
  kNaoSeAplica,         /**< ArqX não existe no HD ou no pendrive. */
  kPendriveMaisAntigo,  /**< Data(PenD, ArqX) <  Data(HD, ArqX). */
  kIguais,              /**< Data(PenD, ArqX) == Data(HD, ArqX). */
  kPendriveMaisNovo     /**< Data(PenD, ArqX) >  Data(HD, ArqX). */
};

/** @brief Ações da tabela de decisão (slide 25). */
enum class Acao {
  kHdParaPendrive,  /**< Salvar: copiar ArqX do HD para o pendrive. */
  kPendriveParaHd,  /**< Restaurar: copiar ArqX do pendrive para o HD. */
  kExcluir,         /**< Excluir ArqX do pendrive (nenhuma coluna usa). */
  kNada,            /**< Faz nada. */
  kErro,            /**< Erro: ArqX não é tratado e é gerado um idErro. */
  kImpossivel       /**< Combinação ilegal: cancela a execução. */
};

/** @brief Resultado global de uma execução de ExecutaBackup(). */
enum class Resultado {
  kSucesso,    /**< Nenhum ArqX caiu em coluna de Erro. */
  kErro,       /**< Ao menos um ArqX caiu em coluna de Erro. */
  kImpossivel  /**< Backup.parm não existe: execução cancelada. */
};

/** @brief Relatório devolvido por ExecutaBackup(). */
struct Relatorio {
  /** Resultado global da execução. */
  Resultado resultado = Resultado::kImpossivel;
  /** Ação aplicada a cada ArqX, na mesma ordem do Backup.parm. */
  std::vector<Acao> acoes;
  /** Mensagens (idErro) condizentes com cada erro encontrado. */
  std::vector<std::string> mensagens_erro;
};

/**
 * @brief Função: Decidir ação.
 *
 * Descrição:
 *   Implementa a tabela de decisão: dadas as condições de um arquivo ArqX,
 *   devolve a ação que deve ser aplicada a ele. Função pura, sem acesso ao
 *   sistema de arquivos.
 *
 * @param tem_parm        V se o arquivo Backup.parm existe.
 * @param operacao        backup (HD -> pendrive) ou restauração.
 * @param arq_no_hd       V se ArqX pertence ao HD.
 * @param arq_no_pendrive V se ArqX pertence ao pendrive.
 * @param data            comparação das datas; deve ser kNaoSeAplica se, e
 *                        somente se, ArqX faltar no HD ou no pendrive.
 *
 * @return A ação da coluna da tabela que corresponde às condições.
 *
 * @pre Assertiva de entrada:
 *      (arq_no_hd && arq_no_pendrive) == (data != ComparacaoData::kNaoSeAplica)
 *      quando tem_parm é V.
 * @post Assertiva de saída:
 *      !tem_parm  =>  retorno == Acao::kImpossivel;
 *      retorno nunca é Acao::kExcluir (nenhuma coluna marca Excluir).
 */
Acao DecideAcao(bool tem_parm, Operacao operacao, bool arq_no_hd,
                bool arq_no_pendrive, ComparacaoData data);

/**
 * @brief Função: Executar backup ou restauração.
 *
 * Descrição:
 *   Lê os nomes de arquivo em Backup.parm (um por linha, relativos aos
 *   diretórios do HD e do pendrive; linhas em branco são ignoradas) e
 *   aplica a cada ArqX a ação dada por DecideAcao(). Ao copiar, a data de
 *   modificação da origem é preservada no destino.
 *
 * @param caminho_parm  caminho do arquivo Backup.parm (pode não existir).
 * @param dir_hd        diretório que representa o HD.
 * @param dir_pendrive  diretório que representa o pendrive.
 * @param operacao      Operacao::kBackup ou Operacao::kRestauracao.
 *
 * @return Relatorio com o resultado global, a ação aplicada a cada ArqX e
 *         as mensagens de erro (idErro).
 *
 * @pre Assertiva de entrada:
 *      !caminho_parm.empty();
 *      dir_hd e dir_pendrive são diretórios existentes;
 *      dir_hd != dir_pendrive.
 * @post Assertiva de saída:
 *      Backup.parm inexistente  =>  resultado == kImpossivel e acoes vazio;
 *      Backup.parm existente    =>  acoes.size() == número de ArqX listados;
 *      resultado == kErro  <=>  alguma ação == kErro;
 *      mensagens_erro.size() == número de ações kErro;
 *      todo ArqX copiado tem, no HD e no pendrive, o mesmo conteúdo e a
 *      mesma data de modificação.
 */
Relatorio ExecutaBackup(const std::string& caminho_parm,
                        const std::string& dir_hd,
                        const std::string& dir_pendrive,
                        Operacao operacao);

}  // namespace backup

#endif  // BACKUP_HPP_
