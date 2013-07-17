// Copyright (C) 2007 by Cristóbal Carnero Liñán
// grendel.ccl@gmail.com
//
// Copyright (C) 2013 by Fabrice de Gans-Riberi for ProViSys Engineering
// fabrice.degans@gmail.com
//
// This file is part of cvBlob.
//
// cvBlob is free software: you can redistribute it and/or modify
// it under the terms of the Lesser GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// cvBlob is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Lesser GNU General Public License for more details.
//
// You should have received a copy of the Lesser GNU General Public License
// along with cvBlob.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _CVBLOB_DEFINES_H_
#define _CVBLOB_DEFINES_H_

#ifdef _MSC_VER // MS VC

# ifdef CVBLOB_LIB
#  define CVBLOB_EXPORT __declspec(dllexport)
# else
#  define CVBLOB_EXPORT
# endif

#else // Other compilers

# define CVBLOB_EXPORT

#endif // Compiler

#endif  // _CVBLOB_DEFINES_H_