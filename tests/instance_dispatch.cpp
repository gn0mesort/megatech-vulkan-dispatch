#include "common.hpp"

TEST_CASE("Instance dispatch tables should resolve function pointers.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  auto instance = create_instance(gdt);
  REQUIRE(instance != nullptr);
  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  REQUIRE(instance == idt.instance());
  DECLARE_INSTANCE_PFN(idt, vkDestroyInstance);
#ifdef MEGATECH_VULKAN_DISPATCH_EXT_DEBUG_UTILS_ENABLED
  DECLARE_INSTANCE_PFN_UNCHECKED(idt, vkCreateDebugUtilsMessengerEXT);
  REQUIRE(vkCreateDebugUtilsMessengerEXT == nullptr);
#endif
  vkDestroyInstance(instance, nullptr);
}

TEST_CASE("Instance dispatch table construction should fail if the instance handle is null.", "[dispatch]") {
  using megatech::vulkan::dispatch::error;
  using gtable = megatech::vulkan::dispatch::global::table;
  using itable = megatech::vulkan::dispatch::instance::table;
  auto gdt = gtable{ vkGetInstanceProcAddr };
  REQUIRE_THROWS_AS((itable{ gdt, nullptr }), error);
}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
