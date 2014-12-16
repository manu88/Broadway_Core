//
//  XMLErrorHandler.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 01/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <xercesc/sax/SAXParseException.hpp>
#include "XMLErrorHandler.h"

#include "../../Log/Log.h"

XMLErrorHandler::XMLErrorHandler() :
_gotErrors ( false )
{
    
}

XMLErrorHandler::~XMLErrorHandler()
{
    
}

void XMLErrorHandler::warning(const SAXParseException& toCatch)
{

}
void XMLErrorHandler::error(const SAXParseException& toCatch)
{
    _gotErrors = true;
    
    const char* systemId = XMLString::transcode( toCatch.getSystemId() );
    const char* msg = XMLString::transcode( toCatch.getMessage() );
    Log::log("Error at file '%s' line %i column %i. Message %s." , systemId , toCatch.getLineNumber() ,toCatch.getColumnNumber() , msg );


}

void XMLErrorHandler::fatalError(const SAXParseException& toCatch)
{
    _gotErrors = true;
    
    const char* systemId = XMLString::transcode( toCatch.getSystemId() );
    const char* msg = XMLString::transcode( toCatch.getMessage() );
    Log::log("FATAL Error at file '%s' line %i column %i. Message %s." , systemId , toCatch.getLineNumber() ,toCatch.getColumnNumber() , msg );
}

void XMLErrorHandler::resetErrors()
{
    _gotErrors = false;
}