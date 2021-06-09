# Copyright (C) 2021 by Michael de Gans
# mike@mdegans.dev
#
# This file is part of cvBlob.
#
# cvBlob is free software: you can redistribute it and/or modify
# it under the terms of the Lesser GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cvBlob is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser GNU General Public License for more details.
#
# You should have received a copy of the Lesser GNU General Public License
# along with cvBlob.  If not, see <https:#www.gnu.org/licenses/>.
#

project('cvblob', ['cpp'],
  version: '0.20.0',
  meson_version: '>= 0.56.0',
  license: 'LGPL',
  default_options: [
    'cpp_std=c++11',  # c++ 11 only
    'warning_level=2',  # -Wall, -Wextra
    'werror=true',  # -Werror
  ],
)

cflags = [
  # '--add', '-Dstrings', 'here',
]

deps = [
  dependency('opencv4', required : false),
]

src = files(
  'cvBlob/cvb_aux.cpp',
  'cvBlob/cvb_blob_list.cpp',
  'cvBlob/cvb_blob.cpp',
  'cvBlob/cvb_contour.cpp',
  # 'cvBlob/cvb_track.cpp',
)

includes = files(
  'cvBlob/cvb_aux.h',
  'cvBlob/cvb_blob_list.h',
  'cvBlob/cvb_blob.h',
  'cvBlob/cvb_contour.h',
  'cvBlob/cvb_defines.h',
  # 'cvBlob/cvb_track.h',
)

# build shared and/or static targets
# FIXME(mdegans): find a way to do this with just `library`
default_library = get_option('default_library')
if default_library == 'shared' or default_library == 'both'
  libcvblob_so = shared_library(meson.project_name(), [src, includes],
    dependencies: deps,
    cpp_args: [
      cflags,
      '-fvisibility=hidden',
      '-DCVB2_BUILD_SHARED',
    ],
    version: meson.project_version(),
    install: true,
  )
elif default_library == 'static' or default_library == 'both'
  libcvblob_a = static_library(meson.project_name(), src,
    include_directories: includes,
    dependencies: deps,
    cpp_args: cflags,
    version: meson.project_version(),
    install: true,
  )
endif

# a dependency object for use as a Meson subproject
cvblob_dep = declare_dependency(
  dependencies: deps,
  link_with: libcvblob_so,
)

# generate pkg-config .pc
pkg = import('pkgconfig')
pkg.generate(libcvblob_so,
  description: 'Find and track blobs using OpenCV',
  requires: deps,
  url: 'https://github.com/Steelskin/cvblob',
  subdirs: meson.project_name(),
)
