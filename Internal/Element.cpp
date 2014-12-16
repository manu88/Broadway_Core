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
    m_id( s_elementIDCounter++ )
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