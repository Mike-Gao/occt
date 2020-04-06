// File:	ExpToCasExe.cxx
// Created:	Mon Nov  1 12:50:27 1999
// Author:	Andrey BETENEV
//		<abv@nordox.nnov.matra-dtv.fr>

#include <string.h>
#include <ctype.h>
#include <time.h>

#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>

#include <Express_Schema.hxx>
#include <Express_Item.hxx>

#pragma warning(push)
#pragma warning (disable:4267)
#pragma warning (disable:4706)

Handle(Express_Schema) ec_parse ( FILE *fin ); // interface to parser

// Load list of (class name, package name) from the file
// Package names and optional mark flag are set to items in the schema
static Standard_Boolean LoadList (const char file[], const Handle(Express_Schema) schema,
				  const Standard_Boolean mark)
{
  FILE *fd_pack = fopen ( file, "r" );
  if ( ! fd_pack ) {
    std::cout << "Warning: cannot open " << file << std::endl;
    return Standard_False;
  }

  std::cout << "Loading " << file << "..";
  char string[200];
  while ( fgets ( string, 199, fd_pack ) ) {
    char *s = string, *name=0;
    Standard_Integer ind = 0;
    while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
    if ( ! *s ) continue;
    name = s;
    name[ind] = '\0';
    Handle(Express_Item) item = schema->Item ( name, Standard_True );
    if ( item.IsNull() ) continue; // silently skip any class name not in schema
    s += ind + 1;
    while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
    if ( ! *s ) continue;
    s[ind] = '\0';
    Handle(TCollection_HAsciiString) pack = new TCollection_HAsciiString ( s );
    if ( ! item->GetPackageName().IsNull() && item->GetPackageName()->IsDifferent ( pack ) ) {
      std::cout << "\nWarning: Package is redefined for item " << name << std::endl;
    }
    item->SetPackageName ( pack );
    if ( mark ) item->SetMark ( Standard_True );
    //gka ----------------------------------------------
    //filling additional field shortname for item  
    //and setting flags marked presence of methods Check and FillShared.
    //fields in the text file situate in the next way:
    //name package_name  shortname check_flag(0 or 1) fillshared_flag(0 or 1) 
    
    s += ind + 1;
    Handle(TCollection_HAsciiString) shortname = new TCollection_HAsciiString;
    Standard_Boolean hasShortName = Standard_False;
    while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
    if ( ! *s ) continue;
    s[ind] = '\0';
    //set CheckFlag or shortname
    if(*s == '0' || *s == '1') {
      Standard_Boolean hasCheck = (*s == '0' ? Standard_False : Standard_True);
      item->SetCheckFlag(hasCheck);
    }
    else {
      if(*s != '-')
        shortname->SetValue(1,s);
      if(shortname->Length() >0)
      item->SetShortName(shortname);
      hasShortName = Standard_True;
    }
    
    s += ind + 1;
    //set FillSharedFlag or shortname
    while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
    if ( ! *s ) continue;
    s[ind] = '\0';
    if(!hasShortName) {
      if(*s == '0' || *s == '1') {
        Standard_Boolean hasFillShared = (*s == '0' ? Standard_False : Standard_True);
        item->SetFillSharedFlag(hasFillShared);
      }
      else {
        if(*s != '-')
          shortname->SetValue(1,s);
        if(shortname->Length() >0)
          item->SetShortName(shortname);
        hasShortName = Standard_True;
      }
      
      s += ind + 1;
      //set short name
      while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
      if ( ! *s ) continue;
      s[ind] = '\0';
    }
    if(!hasShortName) {
      if(*s != '-')
        shortname->SetValue(1,s);
      if(shortname->Length() >0)
        item->SetShortName(shortname);
      
      s += ind + 1;
      //set Category
      while ( *s && ! ( ind = strcspn ( s, " \t\r\n" ) ) ) s++;
      if ( ! *s ) continue;
      s[ind] = '\0';
    }
    Handle(TCollection_HAsciiString) category = new TCollection_HAsciiString;
    category->SetValue(1,s);
    if(category->Length()>0)
      item->SetCategory(category);
    //-----------------------------------------------------
    
    
  }
  std::cout << " Done" << std::endl;
  
  fclose ( fd_pack );
  return Standard_True;
}

