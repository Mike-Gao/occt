// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Standard_Dump_HeaderFile
#define _Standard_Dump_HeaderFile

#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>

#include <Standard_SStream.hxx>
#include <TCollection_AsciiString.hxx>


//! The interface to prepare and parse an object Dump.
class Standard_Dump
{
public:
  /**
    * @brief Simple sentry class providing convenient interface to dump.
    * 
    * Appends start and last rows in dump with class name key
    *
    * Create instance of that class in the first row of Dump.
    */
  class Sentry
  {
    public:
      //! Constructor - add parameters of start class name definition in the stream
      Standard_EXPORT Sentry (Standard_OStream& theOStream, const TCollection_AsciiString& theClassName, const void* thePointer = NULL);

      //! Destructor - add parameters of stop class name definition in the stream
      ~Sentry() { (*myOStream) << stopKey (myClassName); }

    private:
      Standard_OStream* myOStream;
      TCollection_AsciiString myClassName;
  };

  //! Returns separator symbol of Dump information
  static Standard_Character DumpSeparator() { return '\\'; }

  //! Returns separator symbol of class name prefix
  static Standard_CString ClassNameSeparator() { return " ,"; }

  //! Returns default prefix added for each pointer info string
  Standard_EXPORT static TCollection_AsciiString GetPointerPrefix() { return "0x"; }

  //! Convert handle pointer to string value
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo = true);

  //! Convert pointer to string value
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const void* thePointer,
                                                                 const bool isShortInfo = true);

  //! Converts stream into map of values. Values are not empty if the stream contains at least two values.
  //!
  //! The one level stream example: <class_name>key_1\value_1\key_2\value_2</class_name>
  //! In output: theStreamKey equals class_name, theValues contains key_1, value_1, key_2, and value_2.
  //!
  //! Two level stream example: <class_name>key_1\value_1\key_2\value_2\key_3<subclass_name>subclass_key_1\subclass_value1</subclass_name></class_name>
  //! In output: theStreamKey equals class_name, theValues contains key_1, value_1, key_2, and value_2, key_3 and
  //! <subclass_name>subclass_key_1\subclass_value1</subclass_name>.
  //! The last value might be processed later using the same method.
  //!
  //! @param theStream stream value
  //! @param theValues [out] container of split values
  Standard_EXPORT static void Split (const Standard_SStream& theStream,
                                     NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues,
                                     TCollection_AsciiString& theStreamKey);

  //! Unites list of string into one string using the separator
  Standard_EXPORT static TCollection_AsciiString Join (const NCollection_List<TCollection_AsciiString>& theValues,
                                                       const TCollection_AsciiString& theSeparator);

  //! Convert field name into dump text value, removes "&" and "my" prefixes
  //! @param theField a source value 
  //! @param theName [out] an updated name 
  Standard_EXPORT static void DumpFieldToName (const char* theField, const char*& theName);

  //! Converts stream value to string value
  Standard_EXPORT static TCollection_AsciiString ToDumpString (const Standard_SStream& theStream);

  //! Converts stream value to string value
  Standard_EXPORT static TCollection_AsciiString ConvertDumpToText (const Standard_SStream& theStream);

  //! Returns true if the value has bracket key
  static Standard_Boolean HasBracketKey (const TCollection_AsciiString& theSourceValue)
    { return theSourceValue.Search (XMLBracketOpen()) >= 0; }

