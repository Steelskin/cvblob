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

cmake_minimum_required(VERSION 3.1)
project(cvBlob)

# project requires c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# dependencies
find_package(OpenCV REQUIRED)

set(SRC
  cvBlob/cvb_aux.cpp
  cvBlob/cvb_blob_list.cpp
  cvBlob/cvb_blob.cpp
  cvBlob/cvb_contour.cpp
  # cvBlob/cvb_track.cpp
)

set(INCLUDES
  cvBlob/cvb_aux.h
  cvBlob/cvb_blob_list.h
  cvBlob/cvb_blob.h
  cvBlob/cvb_contour.h
  cvBlob/cvb_defines.h
  # cvBlob/cvb_track.h
)

# library target
set(LIB_NAME ${CMAKE_PROJECT_NAME})
add_library(${LIB_NAME} SHARED ${SRC} ${INCLUDES})
target_include_directories(${LIB_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} ${OpenCV_LIBS})

# installation
install(TARGETS ${LIB_NAME}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES ${INCLUDES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${CMAKE_PROJECT_NAME}
)
