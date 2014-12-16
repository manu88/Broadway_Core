//
//  Object.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <assert.h>
#include <algorithm>

#include "Object.h"
#include "../Config.h"

void ObjectWatcher::removeObject( Object* obj )
{
    
    auto got = std::find( m_objList.begin() , m_objList.end(), obj );
    
    if ( got == m_objList.end() )
        return;
    
    //delete (*got);
    
    m_objList.erase( got );
    
    
}

void ObjectWatcher::removeAndDeleteAllObjects(  )
{
    for( auto obj : m_objList ) //.begin() ; got != m_objList.end() ; got++ )
    {
        if (obj)
        {
            printf("\n delete obj %i of class %s " , obj->num_obj, obj->className.c_str() );
            delete obj;
        }
    }
    
    m_objList.clear();
}


ObjectWatcher::~ObjectWatcher()
{

//    printf("\n num Objects remaining to remove : %li \n" , m_objList.size() );
    
    removeAndDeleteAllObjects();

//    printf("\n num Objects remaining after remove : %li \n" , m_objList.size() );
    
   // assert(Object::s_count == 0);
    DEBUG_ASSERT( m_objList.size() == 0 );

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Object::s_count = 0;
ObjectWatcher Object::s_watcher;


Object::Object()
{
    num_obj = s_count;
    s_count++;
    s_watcher.addObject( this );
    

}

Object::~Object()
{
    s_count--;
    s_watcher.removeObject( this );
    

}