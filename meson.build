project(
  'megatech-vulkan-dispatch',
  [ 'cpp' ],
  version: '1.1.0',
  license: 'AGPL-3.0-or-later',
  license_files: [ 'LICENSE' ],
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'b_ndebug=if-release',
    'b_pie=true',
    'cpp_args=-fstrict-aliasing -Wstrict-aliasing=3'
  ]
)
version = meson.project_version().split('-')[0]
description = 'Megatech Vulkan Dispatch Library'
pymod = import('python')
python3 = pymod.find_installation('python3')
has_tools = run_command(python3, '-c',
'''
import megatech.vulkan.applications
''', check: false)
if has_tools.returncode() != 0
  error('Please ensure that all Python requirements are installed.')
endif
megatech_assertions_dep = dependency('megatech-assertions',
                                     fallback: [ 'megatech-assertions', 'megatech_assertions_dep' ],
                                     version: [ '>=1.0.0', '<2.0.0' ])
if get_option('buildtype') == 'release'
  megatech_assertions_dep = megatech_assertions_dep.partial_dependency(includes: true)
endif
dependencies = [ megatech_assertions_dep ]
includes = include_directories('include')
headers = [ ]
extensions = ','.join(get_option('extensions'))
template_arguments = [ ]
if get_option('extra_defines').allowed()
  template_arguments = [ '--template-arguments=@generate-defines' ]
endif
generator = [ find_program('dispatch-table-generator'), '-o@OUTPUT@', '--api=@0@'.format(get_option('api')),
              '--api-version=@0@'.format(get_option('api_version')), '--extensions=@0@'.format(extensions) ]
if not get_option('generator_warnings').allowed()
  generator += '--quiet'
endif
specification = get_option('specification')
if specification != ''
  generator += '--specification=@0@'.format(specification)
endif
if not get_option('deprecated_vulkan_features').allowed()
  generator += '--no-enable-deprecated'
endif
subdir('generated/include')
sources = [
  files('src/megatech/vulkan/dispatch/error.cpp', 'src/megatech/vulkan/dispatch/tables.cpp')
]
lib = library(meson.project_name(), headers + sources, version: version, dependencies: dependencies,
              include_directories: includes, install: true)
megatech_vulkan_dispatch_dep = declare_dependency(link_with: lib, sources: headers, include_directories: includes)
install_headers(files('include/megatech/vulkan/dispatch.hpp'), install_dir: 'include/megatech/vulkan')
install_headers(files('include/megatech/vulkan/dispatch/defs.hpp', 'include/megatech/vulkan/dispatch/commands.hpp',
                      'include/megatech/vulkan/dispatch/error.hpp', 'include/megatech/vulkan/dispatch/tables.hpp'),
                install_dir: 'include/megatech/vulkan/dispatch')
install_headers(files('include/megatech/vulkan/dispatch/internal/base/fnv_1a.hpp'),
                install_dir: 'include/megatech/vulkan/dispatch/internal/base')
pkgconfig = import('pkgconfig')
pkgconfig.generate(lib, url: 'https://github.com/gn0mesort/megatech-vulkan-dispatch',
                   description: description)
subdir('tests')
doxygen = find_program('doxygen', disabler: true)
doc_env = environment()
doc_env.set('DOXYGEN_PROJECT_NAME', meson.project_name())
doc_env.set('DOXYGEN_PROJECT_NUMBER', meson.project_version())
doc_env.set('DOXYGEN_OUTPUT_DIRECTORY', meson.current_build_dir() / 'documentation')
doc_inputs = [ meson.current_source_dir() / 'include', meson.current_source_dir() / 'generated',
               files('README.md')[0].full_path(), files('documentation/namespaces.dox')[0].full_path() ]
doc_env.set('DOXYGEN_USE_MDFILE_AS_MAINPAGE', doc_inputs[2])
foreach header : headers
  doc_inputs += [ header.full_path() ]
endforeach
doc_env.set('DOXYGEN_INPUT', ' '.join(doc_inputs))
doc_strips = [ meson.current_build_dir() / 'generated/include', meson.current_build_dir() / 'generated/src',
               meson.current_source_dir() / 'include' ]
doc_env.set('DOXYGEN_STRIP_FROM_PATH', ' '.join(doc_strips))
extra_css = files('documentation/extra.css')[0]
doc_env.set('DOXYGEN_HTML_EXTRA_STYLESHEET', extra_css.full_path())
doc_env.set('DOXYGEN_ENABLED_SECTIONS', ' '.join(get_option('enabled_doxygen_sections')))
custom_target(output: 'documentation', build_always_stale: true,
              command: [ doxygen, files('Doxyfile') ], env: doc_env, depends: headers)
