# Фаззинг-таргеты для TON (libFuzzer).
# Подключение: положить папку ton-fuzz/ внутрь дерева исходников TON и добавить
#   add_subdirectory(ton-fuzz)
# в конец корневого CMakeLists.txt, затем собрать clang-ом с -DTON_FUZZ=ON.
#
# Каждый таргет компилируется и линкуется с -fsanitize=fuzzer. Санитайзеры
# памяти/UB включаются стандартными опциями TON: -DTON_USE_ASAN=ON -DTON_USE_UBSAN=ON.

option(TON_FUZZ "Build libFuzzer harnesses" OFF)

if (TON_FUZZ)
  set(FUZZ_FLAGS -fsanitize=fuzzer)

  function(add_fuzz name src)
    add_executable(${name} ${src})
    target_compile_options(${name} PRIVATE ${FUZZ_FLAGS})
    target_link_options(${name} PRIVATE ${FUZZ_FLAGS})
    # Подключаем заголовки и библиотеки ядра.
    target_include_directories(${name} PRIVATE
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/crypto
      ${CMAKE_SOURCE_DIR}/tdutils
      ${CMAKE_SOURCE_DIR}/tl
      ${CMAKE_SOURCE_DIR}/tl-utils
      ${CMAKE_SOURCE_DIR}/adnl
      ${CMAKE_BINARY_DIR}/tl)
  endfunction()

  # BoC-парсеры — нужен только ton_crypto.
  add_fuzz(fuzz_boc_deserialize ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_boc_deserialize.cpp)
  target_link_libraries(fuzz_boc_deserialize PRIVATE ton_crypto)

  add_fuzz(fuzz_boc_decompress ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_boc_decompress.cpp)
  target_link_libraries(fuzz_boc_decompress PRIVATE ton_crypto)

  # TVM-исполнение — ton_crypto (+ ton_block для конфигов).
  add_fuzz(fuzz_tvm_run ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_tvm_run.cpp)
  target_link_libraries(fuzz_tvm_run PRIVATE ton_crypto ton_block)

  # ADNL-пакет — нужны tl_api + adnllib + tdutils.
  add_fuzz(fuzz_adnl_packet ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_adnl_packet.cpp)
  target_link_libraries(fuzz_adnl_packet PRIVATE adnl tl_api tl-utils tdutils tdactor ton_crypto)
endif()
