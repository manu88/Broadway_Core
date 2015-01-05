//
//  XMLParser.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 01/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


// Xerces stuff

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif



#include "../Log/Log.h"
#include "../Internal/FileSystem.h"

#include "XMLParser.h"



int XMLParser::s_refCount = 0;

bool XMLParser::s_initOk = false;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

XMLParser::XMLParser() :
_hasErrors ( false )
{
    if (s_refCount == 0 )
        initializeXML();
    
    s_refCount++;
    
    //_parser.setValidationScheme(gValScheme);
    //_parser.setDoNamespaces(gDoNamespaces);
    //_parser.setDoSchema(gDoSchema);
    //_parser.setHandleMultipleImports (true);
    //_parser.setValidationSchemaFullChecking(gSchemaFullChecking);
    
    //_parser.setCreateEntityReferenceNodes(gDoCreate);
    
    _parser = new XercesDOMParser();

    _parser->setErrorHandler(  this );

    
}

XMLParser::~XMLParser()
{
    delete _parser;
    s_refCount--;
    
    if (s_refCount <= 0)
    {
        deInitializeXML();
        s_refCount = 0;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool XMLParser::parseFile( const std::string &filename )
{
    if (!FileSystem::fileExists( filename ))
    {
        Log::log("XML Parser : Error : file '%s' not found!" , filename.c_str() );
        return _hasErrors = false;
    }
    
    bool errorsOccured = false;
    
    try
    {
        _parser->parse( filename.c_str() );
    }
    
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n" ;
//        << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }
    
    catch (const DOMException& e)
    {
//        const unsigned int maxChars = 2047;
      //  XMLCh errText[maxChars + 1];
        
        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << filename << "'\n"
        << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        errorsOccured = true;
    }
    
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }

    
    return _hasErrors = (!errorsOccured && !getSawErrors());
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string XMLParser::getAttributeForTag( const std::string &elementName ,const std::string &attributeName ) const
{
    const DOMDocument* xmlDoc = _parser->getDocument();
    const DOMElement* elementRoot = xmlDoc->getDocumentElement();
    
    const DOMNodeList *list = elementRoot->getElementsByTagName( XMLString::transcode( elementName.c_str() ) );
    
    if (list)
    {
        for (XMLSize_t xx = 0; xx < list->getLength(); ++xx )
        {
            DOMNode* currentNode = list->item(xx);
            
            if( currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                const DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
                return  XMLString::transcode( currentElement->getAttribute(XMLString::transcode( attributeName.c_str() )) ) ;

                
            }


        }
    }
      
    return "undefined";
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/const std::string XMLParser::getAttributeForName( const XMLElement* element ,const std::string &attributeName)
{
    return XMLString::transcode( element->getAttribute(XMLString::transcode( attributeName.c_str() )) );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const XMLParser::AttributesList XMLParser::getAttributesForTag( const std::string &elementName ,const std::string &attributeName ) const
{
    std::list<std::string> ret;
    
    const DOMDocument* xmlDoc = _parser->getDocument();
    const DOMElement* elementRoot = xmlDoc->getDocumentElement();
    
    const DOMNodeList *list = elementRoot->getElementsByTagName( XMLString::transcode( elementName.c_str() ) );
    
    if (list)
    {
        for (XMLSize_t xx = 0; xx < list->getLength(); ++xx )
        {
            DOMNode* currentNode = list->item(xx);
            
            if( currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                const DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
                ret.push_back( XMLString::transcode( currentElement->getAttribute(XMLString::transcode( attributeName.c_str() )) ) );
                
                
            }
            
            
        }
    }

    
    return ret;
    
}

const XMLParser::XMLElementList XMLParser::getNodeListForName( const std::string &elementName ) const
{
    std::vector<XMLElement*> ret;
    
    const DOMDocument* xmlDoc = _parser->getDocument();
    const DOMElement* elementRoot = xmlDoc->getDocumentElement();
    
    const DOMNodeList *list = elementRoot->getElementsByTagName( XMLString::transcode( elementName.c_str() ) );
    
    if (list)
    {
        for (XMLSize_t xx = 0; xx < list->getLength(); ++xx )
        {
            DOMNode* currentNode = list->item(xx);
            
            if( currentNode->getNodeType() && currentNode->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                DOMElement* elem = dynamic_cast< xercesc::DOMElement* >( currentNode );
                ret.push_back( elem );
                
            }
        }
    }
    
    
    return ret;
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool XMLParser::initializeXML()
{
    try
    {
        XMLPlatformUtils::Initialize();
    }
    
    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        Log::log("Error during Xerces-c Initialization. Exception message: %s" , pMsg );
        
        
        XMLString::release(&pMsg);
        return s_initOk = false;
    }
    
    return s_initOk = true;

}

/*static*/ void XMLParser::deInitializeXML()
{
    XMLPlatformUtils::Terminate();
    
    s_initOk = false;

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */