/**
 * @file commands.hpp
 * @brief Vulkan Command Enumerations
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_COMMANDS_HPP
#define MEGATECH_VULKAN_DISPATCH_COMMANDS_HPP
#include <cstddef>
#include <cinttypes>

#include "defs.hpp"
#include "error.hpp"

#include "internal/base/fnv_1a.hpp"

namespace megatech::vulkan::dispatch {

namespace global {

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) name,
/// @endcond
  /**
   * @brief An enumeration of globally available Vulkan command names.
   */
  enum class command : std::size_t {
    MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
  };
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV-1a hash value into a Vulkan ::command.
   * @details This function is equivalent, largely to the ::table::get(const std::uint_least64_t) const method.
   *          The primary exception is that this function can throw a dispatch::error.
   *
   *          The primary utility of this function is to store ::command values for hashes computed at run-time.
   *          Generally, storing a @ref command and retrieving the corresponding function pointer is much faster than
   *          computing hash values multiple times from strings.
   *
   *          In a pinch, you could also use this to pre-resolve all of your hash values to ::command values at
   *          run-time. This would allow you to skip checking for null returns on
   *          ::table::get(const std::uint_least64_t) const.
   * @param hash An FNV-1a hash value that maps to a Vulkan ::command. Hash values are mapped based on the string
   *             value of the ::command's name. For example:
   *             @code{.cpp}
   *               to_command(fnv_1a("vkGetInstanceProcAddr")); // == command::vkGetInstanceProcAddr
   *             @endcode
   *
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   */
  constexpr command to_command(const std::uint_least64_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
    default:
      throw dispatch::error{ "The input hash value does not map to any global Vulkan command." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND
/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case command::name: return dispatch::internal::base::fnv_1a_cstr(#name);
/// @endcond
  /**
   * @brief Convert a ::command to an FNV-1a hash value.
   * @details This function is the inverse of ::to_command(const std::uint_least64_t). At run-time this is a simple
   *          mapping from ::command values to hashes. The hash values are not actually recomputed. If you pass an
   *          ill-formed command value this function will generate a run-time error.
   * @param cmd A ::command that maps to a known FNV-1a hash value. Hash values are mapped based on the string value
   *            of the ::command's name. For example:
   *            @code{.cpp}
   *              to_hash(command::vkGetInstanceProcAddr); // == fnv_1a("vkGetInstanceProcAddr")
   *            @endcode
   * @throw dispatch::error If the input command is ill-formed.
   */
  constexpr std::uint_least64_t to_hash(const command cmd) {
    switch (cmd)
    {
    MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
    default:
      throw dispatch::error{ "The input command is outside the valid range of possible global commands." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

}

namespace instance {

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) name,
/// @endcond
  /**
   * @brief An enumeration of instance-level Vulkan command names.
   */
  enum class command : std::size_t {
    MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
  };
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV-1a hash value into a Vulkan ::command.
   * @param hash An FNV-1a hash value that maps to a Vulkan ::command. Hash values are mapped based on the string
   *             value of the ::command's name.
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   * @see ::global::to_command
   */
  constexpr command to_command(const std::uint_least64_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
    default:
      throw dispatch::error{ "The input hash value does not map to any instance-level Vulkan command." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND
/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case command::name: return dispatch::internal::base::fnv_1a_cstr(#name);
/// @endcond
  /**
   * @brief Convert a ::command to an FNV-1a hash value.
   * @param cmd A ::command that maps to a known FNV-1a hash value. Hash values are mapped based on the string value
   *            of the ::command's name. For example:
   *            @code{.cpp}
   *              to_hash(command::vkGetInstanceProcAddr); // == fnv_1a("vkGetInstanceProcAddr")
   *            @endcode
   * @throw dispatch::error If the input command is ill-formed.
   * @see ::global::to_hash
   */
  constexpr std::uint_least64_t to_hash(const command cmd) {
    switch (cmd)
    {
    MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
    default:
      throw dispatch::error{ "The input command is outside the valid range of possible instance commands." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

}

namespace device {

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) name,
/// @endcond
  /**
   * @brief An enumeration of device-level Vulkan command names.
   */
  enum class command : std::size_t {
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
  };
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv_1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV-1a hash value into a Vulkan ::command.
   * @param hash An FNV-1a hash value that maps to a Vulkan ::command. Hash values are mapped based on the string
   *             value of the ::command's name.
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   * @see ::global::to_command
   */
  constexpr command to_command(const std::uint_least64_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    default:
      throw dispatch::error{ "The input hash value does not map to any device-level Vulkan command." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

/// @cond
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case command::name: return dispatch::internal::base::fnv_1a_cstr(#name);
/// @endcond
  /**
   * @brief Convert a ::command to an FNV-1a hash value.
   * @param cmd A ::command that maps to a known FNV-1a hash value. Hash values are mapped based on the string value
   *            of the ::command's name. For example:
   *            @code{.cpp}
   *              to_hash(command::vkGetInstanceProcAddr); // == fnv_1a("vkGetInstanceProcAddr")
   *            @endcode
   * @throw dispatch::error If the input command is ill-formed.
   * @see ::global::to_hash
   */
  constexpr std::uint_least64_t to_hash(const command cmd) {
    switch (cmd)
    {
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    default:
      throw dispatch::error{ "The input command is outside the valid range of possible device commands." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

}

}


#endif
