#include "common.hpp"

// FAIL(...) is very expensive in benchmarks and not representative of what real client programs would do so I'm
// eliminating it here. Throwing some kind of run-time error is what client programs would do, so I think this is
// more representative of the actual cost.
#undef CHECK_PFN
#define CHECK_PFN(pfn) \
  do \
  { \
    if (!pfn) \
    { \
      throw megatech::vulkan::dispatch::error("The function-pointer returned by the dispatch table was null."); \
    } \
  } \
  while (0)

#define CHECK_PPFN(ppfn) \
  do \
  { \
    if (!ppfn) \
    { \
      throw megatech::vulkan::dispatch::error("The pointer-to-function-pointer returned by the dispatch table was " \
                                              "null."); \
    } \
  } \
  while (0)

TEST_CASE("Bare Vulkan Loader Functions Vs megatech::vulkan::dispatch Tables", "[dispatch][benchmark]") {
  // Global Dispatch
  BENCHMARK("vkGetInstanceProcAddr(nullptr, ...)") {
    const auto pfn = vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");
    CHECK_PFN(pfn);
    return pfn;
  };

  auto gdt = megatech::vulkan::dispatch::global::table{ vkGetInstanceProcAddr };
  BENCHMARK("Global Preload with Index") {
    const auto ppfn = gdt.get(megatech::vulkan::dispatch::global::command::vkEnumerateInstanceLayerProperties);
    const auto pfn = *reinterpret_cast<const PFN_vkEnumerateInstanceLayerProperties*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Global Preload with Run-Time Hash") {
    const auto ppfn = gdt.get(fnv1a_cstr("vkEnumerateInstanceLayerProperties"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkEnumerateInstanceLayerProperties*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  auto hash = fnv1a_cstr("vkEnumerateInstanceLayerProperties");
  BENCHMARK("Global Preload with Stored Run-Time Hash") {
    const auto ppfn = gdt.get(hash);
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkEnumerateInstanceLayerProperties*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  auto global_command = megatech::vulkan::dispatch::global::to_command(hash);
  BENCHMARK("Global Preload with Stored Run-Time Hash->Index Map") {
    const auto ppfn = gdt.get(global_command);
    const auto pfn = *reinterpret_cast<const PFN_vkEnumerateInstanceLayerProperties*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Global Preload with Compile-Time Hash") {
    using namespace megatech::vulkan::dispatch;
    const auto ppfn = gdt.get(internal::base::fnv1a_cstr("vkEnumerateInstanceLayerProperties"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkEnumerateInstanceLayerProperties*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };

  // Instance Dispatch
  auto instance = create_instance(gdt);
  BENCHMARK("vkGetInstanceProcAddr(instance, ...)") {
    const auto pfn = vkGetInstanceProcAddr(instance, "vkDestroyInstance");
    CHECK_PFN(pfn);
    return pfn;
  };

  auto idt = megatech::vulkan::dispatch::instance::table{ gdt, instance };
  BENCHMARK("Instance Preload with Index") {
    const auto ppfn = idt.get(megatech::vulkan::dispatch::instance::command::vkDestroyInstance);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyInstance*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Instance Preload with Run-Time Hash") {
    const auto ppfn = idt.get(fnv1a_cstr("vkDestroyInstance"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyInstance*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  hash = fnv1a_cstr("vkDestroyInstance");
  BENCHMARK("Instance Preload with Stored Run-Time Hash") {
    const auto ppfn = idt.get(hash);
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyInstance*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  auto instance_command = megatech::vulkan::dispatch::instance::to_command(hash);
  BENCHMARK("Instance Preload with Stored Run-Time Hash->Index Map") {
    const auto ppfn = idt.get(instance_command);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyInstance*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Instance Preload with Compile-Time Hash") {
    using namespace megatech::vulkan::dispatch;
    const auto ppfn = idt.get(internal::base::fnv1a_cstr("vkDestroyInstance"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyInstance*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };

  // Device Dispatch
  auto device = create_device(idt);
  auto vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
  CHECK_PFN(vkGetDeviceProcAddr);
  vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vkGetDeviceProcAddr(device, "vkGetDeviceProcAddr"));
  CHECK_PFN(vkGetDeviceProcAddr);
  BENCHMARK("vkGetDeviceProcAddr(device, ...)") {
    const auto pfn = vkGetDeviceProcAddr(device, "vkDestroyDevice");
    CHECK_PFN(pfn);
    return pfn;
  };

  auto ddt = megatech::vulkan::dispatch::device::table{ gdt, idt, device };
  BENCHMARK("Device Preload with Index") {
    const auto ppfn = ddt.get(megatech::vulkan::dispatch::device::command::vkDestroyDevice);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyDevice*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Device Preload with Run-Time Hash") {
    const auto ppfn = ddt.get(fnv1a_cstr("vkDestroyDevice"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyDevice*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  hash = fnv1a_cstr("vkDestroyDevice");
  BENCHMARK("Device Preload with Stored Run-Time Hash") {
    const auto ppfn = ddt.get(hash);
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyDevice*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  auto device_command = megatech::vulkan::dispatch::device::to_command(hash);
  BENCHMARK("Device Preload with Stored Run-Time Hash->Index Map") {
    const auto ppfn = ddt.get(device_command);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyDevice*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };
  BENCHMARK("Device Preload with Compile-Time Hash") {
    using namespace megatech::vulkan::dispatch;
    const auto ppfn = ddt.get(internal::base::fnv1a_cstr("vkDestroyDevice"));
    CHECK_PPFN(ppfn);
    const auto pfn = *reinterpret_cast<const PFN_vkDestroyDevice*>(ppfn);
    CHECK_PFN(pfn);
    return pfn;
  };

}

int main(int argc, char** argv) {
  return Catch::Session().run(argc, argv);
}
