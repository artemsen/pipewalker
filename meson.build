# Rules for building with Meson

project(
  'pipewalker',
  'cpp',
  default_options: [
   'warning_level=3',
   'cpp_std=c++11',
  ],
  license: 'MIT',
  version: '0.0.0',
  meson_version: '>=0.59.0',
)

# version info
version = get_option('version')
if version == '0.0.0'
  git = find_program('git', native: true, required: false)
  if git.found()
    git_ver = run_command([git, 'describe', '--tags', '--long', '--always', '--dirty'], check: false)
    if git_ver.returncode() == 0
      version = git_ver.stdout().strip().substring(1)
    endif
  endif
endif

# mandatory dependencies
sdl_base = dependency('sdl2')
sdl_image = dependency('SDL2_image')

# install path
if host_machine.system() == 'windows'
  install_bin_dir = get_option('prefix') / f'PipeWalker-@version@'
  install_data_dir = install_bin_dir / 'data'
else
  install_bin_dir = get_option('bindir')
  install_data_dir = get_option('datadir') / 'games' / meson.project_name()
endif

# installation
install_data(
  [
    'data/Hellsfire.png',
    'data/Network.png',
    'data/Plumbing.png',
    'data/clatz.wav',
    'data/complete.wav',
  ],
  install_dir: install_data_dir)
if host_machine.system() != 'windows'
  install_data('extra/pipewalker.desktop',
    install_dir: get_option('datadir') / 'applications')
  install_data('extra/pipewalker.png',
    install_dir: get_option('datadir') / 'icons/hicolor/64x64/apps')
  install_man('extra/pipewalker.6')
endif

# path to game data
if get_option('buildtype') == 'debug'
  appdata_dir = meson.current_source_dir() / 'data'
elif host_machine.system() == 'windows'
  appdata_dir = 'data'
else
  appdata_dir = get_option('prefix') / install_data_dir
endif

# configuration file
conf = configuration_data()
if get_option('buildtype') == 'release'
  conf.set('NDEBUG', 1)
else
  conf.set('DEBUG', 1)
endif
conf.set_quoted('APP_DATADIR', appdata_dir + '/')
conf.set_quoted('APP_VERSION', version)
if host_machine.system() == 'windows'
  vnum = version.split('-')[0].split('.')
  assert(vnum.length() == 3, f'Invalid version string: @version@')
  conf.set('APP_VERSION_MAJOR', vnum[0].to_int())
  conf.set('APP_VERSION_MINOR', vnum[1].to_int())
  conf.set('APP_VERSION_PATCH', vnum[2].to_int())
endif
configure_file(output: 'buildcfg.h', configuration: conf)

# source files
sources = [
    'src/cell.cpp',
    'src/firework.cpp',
    'src/game.cpp',
    'src/layout.cpp',
    'src/level.cpp',
    'src/main.cpp',
    'src/mtrand.cpp',
    'src/render.cpp',
    'src/skin.cpp',
    'src/sound.cpp',
    'src/state.cpp',
]
if host_machine.system() == 'windows'
  sources += import('windows').compile_resources(
    'src/winres.rc',
    include_directories: [meson.current_source_dir() / 'extra',
                          meson.current_build_dir()]
)
endif

executable(
  'pipewalker',
  sources,
  dependencies: [
    sdl_base,
    sdl_image,
  ],
  install: true,
  install_dir: install_bin_dir,
)
