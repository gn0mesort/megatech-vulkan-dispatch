#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstddef>
#include <cinttypes>

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#define VK_NO_PROTOTYPES (1)
#include <vulkan/vulkan.h>

extern "C" VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance, const char*);

#include <megatech/vulkan/dispatch.hpp>

#define GET_GLOBAL_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd*>((dt).get(megatech::vulkan::dispatch::global::command::cmd)))
#define GET_INSTANCE_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd*>((dt).get(megatech::vulkan::dispatch::instance::command::cmd)))
#define GET_DEVICE_PFN(dt, cmd) (*reinterpret_cast<const PFN_##cmd*>((dt).get(megatech::vulkan::dispatch::device::command::cmd)))

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


// This is all identical to the library definition **EXCEPT** it can be computed at runtime.
template <std::unsigned_integral Type, Type Offset, Type Prime, Type MaxDigits = std::numeric_limits<Type>::digits>
inline Type basic_fnv_1a_cstr(const char* str) {
  static_assert(MaxDigits <= std::numeric_limits<Type>::digits, "The maximum number of radix digits in an FNV-1a hash "
                "cannot exceed the the width of its data type.");
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

inline std::uint_least64_t fnv_1a_cstr(const char* str) {
  return basic_fnv_1a_cstr<std::uint_least64_t, 0xcbf29ce484222325, 0x100000001b3, 64>(str);
}

template <typename DispatchTable>
const void* get_pfn_by_name(DispatchTable&& dt, const char* name) {
  const auto hash = fnv_1a_cstr(name);
  const auto ppfn = dt.get(hash);
  if (!ppfn)
  {
    FAIL("The hash " << hash << " which corresponds to the name \"" << name << "\" was not recognized.");
  }
  return ppfn;
}

#define DECLARE_PFN_BY_HASH_UNCHECKED(dt, cmd) \
  const auto cmd = *reinterpret_cast<const PFN_##cmd*>(get_pfn_by_name((dt), #cmd))
#define DECLARE_PFN_BY_HASH(dt, cmd) \
  DECLARE_PFN_BY_HASH_UNCHECKED(dt, cmd); \
  CHECK_PFN(cmd)


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
