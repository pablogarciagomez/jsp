project('jobshop', 'cpp', version : '0.1', default_options : ['warning_level=3', 'cpp_std=c++17'])
cc = meson.get_compiler('cpp')
filesystem = cc.find_library('stdc++fs')
executable('jobshop', 'main.cpp', dependencies : [filesystem])