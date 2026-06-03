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
    auto stack = td::make_ref<vm::Stack>();
    long long gas_limit = 100000;
    int global_version = 9;
    vm::VmState vm_state{code_cell, global_version, std::move(stack), vm::GasLimits{gas_limit, gas_limit}, 0};
    vm_state.run();
  } catch (...) {
  }
  return 0;
}
