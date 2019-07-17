// Created on: 1992-10-13
// Created by: Ramin BARRETO
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _TCollection_HeaderFile
#define _TCollection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>

#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>

class TCollection_ExtendedString;
class TCollection_HAsciiString;
class TCollection_HExtendedString;
class TCollection_Array1;
class TCollection_HArray1;
class TCollection_Array2;
class TCollection_HArray2;
class TCollection_List;
class TCollection_ListNode;
class TCollection_ListIterator;
class TCollection_BaseSequence;
class TCollection_SeqNode;
class TCollection_Sequence;
class TCollection_SequenceNode;
class TCollection_HSequence;
class TCollection_MapHasher;
class TCollection_BasicMap;
class TCollection_MapNode;
class TCollection_BasicMapIterator;
class TCollection_Map;
class TCollection_MapIterator;
class TCollection_StdMapNode;
class TCollection_DataMap;
class TCollection_DataMapIterator;
class TCollection_DataMapNode;
class TCollection_DoubleMap;
class TCollection_DoubleMapIterator;
class TCollection_DoubleMapNode;
class TCollection_IndexedMap;
class TCollection_IndexedMapNode;
class TCollection_IndexedDataMap;
class TCollection_IndexedDataMapNode;


//! The package <TCollection> provides the services for the
//! transient basic data structures.
class TCollection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns default prefix added for each pointer info string
  Standard_EXPORT static TCollection_AsciiString GetPointerPrefix() { return "0x"; }

  //! Convert handle pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo = true);

  //! Convert pointer to string value
  //! \param thePointer a pointer
  //! \param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! \return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const void* thePointer,
                                                                 const bool isShortInfo = true);

  //! Returns a  prime number greater than  <I> suitable
  //! to dimension a Map.  When  <I> becomes great there
  //! is  a  limit on  the  result (today  the  limit is
  //! around 1 000 000). This is not a limit of the number of
  //! items but a limit in the number  of buckets.  i.e.
  //! there will be more collisions  in  the map.
  Standard_EXPORT static Standard_Integer NextPrimeForMap (const Standard_Integer I);

  //! Returns separator symbol of Dump information
  static Standard_Character DumpSeparator() { return '\\'; }

  //! Returns separator symbol of values vector union
  static Standard_CString VectorSeparator() { return " ,"; }

  //! Returns separator symbol of class name prefix
  static inline Standard_CString ClassNameSeparator() { return " ,"; }

  //! Unites list of string into one string using the separator
  Standard_EXPORT static TCollection_AsciiString Join (const NCollection_List<TCollection_AsciiString>& theValues,
                                                       const TCollection_AsciiString& theSeparator);

  //! Converts stream to map of values where the key is each odd, value is each even value
  //! \param theStream stream value
  //! \param theValues [out] container of split values
  Standard_EXPORT static void Split (const Standard_SStream& theStream,
                                     NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues);

  //! Unites list of string into one string using the separator
  Standard_EXPORT static void Split (const TCollection_AsciiString& theValue,
                                     const TCollection_AsciiString& theSeparator,
                                     NCollection_List<TCollection_AsciiString>& theValues);

  //! Unites list of string into one string using the separator
  Standard_EXPORT static Standard_Boolean SplitReal (const TCollection_AsciiString& theValue,
                                                     const TCollection_AsciiString& theSeparator,
                                                     NCollection_Vector<Standard_Real>& theValues);

  //! Unites vector to string value using VectorSeparator.
  Standard_EXPORT static TCollection_AsciiString ToDumpString (const NCollection_List<TCollection_AsciiString>& theValues);
  //! Converts stream value to string value
  Standard_EXPORT static TCollection_AsciiString ToDumpString (const Standard_SStream& theStream);

  //! Converts stream value to string value
  static TCollection_AsciiString StartKey (const TCollection_AsciiString& theValue)
    { return XMLBracketOpen() + theValue + XMLBracketClose(); }

  //! Converts stream value to string value
  static TCollection_AsciiString StopKey (const TCollection_AsciiString& theValue)
    { return XMLBracketOpen() + XMLFinishKey() + theValue + XMLBracketClose(); }

  Standard_EXPORT static TCollection_AsciiString ConvertDumpToText (const TCollection_AsciiString& theValue);

  //! Splits a AsciiString into two sub-strings using Dump keys.
  //! Example:
  //! aString contains "<key>abc</key>defg"
  //! aString.SplitDumped() gives <me> = "abc" and returns "defg"
  Standard_EXPORT static Standard_Boolean SplitDumped (const TCollection_AsciiString& theSourceValue,
                                                       TCollection_AsciiString& theSplitValue,
                                                       TCollection_AsciiString& theTailValue,
                                                       TCollection_AsciiString& theKey);

  //! Splits value that contains a key in form: <key>value</key>. In this case the values are:
  //! theSplitValue = value, theKey = key.
  Standard_EXPORT static Standard_Boolean SplitKey (const TCollection_AsciiString& theSourceValue,
                                                    TCollection_AsciiString& theSplitValue,
                                                    TCollection_AsciiString& theKey);

protected:
  static TCollection_AsciiString XMLBracketOpen() { return TCollection_AsciiString ("<"); }
  static TCollection_AsciiString XMLBracketClose() { return TCollection_AsciiString (">"); }
  static TCollection_AsciiString XMLFinishKey() { return TCollection_AsciiString ("\\"); }

protected:





private:




friend class TCollection_AsciiString;
friend class TCollection_ExtendedString;
friend class TCollection_HAsciiString;
friend class TCollection_HExtendedString;
friend class TCollection_Array1;
friend class TCollection_HArray1;
friend class TCollection_Array2;
friend class TCollection_HArray2;
friend class TCollection_List;
friend class TCollection_ListNode;
friend class TCollection_ListIterator;
friend class TCollection_BaseSequence;
friend class TCollection_SeqNode;
friend class TCollection_Sequence;
friend class TCollection_SequenceNode;
friend class TCollection_HSequence;
friend class TCollection_MapHasher;
friend class TCollection_BasicMap;
friend class TCollection_MapNode;
friend class TCollection_BasicMapIterator;
friend class TCollection_Map;
friend class TCollection_MapIterator;
friend class TCollection_StdMapNode;
friend class TCollection_DataMap;
friend class TCollection_DataMapIterator;
friend class TCollection_DataMapNode;
friend class TCollection_DoubleMap;
friend class TCollection_DoubleMapIterator;
friend class TCollection_DoubleMapNode;
friend class TCollection_IndexedMap;
friend class TCollection_IndexedMapNode;
friend class TCollection_IndexedDataMap;
friend class TCollection_IndexedDataMapNode;

};

#define DUMP_VALUES(OS, Value1, Value2) \
{ \
  OS << Value1 << TCollection::DumpSeparator() << Value2 << TCollection::DumpSeparator(); \
}

#define DUMP_START_KEY(OS, Value1) { OS << TCollection::StartKey (Value1); }
#define DUMP_STOP_KEY(OS, Value1)  { OS << TCollection::StopKey (Value1); }






#endif // _TCollection_HeaderFile
