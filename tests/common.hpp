#ifndef COMMON_HPP
#define COMMON_HPP

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>

#define VK_NO_PROTOTYPES (1)
#include <vulkan/vulkan.h>

extern "C" VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance, const char*);

#include <megatech/vulkan/dispatch.hpp>

#define GET_GLOBAL_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd *>((dt).get(megatech::vulkan::dispatch::global::command::cmd)))
#define GET_INSTANCE_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd *>((dt).get(megatech::vulkan::dispatch::instance::command::cmd)))
#define GET_DEVICE_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd *>((dt).get(megatech::vulkan::dispatch::device::command::cmd)))

#define CHECK_PFN(cmd) \
  do \
  { \
    if (!cmd) \
    { \
      FAIL("The required command \"" #cmd "\" was null."); \
    } \
  } \
  while (0)


#define DECLARE_GLOBAL_PFN_UNCHECKED(dt, cmd) \
  const auto cmd = GET_GLOBAL_PFN((dt), cmd)
#define DECLARE_GLOBAL_PFN(dt, cmd) \
  DECLARE_GLOBAL_PFN_UNCHECKED((dt), cmd); \
  CHECK_PFN(cmd)
#define DECLARE_INSTANCE_PFN_UNCHECKED(dt, cmd) \
  const auto cmd = GET_INSTANCE_PFN((dt), cmd)
#define DECLARE_INSTANCE_PFN(dt, cmd) \
  DECLARE_INSTANCE_PFN_UNCHECKED((dt), cmd); \
  CHECK_PFN(cmd)
#define DECLARE_DEVICE_PFN_UNCHECKED(dt, cmd) \
  const auto cmd = GET_DEVICE_PFN((dt), cmd)
#define DECLARE_DEVICE_PFN(dt, cmd) \
  DECLARE_DEVICE_PFN_UNCHECKED((dt), cmd); \
  CHECK_PFN(cmd)

#define VK_CHECK(exp) \
  do \
  { \
    if (const auto res = (exp); res != VK_SUCCESS) \
    { \
      FAIL("The expression \"" #exp "\" failed."); \
    } \
  } \
  while (0)

static inline VkInstance create_instance(const megatech::vulkan::dispatch::global::table& gdt) {
  auto app_info = VkApplicationInfo{ };
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.apiVersion = VK_API_VERSION_1_0;
  auto instance_info = VkInstanceCreateInfo{ };
  instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo = &app_info;
  DECLARE_GLOBAL_PFN(gdt, vkCreateInstance);
  auto instance = VkInstance{ };
  VK_CHECK(vkCreateInstance(&instance_info, nullptr, &instance));
  return instance;
}

static inline VkDevice create_device(const megatech::vulkan::dispatch::instance::table& idt) {
  DECLARE_INSTANCE_PFN(idt, vkEnumeratePhysicalDevices);
  auto sz = std::uint32_t{ };
  VK_CHECK(vkEnumeratePhysicalDevices(idt.instance(), &sz, nullptr));
  auto physical_devices = new VkPhysicalDevice[sz];
  VK_CHECK(vkEnumeratePhysicalDevices(idt.instance(), &sz, physical_devices));
  auto device_info = VkDeviceCreateInfo{ };
  device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  DECLARE_INSTANCE_PFN(idt, vkCreateDevice);
  auto device = VkDevice{ };
  VK_CHECK(vkCreateDevice(physical_devices[0], &device_info, nullptr, &device));
  delete[] physical_devices;
  return device;
}

#endif