// MAIN program
Standard_Integer main ( const Standard_Integer argc, const char *argv[] )
{
  if ( argc <2 ) {
    std::cout << "EXPRESS -> CASCADE/XSTEP classes generator 3.0" << std::endl;
    std::cout << "Use: ExpToCas <schema.exp> [<create.lst> [<packaging.lst> [start_index]]]" << std::endl;
    std::cout << "Where: " << std::endl;
    std::cout << "- schema.exp is a file with EXPRESS schema " << std::endl;
    std::cout << "- create.lst is a file with list of types to generate (all if none)" << std::endl;
    std::cout << "  (or \"-\" for creating all entities in the schema)" << std::endl;
    std::cout << "- packaging.lst is a file with classes distribution per package" << std::endl;
    std::cout << "  in the form of the list (one item per line) \"<TypeName> <Package>\"" << std::endl;
    std::cout << "  If package not defined for some type, \"StepStep\" assumed" << std::endl;
    std::cout << "- start_index - a first number for auxiliary generated files with data" << std::endl;
    std::cout << "  to copy into StepAP214_Protocol.cxx, RWStepAP214_GeneralModule.cxx and RWStepAP214_ReadWriteModule.cxx" << std::endl;
    return 0;
  }
  
  //=================================
  // Step 1: parsing EXPRESS file
  // open schema file
  FILE *fin = fopen ( argv[1], "rt" );
  if ( ! fin ) {
    std::cout << "Error: Cannot open " << argv[1] << std::endl;
    return 0;
  }
  
  // parse
  std::cout << "Starting parsing " << argv[1] << std::endl;
  Handle(Express_Schema) schema = ec_parse ( fin );
  fclose ( fin );

  if ( schema.IsNull() ) {
    std::cout << "Error: Parsing finished with no result" << std::endl;
    return 0;
  }
  else std::cout << "Schema " << schema->Name()->ToCString() << " successfully parsed" << std::endl;

  //=================================
  // Step 2: Prepare data for creating classes
  
  // load packaging information
  if ( argc >3 ) {
    if ( ! LoadList ( argv[3], schema, Standard_False ) ) return 0;
  }
  
  // load list of classes to generate
  TColStd_SequenceOfInteger seq;
  if ( argc >2 ) {
    if ( argv[2][0] =='-' ) { // set mark for all items
      for ( Standard_Integer num=1; num <= schema->NbItems(); num++ )
	schema->Item ( num )->SetMark(Standard_True);
    }
    else if ( ! LoadList ( argv[2], schema, Standard_True ) ) return 0;
  }

  Standard_Integer anIndex = -1;
  if (argc > 4)
    anIndex = atoi(argv[4]);

  //=================================
  // Step 3: Iterate by items and generate classes

  std::cout << "Total " << schema->NbItems() << " items" << std::endl;
  Standard_Boolean done = Standard_False;
  Standard_Integer nbgen = 0;
  Express_Item::SetIndex(anIndex);
  do {
    done = Standard_False;
    for ( Standard_Integer num=1; num <= schema->NbItems(); num++ ) {
      if ( ! schema->Item ( num )->GetMark() ) continue;
      nbgen += schema->Item ( num )->Generate();
      done = Standard_True;
//      std::cout << num << ": " << schema->Item(num)->CPPName()->ToCString() << std::endl;
    }
  } while ( done );
//  std::cout << "Finished; total " << nbgen << " classes generated" << std::endl;
  return 1;
}

#pragma warning(pop)
