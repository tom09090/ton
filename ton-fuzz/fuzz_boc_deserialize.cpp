// Fuzz harness: десериализация BoC (bag of cells) из недоверенных байтов.
// Цель: vm::std_boc_deserialize — основной парсер ячеек из сети/блоков.
// Сборка: clang с -fsanitize=fuzzer,address,undefined, линковка с ton_crypto.

#include "vm/boc.h"
#include "vm/cells.h"
#include "td/utils/Slice.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  td::Slice slice(reinterpret_cast<const char*>(data), size);
  // can_be_empty=true, allow_nonzero_level=true — максимально широкий вход
  auto res = vm::std_boc_deserialize(slice, true, true);
  if (res.is_ok()) {
    auto cell = res.move_as_ok();
    if (cell.not_null()) {
      // get_hash() форсирует обход дерева ячеек и финализацию — этого достаточно,
      // чтобы зацепить пост-парсинговые пути.
      cell->get_hash();
    }
  }
  return 0;
}
