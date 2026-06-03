// Fuzz harness: распаковка сжатых BoC (новый кастомный алгоритм компрессии).
// Цель: vm::boc_decompress — свежий сложный код (F-1/F-2 в ton-audit.md).
// Покрывает baseline LZ4 и improved-structure пути (без state -> ветка WithState
// корректно вернёт ошибку, остальные распакуются).
// Сборка: clang -fsanitize=fuzzer,address,undefined + ton_crypto.

#include "vm/boc-compression.h"
#include "vm/cells.h"
#include "td/utils/Slice.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  td::Slice slice(reinterpret_cast<const char*>(data), size);
  // max_decompressed_size = 4 MiB — ограничение, как в проде, чтобы не уйти в OOM.
  auto res = vm::boc_decompress(slice, 4 * 1024 * 1024, td::Ref<vm::Cell>());
  if (res.is_ok()) {
    auto roots = res.move_as_ok();
    for (auto& c : roots) {
      if (c.not_null()) {
        c->get_hash();
      }
    }
  }
  return 0;
}
