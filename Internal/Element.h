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
    
protected:
    Element();    
    
private:
    int _id;
    
    static int s_elementsCount;
    static int s_elementIDCounter;
    
};

#endif /* defined(____Element__) */
