#include "common.hpp"

TEST_CASE("Device dispatch tables should resolve function pointers.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  auto device = create_device(idt);
  REQUIRE(device != nullptr);
  auto ddt = megatech::vulkan::dispatch::device::table{ gdt, idt, device };
  REQUIRE(device == ddt.device());
  REQUIRE(instance == ddt.instance());
  DECLARE_DEVICE_PFN(ddt, vkDestroyDevice);
#ifdef MEGATECH_VULKAN_DISPATCH_KHR_SWAPCHAIN_ENABLED
  DECLARE_DEVICE_PFN_UNCHECKED(ddt, vkCreateSwapchainKHR);
  REQUIRE(vkCreateSwapchainKHR == nullptr);
#endif
  vkDestroyDevice(device, nullptr);
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
  vkDestroyInstance(instance, nullptr);
}

TEST_CASE("Device dispatch tables should resolve function pointers with only a VkInstance.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  REQUIRE(instance != nullptr);
  {
    auto ddt = megatech::vulkan::dispatch::device::table{ gdt, instance };
    REQUIRE(ddt.instance() == instance);
    REQUIRE(ddt.device() == nullptr);
    DECLARE_DEVICE_PFN(ddt, vkDestroyDevice);
  }
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  {
    auto ddt = megatech::vulkan::dispatch::device::table{ gdt, idt };
    REQUIRE(ddt.instance() == idt.instance());
    REQUIRE(ddt.device() == nullptr);
    DECLARE_DEVICE_PFN(ddt, vkDestroyDevice);
  }
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
  vkDestroyInstance(instance, nullptr);
}

TEST_CASE("Device dispatch tables should resolve function pointers via hash values.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  auto device = create_device(idt);
  REQUIRE(device != nullptr);
  auto ddt = megatech::vulkan::dispatch::device::table{ gdt, idt, device };
  REQUIRE(device == ddt.device());
  REQUIRE(instance == ddt.instance());
  DECLARE_PFN_BY_HASH(ddt, vkDestroyDevice);
#ifdef MEGATECH_VULKAN_DISPATCH_KHR_SWAPCHAIN_ENABLED
  DECLARE_PFN_BY_HASH_UNCHECKED(ddt, vkCreateSwapchainKHR);
  REQUIRE(vkCreateSwapchainKHR == nullptr);
#endif
  vkDestroyDevice(device, nullptr);
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
  vkDestroyInstance(instance, nullptr);
}

TEST_CASE("Device dispatch tables should fail to resolve unknown hash values.", "[dispatch][!shouldfail]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  auto device = create_device(idt);
  REQUIRE(device != nullptr);
  auto ddt = megatech::vulkan::dispatch::device::table{ gdt, idt, device };
  REQUIRE(device == ddt.device());
  REQUIRE(instance == ddt.instance());
  get_pfn_by_name(ddt, "vkNotARealVulkanCommandMEGATECH");
}

TEST_CASE("Device dispatch table construction should fail if the instance handle is null.", "[dispatch]") {
  using megatech::vulkan::dispatch::error;
  using gtable = megatech::vulkan::dispatch::global::table;
  using dtable = megatech::vulkan::dispatch::device::table;
  auto gdt = gtable{ vkGetInstanceProcAddr };
  REQUIRE_THROWS_AS((dtable{ gdt, nullptr }), error);
}

TEST_CASE("Device dispatch table construction should fail if the device handle is null.", "[dispatch]") {
  using megatech::vulkan::dispatch::error;
  using gtable = megatech::vulkan::dispatch::global::table;
  using itable = megatech::vulkan::dispatch::instance::table;
  using dtable = megatech::vulkan::dispatch::device::table;
  auto gdt = gtable{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  auto idt = itable{ gdt, instance };
  REQUIRE_THROWS_AS((dtable{ gdt, idt, nullptr }), error);
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
  vkDestroyInstance(instance, nullptr);
}

TEST_CASE("Device dispatch tables should be constructible from a base table and a VkDevice.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  REQUIRE(instance != nullptr);
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  auto iddt = megatech::vulkan::dispatch::device::table{ gdt, idt };
  auto device = create_device(idt);
  REQUIRE(device != nullptr);
  auto ddt = megatech::vulkan::dispatch::device::table{ iddt, device };
  REQUIRE(device == ddt.device());
  REQUIRE(instance == ddt.instance());
  DECLARE_DEVICE_PFN(ddt, vkDestroyDevice);
  vkDestroyDevice(device, nullptr);
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
  vkDestroyInstance(instance, nullptr);
}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
