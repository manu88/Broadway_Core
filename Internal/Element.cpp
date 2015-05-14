//
//  Element.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#include "Element.h"

#ifdef ENABLE_ELEMENT_SELECTOR
std::vector< Element*> Element::s_elementsList = std::vector< Element*>();


#endif

int Element::s_elementsCount  = 0;
int Element::s_elementIDCounter = 0;


Element::Element() :
_id( s_elementIDCounter++ )
{
    
    className = "Element";
    
    s_elementsCount++;
    
#ifdef ENABLE_ELEMENT_SELECTOR
    s_elementsList.push_back( this );

    _elementName = "element"+ std::to_string(_id);
    
#endif
}

Element::~Element()
{
#ifdef ENABLE_ELEMENT_SELECTOR
    s_elementsList.erase(std::find( s_elementsList.begin(), s_elementsList.end(), this) );
#endif
    
    s_elementsCount--;
    
    if ( s_elementsCount == 0 )
        s_elementIDCounter = 0;
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant Element::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall /*= nullptr*/)
{
    if( validCall )
        *validCall = true;
    
    if ( selector == "getElementId")
        return getElementId();
    
    else if ( selector == "getElementName")
        return getElementName();
    
    else if ( selector == "setElementName")
    {
        setElementName( arguments.at(0).getString() );
    }
    
    else if ( selector == "getClassName")
        return className;
    
    if( validCall )
        *validCall = false;
    
    
    return Variant::null();
}


#endif /*ENABLE_ELEMENT_SELECTOR*/





