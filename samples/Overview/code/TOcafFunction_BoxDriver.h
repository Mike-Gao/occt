// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef _TOcafFunction_BoxDriver_HeaderFile
#define _TOcafFunction_BoxDriver_HeaderFile

#include <TFunction_Driver.hxx>
#include <TFunction_Logbook.hxx>

class TOcafFunction_BoxDriver;
DEFINE_STANDARD_HANDLE(TOcafFunction_BoxDriver, TFunction_Driver)

//! Creation of an instance of the driver.It's possible (and recommended)
//! to have only one instance of a driver for the whole session.
class TOcafFunction_BoxDriver : public TFunction_Driver
{
public:
  DEFINE_STANDARD_RTTI_INLINE(TOcafFunction_BoxDriver, TFunction_Driver)

  Standard_EXPORT static const Standard_GUID& GetID ();
  Standard_EXPORT TOcafFunction_BoxDriver () { }
  Standard_EXPORT virtual  void Validate (Handle (TFunction_Logbook)& log) const override;
  Standard_EXPORT virtual  Standard_Boolean MustExecute (const Handle (TFunction_Logbook)& log) const override;
  Standard_EXPORT virtual  Standard_Integer Execute (Handle (TFunction_Logbook)& log) const override;
};


#endif
