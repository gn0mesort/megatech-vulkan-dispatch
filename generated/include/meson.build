headers += custom_target(output: '@BASENAME@', input: files('defs.inl.in'),
                         command: generator + template_arguments + [ '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')
headers += custom_target(output: '@BASENAME@',input: files('commands.inl.in'),
                         command: generator + [ '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')
headers += custom_target(output: '@BASENAME@', input: files('tables.inl.in'),
                         command: generator + [ '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')

