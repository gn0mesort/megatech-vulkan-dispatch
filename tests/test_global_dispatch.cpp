#include "common.hpp"

TEST_CASE("Global dispatch tables should resolve function pointers.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  REQUIRE(GET_GLOBAL_PFN(gdt, vkGetInstanceProcAddr) == vkGetInstanceProcAddr);
  DECLARE_GLOBAL_PFN(gdt, vkEnumerateInstanceVersion);
  auto ver = std::uint32_t{ };
  VK_CHECK(vkEnumerateInstanceVersion(&ver));
  REQUIRE(VK_API_VERSION_MAJOR(ver) >= 1);
  REQUIRE(ver >= VK_API_VERSION_1_0);
}

TEST_CASE("Global dispatch table construction should fail if the global loader function is null.", "[dispatch]") {
  using megatech::vulkan::dispatch::error;
  using megatech::vulkan::dispatch::global::table;
  REQUIRE_THROWS_AS(table{ nullptr }, error);
}

TEST_CASE("Global dispatch tables should resolve function pointers via hash values.", "[dispatch]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  REQUIRE(GET_GLOBAL_PFN(gdt, vkGetInstanceProcAddr) == vkGetInstanceProcAddr);
  DECLARE_PFN_BY_HASH(gdt, vkEnumerateInstanceVersion);
  auto ver = std::uint32_t{ };
  VK_CHECK(vkEnumerateInstanceVersion(&ver));
  REQUIRE(VK_API_VERSION_MAJOR(ver) >= 1);
  REQUIRE(ver >= VK_API_VERSION_1_0);
}

TEST_CASE("Global dispatch tables should fail to resolve unknown hash values.", "[dispatch][!shouldfail]") {
  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  get_pfn_by_name(gdt, "vkNotARealVulkanCommandMEGATECH");
}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
