//
//  JSMachine.cpp
//  JSMachine
//
//  Created by Manuel Deneu on 02/08/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "JSMachine.h"
#include <errno.h>
#include <assert.h>
//#include <sys/stat.h>
#include <string>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <memory>

// for ip stuffs
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

// for dirs
#include <dirent.h>

// time
#include <sys/time.h>
#include <time.h>


#include "TinyJS/TinyJS_Functions.h"
#include "TinyJS/TinyJS_MathFunctions.h"

#include "../Internal/FileSystem.h"
#include "../Data/Database.h"

#include "../Log/Log.h"

#include "JSMachineDelegate.h"




/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

Selector::Selector(const std::string &_signature , JSMachineDelegate* _delegate) :
    signature ( _signature ),
    delegate  ( _delegate  )
{

    className = "Selector";
    
    // on enleve les espaces
//    remove_if(signature.begin(), signature.end(), isspace);
    signature.erase( remove_if(signature.begin(), signature.end(), isspace), signature.end());
    
    std::istringstream mainString( signature );
    
    std::getline(  mainString , identifier , '(' );
    

}

Selector::~Selector()
{

}

void Selector::callFunctionwithVars(CScriptVar *vars)
{
    variables = vars;
    delegate->functionCalled( this );
}


bool Selector::operator==( const Selector& rhs )
{
    return  (signature == rhs.signature) ;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachine::JSMachine() :
    _allowSystemCall ( false   ),
    _delegate        ( nullptr )
{
    className = "JSMachine";
    
    prepareEnvironment();
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachine::~JSMachine()
{
    removeAllRegisteredFunctions();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::prepareEnvironment()
{
    registerFunctions( &_machine );
    registerMathFunctions( &_machine );
    
    _machine.addNative( "function dump()"                      , &js_dumpStack, this);
    _machine.addNative( "function print(text)"                 , &js_print, 0);
    _machine.addNative( "function readDataFile( filePath )"    , &js_readDataFile, 0);
    
    _machine.addNative( "function system( command )"           , &js_system, this );
    _machine.addNative( "function getTime()"                   , &js_getTime,0);


}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::addRegisteredSelectors()
{
    for (auto sel : _registeredSelectors )
    {
        try
        {
            _machine.addNative("function " + sel->signature, js_nativeFunctWithID, sel );
        }
        
        catch (CScriptException *e)
        {
            Log::log("ERROR WHILE REGISTERING: %s", e->text.c_str());
            return;
        }
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::reset()
{
    
    clearStack();
    
    
    
    prepareEnvironment();
    
    addRegisteredSelectors();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::executeBuffer( const std::string &buffer )
{

    
    try
    {

        _machine.execute(buffer);

        return true;
    }
    
    catch ( CScriptException *e )
    {
        Log::log("ERROR: %s", e->text.c_str());
        

    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

CScriptVarLink JSMachine::evaluate( const std::string &buffer)
{
    try
    {
        return _machine.evaluateComplex( buffer );
    }
    
    catch (CScriptException *e)
    {
        Log::log("ERROR: %s", e->text.c_str());
    }
    
    return CScriptVarLink(new CScriptVar()); // undefined
}


std::string JSMachine::evaluateAsString(const std::string &buffer)
{
    std::string ret = "undefined";
    

    
    try
    {
        ret =  _machine.evaluate( buffer );
    }
    
    catch (CScriptException *e)
    {
        Log::log("%s", e->text.c_str());
    }
    
    catch (...)
    {
        
    }
    


    
    return ret;
}

void JSMachine::test()
{
    CScriptVarLink *child = _machine.root->findChild("timer1");
    
    if (child )
    {
        CScriptVar *var =  child->var;
        
        if ( var->isObject() )
        {
            printf("\n is object");
            CScriptVarLink *v =var->firstChild;
            while (v)
            {
                if ((v->name.compare("test")==0))// && v->var->isInt() )
                {
                    
                }
                v = v->nextSibling;
            }
            
            
        }
        printf("\n child name '%s'" , child->name.c_str() );
    }
    else
        printf("\n NO child ");
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Parsers
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::parseScriptFile( const std::string &filename )
{
    Log::log("opening file %s ", filename.c_str() );
    
    if ( !FileSystem::fileExists( filename ) )
    {
        Log::log("Cannot Open file! '%s'", filename.c_str() );
        return false;
    }

    const std::string buffer = FileSystem::getFileText( filename );
    
    executeBuffer( buffer );


    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::importScriptFile( const std::string &filename  )
{
    for ( auto i : _importedHeaders )
    {
        if ( i == filename)

            return true;
    }
    
    _importedHeaders.push_back( filename );
    

    if ( !FileSystem::fileExists( filename ))
    {
        Log::log("Cannot open file! '%s'", filename.c_str() );
        return false;
    }

    const std::string buf = FileSystem::getFileText( filename );
    
    executeBuffer( buf );
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Selectors
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::registerFunctionWithSignature(const std::string &signature )
{

    Selector* sel = new Selector( signature , _delegate);
    
    if ( findSelectorBySignature( sel->signature ) )
    {
        Log::log("selector '%s' already exists" , sel->signature.c_str());
        delete sel;
    }
    
    else
    {
        try
        {
            _machine.addNative("function " + signature, js_nativeFunctWithID, sel );
        }
        
        catch (CScriptException *e)
        {
            Log::log("ERROR WHILE REGISTERING: %s", e->text.c_str());
            return;
        }
        
        _registeredSelectors.push_back(sel);
        

//        Log::log("selector '%s' added" , sel->signature.c_str());
    }
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::findSelectorBySignature( const std::string &signature )
{

    for (auto sel : _registeredSelectors )
    {
        
        if (  signature == sel->signature  )
        {
            return true;
        }
    }


    
    return false;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool JSMachine::respondsToSelector(const std::string &signature)
{
    return findSelectorBySignature( signature );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeRegisteredFunctionWithSignature(const std::string &signature)
{
    for (auto sel : _registeredSelectors )
    {
        if (  signature == sel->signature  )
        {
            delete sel;
        }
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeAllRegisteredFunctions()
{
//    m_registeredSelectors.er
    for (auto it : _registeredSelectors )
    {

        delete it;
    }
    
    _registeredSelectors.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::removeAllImportedScripts()
{
    _importedHeaders.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::clearStack()
{
    removeAllRegisteredFunctions();
    
    removeAllImportedScripts();
    
    _machine.root->unref();
    
    _machine.root = new CScriptVar();
    _machine.root->ref();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Statics callback from js interpreter
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_nativeFunctWithID(CScriptVar *v, void *userdata)
{
    Selector* sel = reinterpret_cast<Selector*>(userdata);

    sel->callFunctionwithVars(v);
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/void JSMachine::js_print(CScriptVar *v, void *userdata)
{
    Log::log("%s", v->getParameter("text")->getString().c_str());
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_dumpStack(CScriptVar *v, void *userdata)
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if (self)
    {
        self->_machine.root->trace(">  ");
        
    }
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_readDataFile(CScriptVar *v, void *userdata)
{
    Database data;
    
    data.parseFile( v->getParameter("filePath")->getString().c_str() , '=');
    
    CScriptVar* vars =  new CScriptVar();
    vars->setArray();
    
    for (int i = 0; i<data.getSize(); i++)
    {
        CScriptVar* pair =  new CScriptVar();
        pair->setArray();
        pair->setArrayIndex(0 , new CScriptVar( data.getItemNameAtIndex( i ) ) ); // item name
        pair->setArrayIndex(1 , new CScriptVar( data.getValueAtIndex( i ).getString() ) ); // item value
        
        vars->setArrayIndex(i , pair);
    }
    
    v->setReturnVar( vars );
    
}

/*static*/ void JSMachine::js_system( CScriptVar *v, void *userdata )
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if ( self->_allowSystemCall )
        system( v->getParameter("command")->getString().c_str() );
    
    else
        Log::log("access to system() forbidden");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_getTime( CScriptVar *v, void *userdata)
{
    char buffer[30];
    struct timeval tv;
    
    time_t curtime;
    
    
    
    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    
    strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
    Log::log("%s%d",buffer,tv.tv_usec);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_importFile( CScriptVar *v, void *userdata )
{
    JSMachine* self = reinterpret_cast<JSMachine*>(userdata);
    
    if ( self )
    {
        // rep de travail
        self->importScriptFile( v->getParameter("file")->getString().c_str() );

    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ VariantList JSMachine::getArgumentsFromJSArray( CScriptVar *vars )
{
    VariantList ret;
    
    const int size  = vars->getArrayLength();
    
    // pas un array, mais une seule variable
    if ( size == 0)
    {
        if (vars->isInt() )
            ret.push_back( vars->getInt() );
        
        else if (vars->isDouble() )
            ret.push_back( vars->getDouble() );
        
        else if (vars->isString() )
            ret.push_back( vars->getString() );
        
    }
    
    else
    {
        for (int i = 0; i<size ; i++)
        {
            CScriptVar *var =vars->getArrayIndex( i );
            
            if (var->isInt() )
                ret.push_back( var->getInt() );
            
            else if (var->isDouble() )
                ret.push_back( var->getDouble() );
            
            else if (var->isString() )
                ret.push_back( var->getString() );
        }
    }
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsStringList( const VariantList &list )
{
    std::ostringstream stream;
    
    int i = 0;

    for (const Variant &val : list )
    {
        if ( i>0)
            stream << " , ";
        

        
        if ( val.isInt() )
        {
            stream << val.getInt();
        }
        else if ( val.isFloat() )
        {

            stream << val.getFloat();
        }
        
        else if ( val.isString() )
        {
            stream << "\"" << val.getString() << "\"" ;
        }
        
        else
        {
            /* No parsing method for this type, to be added ...*/
            DEBUG_ASSERT(false);
        }
        
        i++;

    }
    

    
    return stream.str();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsJSArrayString( const VariantList &list )
{
    std::ostringstream stream;
    
    stream << " [ ";
    int i = 0;
    
    for (const Variant &val : list )
    {
        if ( i>0)
            stream << " , ";
        

        
        if ( val.isInt() )
        {

            stream << val.getInt() ;
        }
        else if ( val.isFloat() )
        {
            stream << val.getFloat();
        }
        
        else if ( val.isString() )
        {
            stream << "\"" << val.getString() << "\"" ;
        }
        
        else
        {
            /* No parsing method for this type, to be added ...*/
            DEBUG_ASSERT(false);
        }
        
        
    }
    
    stream << " ] ";
    
    return stream.str();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
CScriptVar* JSMachine::getArgumentsAsJSArray( const VariantList &list )
{
    CScriptVar *ret = new CScriptVar();
    ret->setArray();
    
    int count = 0;
    
    for (const Variant &val : list )
    {
        
        if ( val.isInt() )
        {
            ret->setArrayIndex(count, new CScriptVar( val.getInt() ) );
            count ++;
        }
        else if ( val.isFloat() )
        {

            ret->setArrayIndex(count, new CScriptVar( val.getFloat() ) );
            count ++;
        }
        
        else if ( val.isString() )
        {

            ret->setArrayIndex(count, new CScriptVar( val.getString() ) );
            count ++;
        }
        
        else
        {
            /* No parsing method for this type, to be added ...*/
            DEBUG_ASSERT(false);
        }
        
        
        
    }
    
    
    return ret;
}




