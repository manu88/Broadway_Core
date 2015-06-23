//
//  Element.h
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#ifndef ____Element__
#define ____Element__

#include <algorithm>
#include "Object.h"
#include "../Log/Log.h"

//! Element
/********************************************//**
*
*   Base class for any type of Element
*   Its main goal is to provide an _unique_ ID to Elements such as GXElement (GXVideo , GXImage, etc.) so
*   that each instance can be retreived easily.
*
*                                               
*                                                   
*   \sa GpioEvent OscEvent
*
*
***********************************************/

class Element : public virtual Object
{
public:

    virtual ~Element();
    
    //! Main purpose of this base class. Will return the Element'sID, witch is garranted to never change.
    /*!
     \return  the _unique_ element's ID.
     */
    int getElementId() const
    {
        return _id;
    }

    //! Returns the highest Element ID.
    /*!
     \return  the highest Element ID.
     */
    static int getMaxID()
    {
        return s_elementIDCounter;
    }
    

    void setElementName(const std::string &name)
    {
        _elementName = name;
    }
    
    const std::string &getElementName() const
    {
        return _elementName;
    }
    
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
#ifdef ENABLE_ELEMENT_SELECTOR    
    
    virtual const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments, bool *validCall = nullptr ) ;
    
    static Element* getElementByName(const std::string &elementName )
    {
        const auto it = std::find_if(s_elementsList.begin(), s_elementsList.end(), FindElementByName( elementName ));
        
        if( it != s_elementsList.end())
            return *it;
        
        return nullptr;
    }
    

    
    static const Variant performSelectorOnElement( const std::string &elementName ,
                                                   const std::string &selector ,
                                                   const VariantList  &arguments ,
                                                   bool *validCall = nullptr ,
                                                   bool *validElement = nullptr
                                                  )
    {
        auto* element = getElementByName( elementName);
        if( element != nullptr )
        {
            if (validElement)
                *validElement = true;
            return element->performSelectorWithArguments( selector , arguments , validCall);
        }
        
        else if (validElement)
            *validElement = false;

        return Variant::null();
    }

    static long getElementsCount()
    {
        return s_elementsList.size();
    }
    
    static std::vector< Element*> &getElementList()
    {
        return s_elementsList;
    }
    
    
    static void dumpElement()
    {
        for (const Element *e : Element::getElementList() )
        {
            
            Log::log("Element Id %i", e->getElementId() );
            Log::log("\tName '%s'",e->getElementName().c_str() );
            Log::log("\tClass '%s' ", e->className.c_str() );
            
            
        }
    }
#endif
    
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
protected:
    Element();    
    
private:
    int _id;
    std::string _elementName;
    
    static int s_elementsCount;
    static int s_elementIDCounter;
    
    #ifdef ENABLE_ELEMENT_SELECTOR
    
        static std::vector< Element*> s_elementsList;
    
        struct FindElementByName
        {
            std::string elementName;
            
            FindElementByName( const std::string &name) : elementName( name){}
            
            bool operator()(Element *element) const
            {
                return elementName == element->_elementName;
            }
        };
    
    #endif
    
};

#endif /* defined(____Element__) */
