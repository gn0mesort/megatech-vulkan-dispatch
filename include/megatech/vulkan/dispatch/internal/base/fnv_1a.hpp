/// @cond INTERNAL
/**
 * @file fnv_1a.hpp
 * @brief FNV-1a Hash Functions
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_INTERNAL_BASE_FNV_1A_HPP
#define MEGATECH_VULKAN_DISPATCH_INTERNAL_BASE_FNV_1A_HPP

#include <cinttypes>

#include <concepts>
#include <limits>

namespace megatech::vulkan::dispatch::internal::base {

  /**
   * @brief A generic compile-time FNV-1a hash function.
   * @tparam Type The storage type of the hash function. This **MAY** be much wider than the desired hash width but it
   *              **MUST NOT** be shorter. For example, `std::uint64_t` is acceptable for 32-bit and 64-bit
   *              FNV-1a fuctions. However, `std::uint32_t` is only acceptable for the 32-bit FNV-1a function.
   * @tparam Offset The FNV offset basis. This **SHOULD** be the matching offset basis for the desired function. If
   *                a different value is used, you'll get some other function.
   * @tparam Prime A prime integer of the form required by the FNV hash functions. This **SHOULD** be the matching
   *               prime for the desired function. If a different value is used, you'll get some other function.
   * @tparam MaxDigits The maximum number of radix digits in the result value. As a result of allowing wider storage
   *                   types for shorter hashes, hash values can have a large number of extraneous bits. `MaxDigits`
   *                   controls how many of those bits should be considered necessary. The remaining bits are cleared
   *                   to 0. This ensures that comparisons between hash values for the same hash function with
   *                   different storage types remain correct. For example:
   *                   @code{.cpp}
   *                     constexpr auto a = basic_fnv_1a_cstr<std::uint64_t, 0x811c9dc5, 0x1000193, 32>("Hello");
   *                     constexpr auto b = basic_fnv_1a_cstr<std::uint32_t, 0x811c9dc5, 0x1000193, 32>("Hello");
   *                     static_assert(a == static_cast<std::uint64_t>(b));
   *                   @endcode
   *                   **MUST NOT** generate a compiler error. This value **MUST** be the bit width of the desired
   *                   hash function. Using any other value will generate erroneous results.
   * @param str The str to hash. This **MUST** be a C-style NUL (``'\0'``) terminated byte string.
   * @return An N-bit FNV-1a hash value.
   * @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
   * @see http://www.isthe.com/chongo/tech/comp/fnv
   */
  template <std::unsigned_integral Type, Type Offset, Type Prime, Type MaxDigits = std::numeric_limits<Type>::digits>
  consteval Type basic_fnv_1a_cstr(const char* str) {
    static_assert(std::numeric_limits<Type>::radix == 2, "The storage type of the FNV-1a hash must be a binary type.");
    static_assert(MaxDigits <= std::numeric_limits<Type>::digits,
                 "The maximum number of radix digits in an FNV-1a hash cannot exceed the the width of its data type.");
    auto hash = static_cast<Type>(Offset);
    while (*str)
    {
      hash ^= *str;
      hash *= static_cast<Type>(Prime);
      ++str;
    }
    // Mask off extraneous digits to ensure that values are as expected.
    if constexpr (std::numeric_limits<Type>::digits > MaxDigits)
    {
      return hash & ((static_cast<Type>(1) << MaxDigits) - 1);
    }
    return hash;
  }

  /**
   * @brief Compute a 64-bit FNV-1a hash from a C-style string at compile-time.
   * @details Although the generic implementation of ::basic_fnv_1a_cstr is perfectly happy to generate hash values
   *          of other widths, I am not providing that functionality in this API. The Vulkan specification requires
   *          64-bit arithmetic already, and so I can't imagine a scenario where a shorter hash is useful.
   * @param str The str to hash. This **MUST** be a C-style NUL (``'\0'``) terminated byte string.
   * @return A 64-bit FNV-1a hash value.
   * @see https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#fundamentals-host-environment
   */
  consteval std::uint_least64_t fnv_1a_cstr(const char *const str) {
    return basic_fnv_1a_cstr<std::uint_least64_t, 0xcbf29ce484222325, 0x100000001b3, 64>(str);
  }

}

#endif
/// @endcond
