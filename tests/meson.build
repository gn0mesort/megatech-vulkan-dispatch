test_deps = [
  dependency('catch2', required: get_option('tests')),
  dependency('vulkan', required: get_option('tests')),
  megatech_vulkan_dispatch_dep
]

test('Global Dispatch', executable('test-global-dispatch', 'global_dispatch.cpp', dependencies: test_deps))
test('Instance Dispatch', executable('test-instance-dispatch', 'instance_dispatch.cpp', dependencies: test_deps))
test('Device Dispatch', executable('test-device-dispatch', 'device_dispatch.cpp', dependencies: test_deps))
