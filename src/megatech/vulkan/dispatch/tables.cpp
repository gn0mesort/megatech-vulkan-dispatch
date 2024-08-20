/**
 * @file table.cpp
 * @brief Vulkan Dispatch Tables
 * @author Alexander Rothman <[gnomesort@megate.ch](mailto:gnomesort@megate.ch)>
 * @date 2024
 * @copyright AGPL-3.0-or-later
 */
#include "megatech/vulkan/dispatch/tables.hpp"

#include "megatech/vulkan/dispatch/error.hpp"

#define G(cl, ctx, cmd) (m_pfns[static_cast<std::size_t>(megatech::vulkan::dispatch::global::command::cmd)] = (cl)((ctx), (#cmd)))
#define I(cl, ctx, cmd) (m_pfns[static_cast<std::size_t>(megatech::vulkan::dispatch::instance::command::cmd)] = (cl)((ctx), (#cmd)))
#define D(cl, ctx, cmd) (m_pfns[static_cast<std::size_t>(megatech::vulkan::dispatch::device::command::cmd)] = (cl)((ctx), (#cmd)))

namespace megatech::vulkan::dispatch {

namespace global {

#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) G(global, nullptr, cmd);
  table::table(const PFN_vkGetInstanceProcAddr global) {
    if (!global)
    {
      throw dispatch::error{ "The global loader command, \"vkGetInstanceProcAddr\", cannot be null." };
    }
    MEGATECH_VULKAN_DISPATCH_GLOBAL_COMMAND_LIST
    m_pfns[static_cast<std::size_t>(command::vkGetInstanceProcAddr)] = reinterpret_cast<PFN_vkVoidFunction>(global);
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

}

namespace instance {

#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) I(cl, instance, cmd);

  table::table(const megatech::vulkan::dispatch::global::table& global, const VkInstance instance) {
    using gcmd = megatech::vulkan::dispatch::global::command;
    if (!instance)
    {
      throw dispatch::error{ "The \"VkInstance\" handle cannot be null." };
    }
    const auto cl = *reinterpret_cast<const PFN_vkGetInstanceProcAddr*>(global.get(gcmd::vkGetInstanceProcAddr));
    MEGATECH_VULKAN_DISPATCH_INSTANCE_COMMAND_LIST
    m_instance = instance;
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

  VkInstance table::instance() const {
    return m_instance;
  }

}

namespace device {

#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) D(cl, device, cmd);
  table::table(const megatech::vulkan::dispatch::global::table& global,
               const megatech::vulkan::dispatch::instance::table& instance, const VkDevice device) {
    using gcmd = megatech::vulkan::dispatch::global::command;
    if (!device)
    {
      throw dispatch::error{ "The \"VkDevice\" handle cannot be null." };
    }
    const auto gipa = *reinterpret_cast<const PFN_vkGetInstanceProcAddr*>(global.get(gcmd::vkGetInstanceProcAddr));
    const auto cl = reinterpret_cast<PFN_vkGetDeviceProcAddr>(gipa(instance.instance(), "vkGetDeviceProcAddr"));
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    m_instance = instance.instance();
    m_device = device;
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) D(cl, instance, cmd);
  table::table(const megatech::vulkan::dispatch::global::table& global, const VkInstance instance) {
    using gcmd = megatech::vulkan::dispatch::global::command;
    if (!instance)
    {
      throw dispatch::error{ "The \"VkInstance\" handle cannot be null." };
    }
    const auto cl = *reinterpret_cast<const PFN_vkGetInstanceProcAddr*>(global.get(gcmd::vkGetInstanceProcAddr));
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    m_instance = instance;
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND


#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) D(cl, (instance.instance()), cmd);
  table::table(const megatech::vulkan::dispatch::global::table& global,
               const megatech::vulkan::dispatch::instance::table& instance) {
    using gcmd = megatech::vulkan::dispatch::global::command;
    const auto cl = *reinterpret_cast<const PFN_vkGetInstanceProcAddr*>(global.get(gcmd::vkGetInstanceProcAddr));
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    m_instance = instance.instance();
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

#define MEGATECH_VULKAN_DISPATCH_COMMAND(cmd) D(cl, device, cmd);
  table::table(const table& base, const VkDevice device) {
    using dcmd = megatech::vulkan::dispatch::device::command;
    if (base.device())
    {
      throw dispatch::error{ "The base table cannot have a \"VkDevice\" handle loaded." };
    }
    if (!device)
    {
      throw dispatch::error{ "The \"VkDevice\" handle cannot be null." };
    }
    const auto cl = *reinterpret_cast<const PFN_vkGetDeviceProcAddr*>(base.get(dcmd::vkGetDeviceProcAddr));
    MEGATECH_VULKAN_DISPATCH_DEVICE_COMMAND_LIST
    m_instance = base.m_instance;
    m_device = device;
  }
#undef MEGATECH_VULKAN_DISPATCH_COMMAND

  VkInstance table::instance() const {
    return m_instance;
  }

  VkDevice table::device() const {
    return m_device;
  }

}

}
