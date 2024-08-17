#include "common.hpp"

TEST_CASE("64-bit FNV-1a computations should return the expected results.", "[dispatch]") {
  constexpr auto hash = megatech::vulkan::dispatch::internal::base::fnv_1a_cstr("foobar");
  REQUIRE(hash == std::uint_least64_t{ 0x85944171f73967e8 });
}

TEST_CASE("The \"\" value must return the expected 64-bit offset basis.", "[dispatch]") {
  constexpr auto hash = megatech::vulkan::dispatch::internal::base::fnv_1a_cstr("");
  REQUIRE(hash == std::uint_least64_t{ 0xcbf29ce484222325 });
}

TEST_CASE("The storage width must not alter the hash value for numeric_limits<Type>::digits >= MaxDigits", "dispatch") {
  using megatech::vulkan::dispatch::internal::base::basic_fnv_1a_cstr;
  constexpr auto a = basic_fnv_1a_cstr<std::uint64_t, 0x811c9dc5, 0x1000193, 32>("foobar");
  constexpr auto b = basic_fnv_1a_cstr<std::uint32_t, 0x811c9dc5, 0x1000193, 32>("foobar");
  REQUIRE(a == std::uint64_t{ b });
}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
