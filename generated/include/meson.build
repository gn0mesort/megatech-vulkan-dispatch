headers += custom_target(output: '@BASENAME@', input: files('defs.inl.in'),
                         command: generator + template_arguments + [ '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')

