// Fuzz harness: исполнение произвольного TVM-байткода.
// Цель: vm::VmState::run — детерминизм/устойчивость VM на враждебном коде (F-3).
// Вход трактуется как BoC с кодом; при невалидном BoC просто выходим.
// Лимит газа жёсткий, чтобы фаззер не зависал.
// Сборка: clang -fsanitize=fuzzer,address,undefined + ton_crypto, ton_block.
//
// ПРИМЕЧАНИЕ: API VmState между версиями менялся. Если не компилируется —
// сверьте сигнатуру конструктора в crypto/vm/vm.h и подправьте вызов.

#include "vm/boc.h"
#include "vm/cells.h"
#include "vm/cellslice.h"
#include "vm/vm.h"
#include "vm/stack.hpp"
#include "td/utils/Slice.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size < 2) {
    return 0;
  }
  auto res = vm::std_boc_deserialize(td::Slice(reinterpret_cast<const char*>(data), size), true, true);
  if (res.is_error()) {
    return 0;
  }
  auto code_cell = res.move_as_ok();
  if (code_cell.is_null()) {
    return 0;
  }
  try {
    vm::CellSlice cs(vm::NoVm(), code_cell);
    auto stack = td::make_ref<vm::Stack>();
    long long gas_limit = 100000;       // жёсткий лимит газа
    int global_version = 9;             // актуальная версия глобальных правил TVM
    vm::VmState vm_state{code_cell, global_version, std::move(stack), vm::GasLimits{gas_limit, gas_limit}, /*flags=*/0};
    vm_state.run();
  } catch (...) {
    // VmError/любые исключения VM — штатная обработка, не падение процесса.
  }
  return 0;
}
