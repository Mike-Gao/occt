// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef OSD_Function_HeaderFile
#define OSD_Function_HeaderFile

#include <Standard_Std.hxx>

// This is in fact a pointer to a function.
// This is not an "enumeration" but a trick to solve an omission from CDL.

typedef int (* OSD_Function)(...);

//! Converts the given pointer to function @p theFromFunction
//! to the type of the pointer to function specified as the first template argument @p TheToFunction.
//! This function is necessary to explicitly mark the places where such conversion takes place
//! and to suppress @em gcc warning @c -Wcast-function-type.
//! @tparam TheToFunction the type to which the given pointer to function must be casted
//! (it may be the type of the function or the type of the pointer to function)
//! @tparam TheFromFunction the type of the function to be casted
//! @tparam AToFunction it is not an actual template parameter,
//! but a simple alias needed to reduce the amount of boilerplate
//! @param theFromFunction the pointer to the function to be casted
//! @return the pointer of the function of type @c AToFunction* that is the result of the cast
template <typename TheToFunction,
          typename TheFromFunction,
          typename AToFunction = typename opencascade::std::remove_pointer<TheToFunction>::type>
typename opencascade::std::enable_if<opencascade::std::is_function<AToFunction>::value
                                       && opencascade::std::is_function<TheFromFunction>::value,
                                     AToFunction*>::type
OSD_FunctionCast (TheFromFunction* const theFromFunction)
{
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#  if (__GNUC__ > 8) || ((__GNUC__ == 8) && (__GNUC_MINOR__ >= 1))
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#  endif
#endif
  return reinterpret_cast<AToFunction*> (theFromFunction);
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#  if (__GNUC__ > 8) || ((__GNUC__ == 8) && (__GNUC_MINOR__ >= 1))
#    pragma GCC diagnostic pop
#  endif
#endif
}

#endif
