project(
  'Graphics Tests',
  'c',
  version : '0.0.1',
  license : 'CC0-1.0',
  meson_version : '>= 1.1',
  default_options : ['c_std=gnu99', 'warning_level=2'],
)

cc = meson.get_compiler('c')

dep_gbm = dependency('gbm')
dep_egl = dependency('egl')
dep_gles2 = dependency('glesv2')

if get_option('lightweight-offscreen').enabled()
  subdir('lightweight-offscreen')
endif
