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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

class ScopedXMLString : public Object
{
public:
    ScopedXMLString( const std::string &str)
    {
        _release = true;

        _str = XMLString::transcode( str.c_str() );
    }
    
    ScopedXMLString( const XMLCh* str)
    {
        _release = false;

        _str = const_cast<XMLCh*>( str );
    }

    ~ScopedXMLString()
    {
        if ( _release )
            XMLString::release( &_str);
    }
    
    // will automaticaly work with Xerces parser
    operator XMLCh*() const
    {
        return _str;
    }
    
    operator std::string() const
    {
        auto tmp = XMLString::transcode( _str);
        
        const std::string ret = tmp;
        
        XMLString::release( &tmp);
        return ret;
        
    }
    

    
private:
    bool   _release;
    XMLCh* _str;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

class XMLParser : public Object , private XMLErrorHandler
{
public:
    
    // conveniency typedef
    typedef std::list< std::string > AttributesList;
    
    // use this to access XML element list with
    // methods getNodeListForName() to get a list
    // and getAttributeForName( XMLElement* element ,const std::string &attributeName)
    
    typedef xercesc::DOMElement XMLElement;
    typedef std::vector<XMLElement*> XMLElementList;
    

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
    
    static const std::string getAttributeForName( const XMLElement* element ,const std::string &attributeName);

    /*
     return a list containing the named attributes associated with 'elementName'.
     */
    const AttributesList getAttributesForTag( const std::string &elementName ,const std::string &attributeName ) const;
    
    const XMLElementList getNodeListForName( const std::string &elementName ) const;
    
    //! Return Root Dom Element. Can be nullptr.
    const XMLElement* getRootElement() const;
    

    
    
    /* TODO : create real XMLElement class*/
    
    static const std::string getAttribute( const XMLElement *element , const std::string &attr) /*const*/ ;
    static const std::string getName( const XMLElement *element ) /*const*/ ;
    
    static const XMLElementList getChilds( const XMLElement* element) /*const*/;
    
    static const XMLElement* getChildElementNamed( const XMLElement* element, const std::string &elementName ) /*const*/;

    /**/
private:
    
    XercesDOMParser *_parser;
    bool             _hasErrors;
    
    
    static int s_refCount;
    
    static bool s_initOk;
    
    static bool initializeXML();
    static void deInitializeXML();
};

#endif /* defined(__Broadway_core__XMLParser__) */
