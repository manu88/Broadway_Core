//
//  JSONParser.h
//  Broadway_core
//
//  Created by Manuel Deneu on 08/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__JSONParser__
#define __Broadway_core__JSONParser__

#include <iostream>

#include "../Internal/Object.h"

#include "cJSON/cJSON.h"



class JSONParser : public Object
{
public:    
    
    typedef enum
    {
        Node_False = 0,
        Node_True = 1,
        Node_NULL = 2,
        Node_Number = 3,
        Node_String = 4,
        Node_Array = 5,
        Node_Object = 6,
        
    } NodeType;
    
    /* **** **** **** **** **** **** */    
    
    class Node : public Object
    {
        friend class JSONParser;
        
    public:
        
        Node( cJSON *node = nullptr ):
        _node ( node )
        {
            if ( _node != nullptr)
                _type = ( NodeType ) _node->type;
            else
                _type = Node_NULL;
        }
        
        Node( const Node &other):
        _node ( other._node ),
        _type ( other._type )
        {}
        
        ~Node() {}
        
        operator bool() const
        {
            return _node != nullptr;
        }
        
        /* typecheck */
        
        NodeType getType() const
        {
            return _type;
        }
        
        bool isString() const
        {
            return _type == Node_String;
        }
        
        bool isNumber() const
        {
            return _type == Node_Number;
        }
        
        bool isNull() const
        {
            return _type == Node_NULL;
        }
        
        /* get vals */
        
        const std::string getString() const
        {
            return _node->valuestring;
        }
        
        double getDouble() const
        {
            return _node->valuedouble;
        }
        
    private:

        
        // attribs
        cJSON *_node;
        NodeType _type;
        
    };
    
    /* **** **** **** **** **** **** */
    

    
    JSONParser();
    ~JSONParser();
    
    bool parseFile( const std::string &file );
    
    
    bool traverseNode( cJSON *node);
    
    const Node getRoot() const
    {
        return Node(_json);
    }
    
    // will return nullptr if not found
    const Node getNode( const std::string &name) const;
    
    
    
private:
    
    cJSON *_json;

};

#endif /* defined(__Broadway_core__JSONParser__) */
