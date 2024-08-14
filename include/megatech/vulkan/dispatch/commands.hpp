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

}

}

#endif
