//
//  Object.h
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__Object__
#define __Broadway_test__Object__

#include <iostream>
#include <vector>
#include <set>
#include <mutex>

#include "../Data/Variant.h"




/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
class Object;

class ObjectWatcher
{
public:
    ObjectWatcher()
    {

    }
    
    ~ObjectWatcher();
    
    void addObject( Object* obj )
    {
        m_objList.push_back(obj);
    }
    
    void removeObject( Object* obj );
    void removeAndDeleteAllObjects( );

private:
    
    std::vector<Object*> m_objList;

    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */



class Object
{
    friend class ObjectWatcher;
public:
    
    int getId() const
    {
        return num_obj;
    }
    
    const std::string &getClassName() const
    {
        return className;
    }
    /*
    const Variant performSelectorWithArguments( const std::string &selector , const Variant  &arguments) ;
    const Variant performSelectorWithArguments( const std::string &selector , const Variant  &arguments) const;
    */
    
protected:
    Object();
    // copy ctor is disabled
    Object( const Object&) = delete ;
    
    virtual ~Object();
    
    //! A good way to keep track of objects.
    /* \
       \ className will be printed when an error occur in Object's management,
       \ e.g when ObjectWatcher has to remove any instance.
     */
    std::string className;
    
    int num_obj;
    
    
    //std::mutex m_mutex;
private:
    
    static int s_count;
    
    
    static ObjectWatcher s_watcher;
    
};

#endif /* defined(__Broadway_test__Object__) */
