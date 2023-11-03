/// ###########################################################################
/// @copyright Copyright (c) 2023, Pedro Oliveira. All rights reserved.
/// ###########################################################################
#ifndef CCIA_H
#define CCIA_H

#pragma once

namespace ccia {

  class Application;

  /****************************************************************************
   * @brief A better main that allows for a more flexible interface with a span of string_views.
   *
   * @param args
   * @return int
   ***************************************************************************/
  [[nodiscard]] int better_main([[maybe_unused]] std::span<const std::string_view> args) noexcept;

} // namespace ccia

#endif // CCIA_H
