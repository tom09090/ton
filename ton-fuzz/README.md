# TON fuzzing harnesses

Набор для динамической проверки TON на уязвимости — то, что нельзя поймать чтением и
статическим анализатором (переполнения буфера/стека, use-after-free, UB, недетерминизм VM).
Покрывает зоны риска из `../ton-audit.md` (F-1, F-3, F-5).

## Что внутри

| Файл | Цель | Что ищет |
|---|---|---|
| `fuzz_boc_deserialize.cpp` | `vm::std_boc_deserialize` | баги парсера ячеек (основной недоверенный вход) |
| `fuzz_boc_decompress.cpp` | `vm::boc_decompress` | баги нового кода компрессии BoC (F-1, F-2) |
| `fuzz_tvm_run.cpp` | `vm::VmState::run` | крахи/UB при исполнении произвольного байткода (F-3) |
| `fuzz_adnl_packet.cpp` | разбор `adnl.packetContents` | DoS на первом рубеже сетевого парсинга |

RaptorQ-декодер (`tdfec`, F-5) тоже стоит зафаззить, но его харнес требует сборки
валидного набора символов; добавьте по образцу `td::fec::FecType::create_decoder` →
`add_symbol` → `try_decode`.

## Как запустить

1. Скопируйте папку `ton-fuzz/` в корень исходников TON.
2. В конец корневого `CMakeLists.txt` добавьте: `add_subdirectory(ton-fuzz)`
3. Соберите и запустите:
   ```
   cp <путь>/ton-fuzz/build-and-run.sh .
   chmod +x build-and-run.sh
   ./build-and-run.sh
   ```
   Скрипт включает `-DTON_USE_ASAN=ON -DTON_USE_UBSAN=ON -DTON_FUZZ=ON` и собирает clang-ом.

## Как читать результат

- Фаззер крутится, пока не найдёт вход, вызывающий креш/санитайзер-ошибку. Тогда он
  сохранит `crash-*` файл и выведет стек. Это и есть подтверждённый баг.
- ASan ловит выходы за границы и use-after-free; UBSan — переполнения/UB; сам libFuzzer —
  зависания (timeout) и OOM.
- Долгий прогон без крешей = хороший признак устойчивости (но не доказательство отсутствия багов).

## Замечания
- Сигнатуры (`VmState`, `fetch_tl_object`) могли меняться между версиями — при ошибке
  компиляции сверьте с заголовками (`crypto/vm/vm.h`, `tl-utils/tl-utils.hpp`) и поправьте.
- Имена библиотек-таргетов (`ton_crypto`, `ton_block`, `adnl`, `tl_api`, `tl-utils`, `tdactor`)
  сверены с `crypto/CMakeLists.txt`, `adnl/CMakeLists.txt`, `tl/CMakeLists.txt`.
