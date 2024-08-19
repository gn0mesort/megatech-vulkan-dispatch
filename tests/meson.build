dependencies = [
  megatech_vulkan_dispatch_dep
]

if get_option('tests').allowed() or get_option('benchmarks').allowed()
  dependencies += dependency('catch2')
  dependencies += dependency('vulkan')
endif

if get_option('tests').allowed()
  test('FNV-1a Hashes',
        executable('test-fnv-1a', files('test_fnv_1a.cpp'), dependencies: dependencies))
  test('Global Dispatch',
        executable('test-global-dispatch', files('test_global_dispatch.cpp'), dependencies: dependencies))
  test('Instance Dispatch',
        executable('test-instance-dispatch', files('test_instance_dispatch.cpp'), dependencies: dependencies))
  test('Device Dispatch',
        executable('test-device-dispatch', files('test_device_dispatch.cpp'), dependencies: dependencies))
endif
if get_option('benchmarks').allowed()
  benchmark('Dispatch', executable('benchmark-dispatch', files('benchmark_dispatch.cpp'), dependencies: dependencies),
            verbose: true, timeout: 0)
endif
