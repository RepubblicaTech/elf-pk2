project('libelf-pk2', 'c',
  version : '0.1',
  default_options : [
  'warning_level=3',
  'c_std=gnu23'
  ])

sources = [
  'src/main.c',
  'src/elf.c'
]

target = host_machine.cpu_family()
subdir('src/arch'/target)

inc = include_directories('include', 'include/arch'/target)

executable('libelf-pk2', sources, include_directories: inc)
