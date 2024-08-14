/**
 * @file commands.hpp
 * @brief Vulkan Command Enumerations
 * @author Alexander Rothman <gnomesort@megate.ch>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#ifndef MEGATECH_VULKAN_DISPATCH_COMMANDS_HPP
#define MEGATECH_VULKAN_DISPATCH_COMMANDS_HPP
#include <cstddef>

#include "defs.hpp"

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
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV1a hash value into a Vulkan @ref command.
   * @details This function is equivalent, largely to the @ref ::table::get(const std::size_t) const method.
   *          The primary exception is that this function can throw a @ref dispatch::error.
   *
   *          The primary utility of this function is to store @ref command values for hashes computed at run-time.
   *          Generally, storing a @ref command and retrieving the corresponding function pointer is much faster than
   *          computing hash values multiple times from strings.
   *
   *          In a pinch, you could also use this to pre-resolve all of your hash values to @ref command values at
   *          run-time. This would allow you to skip checking for null returns on
   *          @ref ::table::get(const std::size_t) const.
   * @param hash An FNV1a hash value that maps to a Vulkan @ref command. Hash values are mapped based on the string
   *             value of the command's name. For example:
   *
   *             @code{.cpp}
   *               to_command(fnv1a("vkGetInstanceProcAddr")); // == command::vkGetInstanceProcAddr
   *             @endcode
   *
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   */
  constexpr command to_command(const std::size_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
    default:
      throw error{ "The input hash value does not map to any global Vulkan command." };
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
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV1a hash value into a Vulkan command.
   * @param hash An FNV1a hash value that maps to a Vulkan @ref command. Hash values are mapped based on the string
   *             value of the command's name.
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   * @see ::global::to_command
   */
  constexpr command to_command(const std::size_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
    default:
      throw error{ "The input hash value does not map to any instance-level Vulkan command." };
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
#define MEGATECH_VULKAN_DISPATCH_COMMAND(name) case dispatch::internal::base::fnv1a_cstr(#name): return command::name;
/// @endcond
  /**
   * @brief Convert an FNV1a hash value into a Vulkan command.
   * @param hash An FNV1a hash value that maps to a Vulkan @ref command. Hash values are mapped based on the string
   *             value of the command's name.
   * @throws dispatch::error If the input hash doesn't have a compile-time mapping.
   * @see ::global::to_command
   */
  constexpr command to_command(const std::size_t hash) {
    switch (hash)
    {
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    default:
      throw error{ "The input hash value does not map to any device-level Vulkan command." };
    }
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

}

}

#endif
