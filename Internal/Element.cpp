//
//  Element.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#include "Element.h"


int Element::s_elementsCount  = 0;
 int Element::s_elementIDCounter = 0;


Element::Element() :
_id( s_elementIDCounter++ )
{
    
    className = "Element";    
    s_elementsCount++;
}

Element::~Element()
{
    s_elementsCount--;
    
    if ( s_elementsCount == 0 )
        s_elementIDCounter = 0;
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Element::performSelectorWithArguments( const std::string &selector , const Variant  &arguments)
{
    
    return Variant();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

const Variant Element::performSelectorWithArguments( const std::string &selector , const Variant  &arguments) const
{
    if ( selector == "getElementId")
        return getElementId();
    
    return Variant();
}

#endif /*ENABLE_ELEMENT_SELECTOR*/





