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

# path to game data
if get_option('buildtype') == 'debug'
  data_dir = join_paths(meson.current_source_dir(), 'data')
else
  data_dir = join_paths(get_option('prefix'), get_option('datadir'),
                        'games', meson.project_name())
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
  install_dir: data_dir)
install_data('extra/pipewalker.desktop',
  install_dir: join_paths(get_option('datadir'), 'applications'))
install_data('extra/pipewalker.png',
  install_dir: join_paths(get_option('datadir'), 'icons/hicolor/64x64/apps'))
install_man('extra/pipewalker.6')

# configuration file
conf = configuration_data()
conf.set_quoted('APP_VERSION', version)
conf.set_quoted('APP_DATADIR', data_dir)
if get_option('buildtype') == 'release'
  conf.set('NDEBUG', 1)
else
  conf.set('DEBUG', 1)
endif
configure_file(output: 'buildcfg.h', configuration: conf)

executable(
  'pipewalker',
  [
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
  ],
  dependencies: [
    sdl_base,
    sdl_image,
  ],
  install: true
)
