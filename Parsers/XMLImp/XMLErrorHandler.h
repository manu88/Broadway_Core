//
//  XMLErrorHandler.h
//  Broadway_core
//
//  Created by Manuel Deneu on 01/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__XMLErrorHandler__
#define __Broadway_core__XMLErrorHandler__

#include <iostream>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include "XMLErrorHandler.h"

XERCES_CPP_NAMESPACE_USE




class XMLErrorHandler : public ErrorHandler
{
public:
    XMLErrorHandler();
    ~XMLErrorHandler();
    
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();
    
    bool getSawErrors() const
    {
        return _gotErrors;
    }
    
private:
    bool _gotErrors;

};

#endif /* defined(__Broadway_core__XMLErrorHandler__) */
