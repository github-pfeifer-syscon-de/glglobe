
#
# see https://mesonbuild.com/Gnome-module.html
gnome = import('gnome')

app_resources = gnome.compile_resources(
    'resources', meson.project_name() + '.gresources.xml'
    , source_dir: meson.current_source_dir()
)

# icons + desktop
dataDir = join_paths(get_option('prefix'), get_option('datadir'))
iconDir = join_paths(dataDir, 'icons/hicolor')
install_data(
      meson.project_name() + '.png'
    , install_dir: join_paths(iconDir, '64x64/apps'))
install_data(
      meson.project_name() + '.svg'
    , install_dir: join_paths(iconDir, 'scalable/apps'))
install_data(
      meson.project_name() + '.desktop'
    , install_dir: join_paths(dataDir, 'applications'))

# These textrues are needed to show something meaningful, if you don't like them replace them
curl_exec = find_program('curl', required: true)
texture_files = [
    '2k_moon.jpg'
  , '2k_earth_specular_map.tif'
  , '2k_earth_normal_map.tif'
  , '2k_earth_nightmap.jpg'
  , '2k_earth_daymap.jpg'
]
foreach texture_file : texture_files
    url = 'https://www.solarsystemscope.com/textures/download/' + texture_file
    custom_target(
        build_by_default   : true
        , command          : [curl_exec, url, '-o', '@OUTPUT@']
        , output           : texture_file
        , install           : true
        , install_dir      : pkgdatadir)
endforeach
