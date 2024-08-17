test_deps = [
  dependency('catch2', required: get_option('tests')),
  dependency('vulkan', required: get_option('tests')),
  megatech_vulkan_dispatch_dep
]

if get_option('tests').allowed()
  test('FNV-1a Hashes', executable('test-fnv-1a', files('test_fnv_1a.cpp'), dependencies: test_deps))
  test('Global Dispatch', executable('test-global-dispatch', files('global_dispatch.cpp'), dependencies: test_deps))
  test('Instance Dispatch', executable('test-instance-dispatch', files('instance_dispatch.cpp'), dependencies: test_deps))
  test('Device Dispatch', executable('test-device-dispatch', files('device_dispatch.cpp'), dependencies: test_deps))
endif
if get_option('benchmarks').allowed()
  benchmark('Benchmark', executable('test-benchmark', 'benchmark.cpp', dependencies: test_deps), verbose: true, timeout: 0)
endif
