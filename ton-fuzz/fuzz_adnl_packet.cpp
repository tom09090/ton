// Fuzz harness: разбор ADNL-пакета из недоверенных байтов.
// Цель: TL-десериализация adnl.packetContents + AdnlPacket::create + run_basic_checks.
// Это первый рубеж обработки сетевых датаграмм (исторически зона DoS).
// Сборка: clang -fsanitize=fuzzer,address,undefined + tl_api, adnllib, tdutils, ton_crypto.

#include "auto/tl/ton_api.h"
#include "adnl/adnl-packet.h"
#include "tl-utils/tl-utils.hpp"
#include "td/utils/buffer.h"

using namespace ton;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  td::BufferSlice buf(reinterpret_cast<const char*>(data), size);
  // bare=true: пробуем как «голую» TL-структуру (как в канале/пакете).
  auto R = fetch_tl_object<ton_api::adnl_packetContents>(std::move(buf), true);
  if (R.is_ok()) {
    auto packet = adnl::AdnlPacket::create(R.move_as_ok());
    (void)packet;
  }
  return 0;
}
