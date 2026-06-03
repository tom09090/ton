#include "vm/boc.h"
#include "vm/cells.h"
#include "td/utils/Slice.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  td::Slice slice(reinterpret_cast<const char*>(data), size);
  auto res = vm::std_boc_deserialize(slice, true, true);
  if (res.is_ok()) {
    auto cell = res.move_as_ok();
    if (cell.not_null()) {
      cell->get_hash();
    }
  }
  return 0;
}
