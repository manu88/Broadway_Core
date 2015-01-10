//
//  JSONParser.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 08/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <assert.h>
#include "JSONParser.h"
#include "../Internal/FileSystem.h"
#include "../Log/Log.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSONParser::JSONParser() :
_json ( nullptr )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSONParser::~JSONParser()
{
    if ( _json != nullptr )
        cJSON_Delete( _json );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSONParser::parseFile( const std::string &file )
{
    if ( !FileSystem::fileExists( file ) )
        return false;
    
    bool ret = false;
    
    FILE *f=fopen(file.c_str() ,"rb");
    
    fseek(f,0,SEEK_END);
    
    const long len=ftell(f);
    
    fseek(f,0,SEEK_SET);
    
    char *data=(char*)malloc(len+1);
    
    fread(data,1,len,f);
    
    fclose(f);
    
    // tmp;
    //char *out = nullptr ;
    
    // release previous _json if already parsed
    if ( _json != nullptr )
        cJSON_Delete( _json );
    
    _json =cJSON_Parse( data );
    
    if (! _json )
    {
//        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        
        ret = false;
    }
    
    else /* We're good here */
    {
        ret = true;
        /*
        // temp
        out=cJSON_Print( _json );
        printf("%s\n",out);
        free(out);
        
        if ( _json->type == cJSON_Object )
            printf("\n is object");
        */
        
        //traverseNode( _json );
    }
    
    free( data );
    
    return ret;

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSONParser::traverseNode( cJSON *node)
{
    cJSON *currentNode = node;
    
    while ( currentNode != nullptr)
    {
    
        printf("\n current node name : '%s'" , currentNode->string );
        if ( currentNode->type == cJSON_Object )
        {
            
        }
        
        else if ( currentNode->type == cJSON_Array )
        {
            printf("\n node is Array");
        }
        
        else if ( currentNode->type == cJSON_String )
        {
            printf("\n node is string : '%s' " , currentNode->valuestring );
            
        }
        
        else if ( currentNode->type == cJSON_Number )
        {
            printf("\n node is Number %f"  , currentNode->valuedouble );
        }
        
        else if ( currentNode->type == cJSON_NULL )
        {
            printf("\n node is NULL");
        }
        
        else if ( currentNode->type == cJSON_True )
        {
            printf("\n node is True");
        }
        
        else if ( currentNode->type == cJSON_False )
        {
            printf("\n node is False");
        }
        
        if ( currentNode->child)
        {
            printf("\n got a child ->");
            currentNode = currentNode->child;
        }

        else if ( currentNode->next )
        {
            printf("\n goto  next");
            currentNode = currentNode->next ;
        }
            
        else
            currentNode = nullptr;
    }

    return false;
}


std::unique_ptr<JSONParser::Node> JSONParser::getNode( const std::string &name) const
{
    cJSON* currentNode = _json;
    
    while ( currentNode != nullptr)
    {
        if ( currentNode->string && ( strcmp(name.c_str(), currentNode->string) == 0 ) )
            return std::auto_ptr<JSONParser::Node>( new Node( currentNode ) );
        
        if ( currentNode->child)
        {
            currentNode = currentNode->child;
        }
        
        else if ( currentNode->next )
        {
            currentNode = currentNode->next ;
        }
        
        else
            currentNode = nullptr;
    }
    
    
    return nullptr;
}







