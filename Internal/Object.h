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
//#include <unordered_set>
#include <vector>
#include <set>
#include <mutex>

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
    
protected:
    Object();
    // copy ctor is disabled
    Object( const Object&) = delete ;
    
    virtual ~Object();
    
    std::string className;
    int num_obj;
    
    
    //std::mutex m_mutex;
private:
    
    static int s_count;
    
    
    static ObjectWatcher s_watcher;
    
};

#endif /* defined(__Broadway_test__Object__) */
