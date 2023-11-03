/// ###########################################################################
/// @copyright Copyright (c) 2023, Pedro Oliveira. All rights reserved.
/// ###########################################################################
#include "ccia/ccia.h"

namespace ccia {

  int better_main([[maybe_unused]] std::span<const std::string_view> args) noexcept {
    for (const auto& arg : args) {
      std::cout << arg << '\n';
    }
    std::cout << std::endl;

    return 0;
  }
} // namespace ccia
