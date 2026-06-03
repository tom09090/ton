#!/usr/bin/env bash
# Сборка TON с санитайзерами + libFuzzer и запуск фаззеров.
# Требования: clang/clang++ (>=14), cmake, ninja, и системные зависимости TON
# (OpenSSL, zlib, libsodium, lz4, и т.д. — см. README репозитория).
#
# Запускать из КОРНЯ исходников TON, предварительно скопировав туда папку ton-fuzz/
# и добавив в конец корневого CMakeLists.txt строку:  add_subdirectory(ton-fuzz)
set -euo pipefail

ROOT="$(pwd)"
BUILD="${ROOT}/build-fuzz"

export CC=clang
export CXX=clang++

cmake -B "${BUILD}" -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DTON_USE_ASAN=ON \
  -DTON_USE_UBSAN=ON \
  -DTON_FUZZ=ON \
  "${ROOT}"

# Собираем только фаззеры (и их зависимости подтянутся автоматически).
cmake --build "${BUILD}" --target \
  fuzz_boc_deserialize fuzz_boc_decompress fuzz_tvm_run fuzz_adnl_packet -j"$(nproc)"

mkdir -p "${ROOT}/fuzz-corpus/boc" "${ROOT}/fuzz-corpus/boc_z" \
         "${ROOT}/fuzz-corpus/tvm" "${ROOT}/fuzz-corpus/adnl"

echo
echo "== Сборка готова. Примеры запуска (Ctrl-C для остановки): =="
echo "  ${BUILD}/ton-fuzz/fuzz_boc_deserialize -max_len=65536 -timeout=10 fuzz-corpus/boc"
echo "  ${BUILD}/ton-fuzz/fuzz_boc_decompress  -max_len=65536 -timeout=10 fuzz-corpus/boc_z"
echo "  ${BUILD}/ton-fuzz/fuzz_tvm_run         -max_len=8192  -timeout=10 fuzz-corpus/tvm"
echo "  ${BUILD}/ton-fuzz/fuzz_adnl_packet     -max_len=8192  -timeout=10 fuzz-corpus/adnl"
echo
echo "Совет: для seed-корпуса подложите реальные .boc файлы (например из test/)"
echo "в fuzz-corpus/boc — фаззер быстрее найдёт интересные пути."
