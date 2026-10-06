#!/usr/bin/env bash
# Instala todas as ferramentas exigidas pelo Trabalho 2 de TP2 (CIC0198).
# Executado automaticamente pelo devcontainer; pode ser rodado à mão:
#   bash scripts/setup_ambiente.sh
set -euo pipefail

SUDO=""
if [ "$(id -u)" -ne 0 ]; then
  SUDO="sudo"
fi

export DEBIAN_FRONTEND=noninteractive
$SUDO apt-get update
$SUDO apt-get install -y --no-install-recommends \
  build-essential g++ make cmake gdb valgrind cppcheck \
  doxygen graphviz libgtest-dev zip python3-pip

# cpplint (verificador de estilo do Google)
$SUDO python3 -m pip install cpplint \
  || $SUDO python3 -m pip install --break-system-packages cpplint

# Algumas versões do pacote libgtest-dev trazem apenas o código-fonte.
# Nesse caso compilamos a biblioteca.
if ! find /usr/lib /usr/local/lib -name 'libgtest.a' 2>/dev/null | grep -q .; then
  echo ">> Compilando googletest a partir de /usr/src/googletest"
  cd /usr/src/googletest
  $SUDO cmake -B build .
  $SUDO cmake --build build
  $SUDO cp build/lib/*.a /usr/local/lib/
  cd -
fi

echo ""
echo ">> Versões instaladas:"
g++ --version | head -1
make --version | head -1
gdb --version | head -1
valgrind --version
cppcheck --version
cpplint --version | head -2
doxygen --version
gcov --version | head -1
echo ">> Ambiente pronto. Rode: make verifica"
