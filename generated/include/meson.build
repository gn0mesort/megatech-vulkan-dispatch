headers += custom_target('defs.inl', output: '@BASENAME@', input: files('defs.inl.in'),
                         command: [ generator, '-o@OUTPUT@', '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')
headers += custom_target('commands.inl', output: '@BASENAME@',input: files('commands.inl.in'),
                         command: [ generator, '-o@OUTPUT@', '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')
headers += custom_target('tables.inl', output: '@BASENAME@', input: files('tables.inl.in'),
                         command: [ generator, '-o@OUTPUT@', '@INPUT@' ], install: true,
                         install_dir: 'include/megatech/vulkan/dispatch')

