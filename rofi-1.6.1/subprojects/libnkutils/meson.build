#
# libnkutils - Miscellaneous utilities
#
# Copyright © 2011-2017 Quentin "Sardem FF7" Glidic
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

project('libnkutils', 'c',
    meson_version: '>=0.47.0',
    license: 'MIT',
    default_options: [
        'c_std=gnu11',
        'warning_level=3',
    ],
)

nk_glib_min_version = '2.40'

nk_options = []

nk_modules = [
    'token',
    'gtk-settings',
    'xdg-de',
    'xdg-theme',
    'enum',
    'colour',
]
nk_special_modules = [
    'uuid',
    'bindings',
]
nk_dependencies = [
    [ 'token', 'enum' ],
    [ 'xdg-de', 'enum' ],
    [ 'xdg-theme', 'enum' ],
    [ 'xdg-theme', 'gtk-settings' ],
    [ 'xdg-theme', 'xdg-de' ],
    [ 'bindings', 'enum' ],
    [ 'bindings', 'gtk-settings' ],
    [ 'bindings', 'xdg-de' ],
]

nk_sources = []
nk_args = [
    '-DSRCDIR="@0@"'.format(meson.current_source_dir()),
    '-DSYSCONFDIR="@0@"'.format(join_paths(get_option('prefix'), get_option('sysconfdir'))),
    '-DDATADIR="@0@"'.format(join_paths(get_option('prefix'), get_option('datadir'))),
]
nk_deps = [ dependency('glib-2.0', version: '>= @0@'.format(nk_glib_min_version)) ]

nkutils_xsltpaths = [ join_paths(meson.current_source_dir(), 'man'), meson.current_build_dir() ]
nkutils_manfiles = files('man/libnkutils-man.xml')
nkutils_mandepends = []
nkutils_docbook_conditions = []

foreach m : nk_modules + nk_special_modules
    if get_option(m)
        nk_options += '@0@=true'.format(m)
    endif
    set_variable(m.underscorify(), get_option(m) or not meson.is_subproject())
endforeach

foreach d : nk_dependencies
    if get_option(d[0])
        set_variable(d[1].underscorify(), true)
    endif
endforeach

foreach m : nk_modules
    if get_variable(m.underscorify())
        nk_sources += files(
            'src/@0@.c'.format(m),
            'src/nkutils-@0@.h'.format(m),
        )
    endif
endforeach

if xdg_theme
    nk_deps += [ dependency('gio-2.0'), dependency('gobject-2.0') ]
endif

nk_lib = static_library('nkutils', nk_sources, c_args: nk_args, dependencies: nk_deps)
nk_inc = include_directories('.', 'src')
nk_src = []

if meson.is_subproject()
    nk_git_work_tree = get_option('git-work-tree')
else
    nk_git_work_tree = meson.source_root()
endif

if nk_git_work_tree != ''
    nk_options += 'git-work-tree=@0@'.format(nk_git_work_tree)
    nk_git = find_program('git', required: false)
    if not nk_git.found()
        nk_git = ''
    endif

    nk_git_version = executable('nk-git-version', files('src/git-version.c'), dependencies: dependency('glib-2.0', native: true), native: true)
    nk_src += custom_target('nkutils-git-version.h',
        output: 'nkutils-git-version.h',
        command: [ nk_git_version, 'header', '@OUTPUT@', nk_git_work_tree, join_paths(nk_git_work_tree, '.git'), nk_git ],
        build_always_stale: true,
        build_by_default: not meson.is_subproject(),
    )

    nkutils_mandepends += custom_target('nkutils-git-version.ent',
        output: 'nkutils-git-version.ent',
        command: [ nk_git_version, 'entity', '@OUTPUT@', nk_git_work_tree, join_paths(nk_git_work_tree, '.git'), nk_git ],
        build_always_stale: true,
        build_by_default: not meson.is_subproject(),
    )
endif

libnkutils = declare_dependency(link_with: nk_lib, include_directories: nk_inc, dependencies: nk_deps, sources: nk_src)

if uuid
    nk_libuuid = dependency('uuid', required: false)
    nk_sources = files(
        'src/uuid.c',
        'src/nkutils-uuid.h',
        'src/uuid-internal.h',
    )
    if nk_libuuid.found()
        nk_sources += files('src/uuid-libuuid.c')
        nk_deps += nk_libuuid
    else
        nk_libuuid = dependency('apr-util-1', required: false)
        if nk_libuuid.found()
            nk_sources += files('src/uuid-apr-util.c')
        else
            nk_libuuid = dependency('apr-util', required: false)
            if nk_libuuid.found()
                nk_sources += files('src/uuid-apr-util.c')
                nk_deps += nk_libuuid
            else
                nk_libuuid = disabler()
            endif
        endif
    endif
    if not nk_libuuid.found()
        libnkutils_uuid = disabler()
    else
        nk_deps_uuid = [ nk_libuuid, nk_deps ]
        nk_lib = static_library('nkutils-uuid', nk_sources, c_args: nk_args, dependencies: nk_deps_uuid, build_by_default: false)
        libnkutils_uuid = declare_dependency(link_with: nk_lib, include_directories: nk_inc, dependencies: nk_deps_uuid, sources: nk_src)
    endif
endif

nk_xkbcommon = dependency('xkbcommon', version: '>= 0.4.1', required: false)
if bindings and not nk_xkbcommon.found()
    libnkutils_bindings = disabler()
elif bindings
    if nk_xkbcommon.version().version_compare('>= 0.7.0')
        nk_args += '-DNK_XKBCOMMON_HAS_COMPOSE'
        nk_args += '-DNK_XKBCOMMON_HAS_CONSUMED2'
    elif nk_xkbcommon.version().version_compare('>= 0.5.0')
        nk_args += '-DNK_XKBCOMMON_HAS_COMPOSE'
    endif
    nk_sources = files(
        'src/bindings.c',
        'src/nkutils-bindings.h',
    )
    nk_deps_bindings = [ nk_xkbcommon, libnkutils ]
    nk_lib = static_library('nkutils-bindings', nk_sources, c_args: nk_args, dependencies: nk_deps_bindings, build_by_default: false)
    libnkutils_bindings = declare_dependency(link_with: nk_lib, include_directories: nk_inc, dependencies: nk_deps_bindings, sources: nk_src)
endif

if token
    executable('nk-token-replace', files('src/token-example.c'), dependencies: libnkutils)
endif

if xdg_theme
    executable('nk-xdg-theme-lookup', files('src/xdg-theme-example.c'), dependencies: libnkutils)
endif

if bindings
    nk_xkeyboard_config = dependency('xkeyboard-config', required: false)
    if not nk_xkeyboard_config.found()
        # Do not build and run the test if no keymap data
        bindings = false
    endif
endif

foreach m : nk_modules
    if get_variable(m.underscorify())
        nk_test = executable('nk-@0@.test'.format(m), files('tests/@0@.c'.format(m)), c_args: nk_args, dependencies: libnkutils)
        test('libnkutils @0@ module tests'.format(m), nk_test)
    endif
endforeach

foreach m : nk_special_modules
    if get_variable(m.underscorify())
        nk_test = executable('nk-@0@.test'.format(m), files('tests/@0@.c'.format(m)), c_args: nk_args, dependencies: get_variable('libnkutils_@0@'.format(m.underscorify())))
        test('libnkutils @0@ module tests'.format(m), nk_test)
    endif
endforeach
