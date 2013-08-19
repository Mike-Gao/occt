// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// Workaround: Handles for none-cdl classes.
//             Delete this file after transformation all classes to none-cdl ones.

#ifndef _Transfer_TransientProcess_Handle_HeaderFile
#define _Transfer_TransientProcess_Handle_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <MMgt_TShared.hxx>

DEFINE_STANDARD_HANDLE(Transfer_TransientProcess,MMgt_TShared)

typedef Handle(Transfer_TransientProcess) Transfer_TransientProcess_Handle;

#endif
