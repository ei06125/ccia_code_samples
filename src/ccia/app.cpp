#include "ccia/ccia.h"

int main(const int argc, char const* const* const argv) {
  std::vector<std::string_view> args(argv, std::next(argv, static_cast<std::ptrdiff_t>(argc)));
  return ccia::better_main(args);
}
