//
//  XMLParser.h
//  Broadway_core
//
//  Created by Manuel Deneu on 01/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__XMLParser__
#define __Broadway_core__XMLParser__

#include <iostream>
#include <list>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include "XMLImp/XMLErrorHandler.h"


#include "../Internal/Object.h"



XERCES_CPP_NAMESPACE_USE // beurk...


class XMLParser : public Object , private XMLErrorHandler
{
public:
    
    // conveniency typedef
    typedef std::list< std::string > AttributesList;

    XMLParser();
    ~XMLParser();
    
    bool hasErrors() const
    {
        return _hasErrors;
    }
    
    bool parseFile( const std::string &filename );
    
    /*
        return the named attribute associated with 'elementName'
        Note : will return the first node found. If you want to get multiple elements with the same name,
        use getAttributesForTag() bellow.
     */
    const std::string getAttributeForTag( const std::string &elementName ,const std::string &attributeName ) const;

    /*
     return a list containing the named attributes associated with 'elementName'.
     */
    const AttributesList getAttributesForTag( const std::string &elementName ,const std::string &attributeName ) const;
    
    
    

    
private:
    
    XercesDOMParser *_parser;
    bool             _hasErrors;
    
    
    static int s_refCount;
    
    static bool s_initOk;
    
    static bool initializeXML();
    static void deInitializeXML();
};

#endif /* defined(__Broadway_core__XMLParser__) */
