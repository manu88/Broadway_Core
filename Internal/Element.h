//
//  Element.h
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#ifndef ____Element__
#define ____Element__

#include "Object.h"

//! Element
/********************************************//**
*
*   Base class for any type of Element
*   It's mainly goal is to provide an _unique_ ID to Elements such as GXElement (GXVideo , GXImage, etc.) so 
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
    
#ifdef ENABLE_ELEMENT_SELECTOR
    void setElementName(const std::string &name)
    {
        _elementName = name;
    }
    
    const std::string &getElementName() const
    {
        return _elementName;
    }
    
    virtual const Variant performSelectorWithArguments( const std::string &selector , const Variant  &arguments) ;
    
    static Element* getElementByName(const std::string &elementName )
    {
        const auto it = std::find_if(s_elementsList.begin(), s_elementsList.end(), FindElementByName( elementName ));
        
        if( it != s_elementsList.end())
            return *it;
        
        return nullptr;
    }
    
    struct FindElementByName
    {
        std::string elementName;
        FindElementByName( const std::string &name) : elementName( name)
        {}
        bool operator()(Element *element) const
        {
            return elementName == element->_elementName;
        }
    };
    static const Variant performSelectorOnElement( const std::string &elementName , const std::string &selector , const Variant  &arguments)
    {
        auto* element = getElementByName( elementName);
        if( element != nullptr )
            return element->performSelectorWithArguments( selector , arguments);

        return Variant::null();
    }

    static long getElementsCount()
    {
        return s_elementsList.size();
    }
#endif
    
protected:
    Element();    
    
private:
    int _id;
    
    static int s_elementsCount;
    static int s_elementIDCounter;
    
    #ifdef ENABLE_ELEMENT_SELECTOR
    
    std::string _elementName;
    static std::vector< Element*> s_elementsList;
    #endif
    
};

#endif /* defined(____Element__) */
