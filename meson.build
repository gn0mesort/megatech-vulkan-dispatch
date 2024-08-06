project(
  'megatech-vulkan-dispatch',
  [ 'cpp' ],
  version: '0.0.0-alpha1',
  license: 'AGPL-3.0-or-later',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3',
    'b_ndebug=if-release',
    'b_pie=true',
    'cpp_args=-fstrict-aliasing -Wstrict-aliasing=3'
  ]
)
version = meson.project_version().split('-')[0]
pymod = import('python')
python3 = pymod.find_installation('python3')
has_tools = run_command(python3, '-c',
'''
import megatech.vulkan.applications
''', check: false)
if has_tools.returncode() != 0
  error('Please ensure that all Python requirements are installed.')
endif
includes = include_directories('include')
headers = [ ]
extensions = ','.join(get_option('extensions'))
template_arguments = [ ]
if get_option('extra_defines').allowed()
  template_arguments = [ '--template-arguments=@generate-defines' ]
endif
specification = get_option('specification')
generator = [ find_program('tools/dispatch-table-generator'), '-o@OUTPUT@', '--api=@0@'.format(get_option('api')),
              '--api-version=@0@'.format(get_option('api_version')), '--extensions=@0@'.format(extensions) ]
if specification != ''
  generator += '--specification=@0@'.format(specification)
endif
subdir('generated/include')
sources = [ ]
subdir('generated/src')
lib = library(meson.project_name(), headers + sources, version: version, include_directories: includes, install: true)
megatech_vulkan_dispatch_dep = declare_dependency(link_with: lib, sources: headers, include_directories: includes)
install_headers(files('include/megatech/vulkan/dispatch.hpp'), install_dir: 'include/megatech/vulkan')
install_headers(files('include/megatech/vulkan/dispatch/defs.hpp', 'include/megatech/vulkan/dispatch/commands.hpp',
                      'include/megatech/vulkan/dispatch/tables.hpp'), install_dir: 'include/megatech/vulkan/dispatch')
subdir('tests')
if get_option('tests').allowed()
  test('Dispatch Table Generator', find_program(files('tools/tests/test-dispatch-table-generator')))
endif
doxygen = find_program('doxygen')
doc_env = environment()
doc_env.set('DOXYGEN_PROJECT_NAME', meson.project_name())
doc_env.set('DOXYGEN_PROJECT_NUMBER', meson.project_version())
doc_env.set('DOXYGEN_OUTPUT_DIRECTORY', meson.current_build_dir() / 'documentation')
doc_inputs = [ meson.current_source_dir() / 'include', meson.current_source_dir() / 'generated' ]
foreach header : headers
  doc_inputs += [ header.full_path() ]
endforeach
doc_env.set('DOXYGEN_INPUT', ' '.join(doc_inputs))
doc_strips = [ meson.current_build_dir() / 'generated/include', meson.current_build_dir() / 'generated/src',
               meson.current_source_dir() / 'include' ]
doc_env.set('DOXYGEN_STRIP_FROM_PATH', ' '.join(doc_strips))
doc_env.set('DOXYGEN_HTML_EXTRA_STYLESHEET', files('documentation/extra.css')[0].full_path())
custom_target('documentation', output: 'documentation', build_always_stale: true,
              command: [ doxygen, files('Doxyfile') ], env: doc_env, depends: headers)
