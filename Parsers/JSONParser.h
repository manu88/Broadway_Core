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
#include <memory>

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
        
        Node( cJSON *node  ):
        _node ( node )
        {
            if ( _node != nullptr)
                _type = ( NodeType ) _node->type;
            else
                _type = Node_NULL;
        }

        ~Node() {}
        
        operator bool() const
        {
            return _node != nullptr;
        }
        
        const std::string getName() const
        {
            return _node->string;
        }
        
        const int getAttribsSize() const
        {
            return getArraySize();
        }
        
        /* ---- typecheck ----  */
        
        NodeType getType() const
        {
            return _type;
        }
        
        bool isObject() const
        {
            return _type == Node_Object;
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
        
        bool isArray() const
        {
            return _type == Node_Array;
        }
        
        /* ----  get vals ---- */
        
        const std::string getString() const
        {
            return _node->valuestring;
        }
        

        /*
            numeric values:
         
            Warning : there's no way to tell if a numeric value type is integer or floatting point.
            If getInt() is called on a float, the returned value will be truncated.
            In doubt, better call Saul, heuu no,  better call getDouble()
         */
        
        double getDouble() const
        {
            return _node->valuedouble;
        }
        
        int getInt() const
        {
            return _node->valueint;
        }
        
        // array
        
        int getArraySize() const
        {
            return cJSON_GetArraySize( _node );
        }
        
        std::unique_ptr<Node> getArrayItem( int index ) const
        {
            return std::auto_ptr<JSONParser::Node>( new Node( cJSON_GetArrayItem(_node, index ) ) );
        }
        
        // child
        
        bool hasChild() const
        {
            return _node->child != nullptr;
        }
        
        std::unique_ptr<Node> getChild() const
        {
            return std::auto_ptr<JSONParser::Node>( new Node( _node->child ) );
        }
        
        // next sibling
        
        bool hasNext() const
        {
            return _node->next != nullptr;
        }
        
        std::unique_ptr<Node> getNext() const
        {
            return std::auto_ptr<JSONParser::Node>( new Node( _node->next ) );
        }
        
        std::unique_ptr<Node> getObjectItem( const std::string &attribName ) const
        {
            return std::auto_ptr<JSONParser::Node>( new Node( cJSON_GetObjectItem( _node, attribName.c_str() ) ) );
        }
        
        
        
    private:

        cJSON *_node;
        NodeType _type;
        
        
    };
    
    /* **** **** **** **** **** **** */
    

    
    JSONParser();
    ~JSONParser();
    
    bool parseFile( const std::string &file );
    
    
    bool traverseNode( cJSON *node);
    
    // return the root of the JSON database. can be NULL!!
    std::unique_ptr<Node> getRootNode() const
    {
        return std::auto_ptr<JSONParser::Node>( new Node( _json ) );
    }
    
    // will return nullptr if not found
    std::unique_ptr<Node> getNode( const std::string &name) const;
    
    
    
private:
    
    cJSON *_json;

};

#endif /* defined(__Broadway_core__JSONParser__) */