private:
  //! Converts stream value to string value
  static TCollection_AsciiString startKey (const TCollection_AsciiString& theValue)
    { return XMLBracketOpen() + theValue + XMLBracketClose(); }

  //! Converts stream value to string value
  static TCollection_AsciiString stopKey (const TCollection_AsciiString& theValue)
    { return XMLBracketOpen() + XMLFinishKey() + theValue + XMLBracketClose(); }

  //! Converts stream text into map of values.
  //! @param theStream stream value
  //! @param theValues [out] container of split values
  Standard_EXPORT static void split (const TCollection_AsciiString& theStreamStr,
                                     NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues,
                                     TCollection_AsciiString& theStreamKey);

  //! Splits value into container of value using separator.
  //! It is not recursive, do not split sub class values.
  //!
  //! Two level stream example: key_1\value_1\key_2\value_2\key_3<subclass_name>subclass_key_1\subclass_value1</subclass_name>
  //!  In out, theValues contains: key_1, value_1, key_2, and value_2, key_3 and <subclass_name>subclass_key_1\subclass_value1</subclass_name>.
  //!
  //! @param theValue source stream value
  //! @param theSeparator split separator
  //! @param theValues [out] container of split values
  Standard_EXPORT static void splitValue (const TCollection_AsciiString& theValue,
                                          const TCollection_AsciiString& theSeparator,
                                          NCollection_List<TCollection_AsciiString>& theValues);

  //! Splits value into two sub-strings using Dump keys.
  //! Example:
  //! aString contains "<key>abc</key>defg"
  //! splitDumped(aString) gives theSplitValue = "abc", theTailValue = "defg", theKey = "key"
  //! @param theSourceValue source stream value to split
  //! @param theSplitValue [out] split value
  //! @param theTailValue [out] value out of the key on the left
  //! @param theKey [out] key of split value
  Standard_EXPORT static Standard_Boolean splitDumped (const TCollection_AsciiString& theSourceValue,
                                                       TCollection_AsciiString& theSplitValue,
                                                       TCollection_AsciiString& theTailValue,
                                                       TCollection_AsciiString& theKey);

  //! Splits value that contains a key in form: <key>value</key>. In this case the values are:
  //! theSplitValue = value, theKey = key.
  //! @param theSourceValue source stream value to split
  //! @param theSplitValue [out] split value
  //! @param theKey [out] key of split value
  Standard_EXPORT static Standard_Boolean splitKey (const TCollection_AsciiString& theSourceValue,
                                                    TCollection_AsciiString& theSplitValue,
                                                    TCollection_AsciiString& theKey);

  //! Converts stream value to string value
  static void convertDumpToText (const TCollection_AsciiString& theStreamStr,
                                 const Standard_Integer theLevel,
                                 TCollection_AsciiString& theText);

  //! Stream value open key separator
  static TCollection_AsciiString XMLBracketOpen() { return TCollection_AsciiString ("<"); }

  //! Stream value close key separator
  static TCollection_AsciiString XMLBracketClose() { return TCollection_AsciiString (">"); }

  //! Stream value finish key separator
  static TCollection_AsciiString XMLFinishKey() { return TCollection_AsciiString ("\\"); }

};

#define CLASS_NAME(theClass) #theClass

#define DUMP_FIELD_VALUES(theOStream, theField) \
{ \
  const char* aName = NULL; \
  Standard_Dump::DumpFieldToName (#theField, aName); \
  theOStream << aName << Standard_Dump::DumpSeparator() << theField << Standard_Dump::DumpSeparator(); \
}

#define DUMP_FIELD_VALUES_POINTER(theOStream, theField) \
{ \
  const char* aName = NULL; \
  Standard_Dump::DumpFieldToName (#theField, aName); \
  theOStream << aName << Standard_Dump::DumpSeparator() << Standard_Dump::GetPointerInfo (theField) << Standard_Dump::DumpSeparator(); \
}

#define DUMP_FIELD_VALUES_SUBCLASS(theOStream, theField) \
{ \
  Standard_SStream aFieldStream; \
  if (theField) (theField)->Dump (aFieldStream); \
  const char* aName = NULL; \
  Standard_Dump::DumpFieldToName (#theField, aName); \
  theOStream << aName << Standard_Dump::DumpSeparator() << Standard_Dump::ToDumpString (aFieldStream) << Standard_Dump::DumpSeparator(); \
}

#define DUMP_FIELD_VALUES_PARENT(theOStream, theField) \
{ \
  Standard_SStream aTmpStream; \
  theField::Dump (aTmpStream); \
  const char* aName = NULL; \
  Standard_Dump::DumpFieldToName (#theField, aName); \
  theOStream << aName << Standard_Dump::DumpSeparator() << Standard_Dump::ToDumpString (aTmpStream) << Standard_Dump::DumpSeparator(); \
}

#endif // _Standard_Dump_HeaderFile
