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
    _machine.addNative( "function getIP() "                    , &js_getIP,0);
    _machine.addNative( "function readDataFile( filePath )"    , &js_readDataFile, 0);
    _machine.addNative( "function readDir( directoryPath , extensions )"    , &js_readDir,0);
    
    _machine.addNative( "function system( command )"           , &js_system, this );
    _machine.addNative( "function getTime()"                   , &js_getTime,0);

    executeBuffer("var lets_quit = 0; function quit() { lets_quit = 1; }");
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
    
    _importedHeaders.clear();
    
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
/*
CScriptVarLink JSMachine::evaluate( const std::string &buffer)
{
    try
    {
        return m_machine.evaluateComplex( buffer );
    }
    
    catch (CScriptException *e)
    {
        Log::log("ERROR: %s", e->text.c_str());
    }
    
    return CScriptVarLink(new CScriptVar()); // undefined
}
 */

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
    
    const int size = ( int ) FileSystem::getFileSize( filename );
    
    FILE *file = fopen( filename.c_str(), "rb" );

    if( !file )
    {
        Log::log("Unable to open file! '%s'", filename.c_str() );
        return false;
    }
    
    char *buffer = new char[size+1];
    long actualRead = fread(buffer,1,size,file);
    buffer[actualRead]=0;
    buffer[size]=0;
    fclose(file);

    
    executeBuffer( buffer );
    
    /*
    
    try
    {
        m_machine.evaluate("main()");
    }
    
    catch (CScriptException *e)
    {
        Log::log("ERROR: %s\n", e->text.c_str());
    }

*/

    


    
    
    delete[] buffer;
    
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
    
    const int size = ( int ) FileSystem::getFileSize( filename );
    
    FILE *file = fopen( filename.c_str() , "rb" );
    
    if( !file )
    {
        Log::log("Unable to open file! '%s'", filename.c_str() );
        return false;
    }
    
    char *buffer = new char[size+1];
    long actualRead = fread(buffer,1,size,file);
    buffer[actualRead]=0;
    buffer[size]=0;
    fclose(file);
    
    
    executeBuffer( buffer );
    

    delete[] buffer;
    
    return true;
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void JSMachine::run( bool withLiveParser )
{

    if ( withLiveParser )
        Log::log("Interactive mode... Type quit(); to exit ");
    
    while ( evaluateAsString("lets_quit") == "0")
    {
        if ( withLiveParser )
        {
            Log::log("> ");
            char buffer[2048];
            fgets ( buffer, sizeof( buffer ), stdin );
            executeBuffer( buffer );
        }
        else
        {
            usleep(4000);
        }

        
    }
    

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

void JSMachine::clearStack()
{
    
    
    //Log::log("root ref count is %i" ,m_machine.root->getRefs() );
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

/*static*/ void JSMachine::js_getIP(CScriptVar *v, void *userdata)
{
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa          = NULL;
    void * tmpAddrPtr             = NULL;
    
    getifaddrs( &ifAddrStruct );
    
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }
        
        // check it is IP4
        if (ifa->ifa_addr->sa_family == AF_INET)
        {
            // is a valid IP4 Address
            tmpAddrPtr= &( ( struct sockaddr_in * ) ifa->ifa_addr )->sin_addr;

            char addressBuffer[INET_ADDRSTRLEN];
            
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            v->setReturnVar( new CScriptVar( addressBuffer ) );
            
            if (strcmp(ifa->ifa_name, "eth0") == 0)
                v->setReturnVar( new CScriptVar( addressBuffer ) );
            
            //Log::log("%s IP4 Address %s\n", ifa->ifa_name, addressBuffer);
            
        }

        // check it is IP6
        else if (ifa->ifa_addr->sa_family == AF_INET6)
        {
            // is a valid IP6 Address
            tmpAddrPtr= &( ( struct sockaddr_in6 * ) ifa->ifa_addr )->sin6_addr;

            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            
            
            if (strcmp(ifa->ifa_name, "eth0") == 0)
                v->setReturnVar( new CScriptVar( addressBuffer ) );
            
//            Log::log("%s IP6 Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_readDataFile(CScriptVar *v, void *userdata)
{
    Database< std::string > data;
    
    data.parseFile( v->getParameter("filePath")->getString().c_str() , '=');
    
    CScriptVar* vars =  new CScriptVar();
    vars->setArray();
    
    for (int i = 0; i<data.getSize(); i++)
    {
        CScriptVar* pair =  new CScriptVar();
        pair->setArray();
        pair->setArrayIndex(0 , new CScriptVar( data.getItemNameAtIndex( i ) ) ); // item name
        pair->setArrayIndex(1 , new CScriptVar( data.getValueAtIndex<std::string>( i ) ) ); // item value
        
        vars->setArrayIndex(i , pair);
    }
    
    v->setReturnVar( vars );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void JSMachine::js_readDir( CScriptVar *v, void *userdata )
{
    DIR * d;

    
    /* Open the current directory. */
    
    const char* dir_name = v->getParameter("directoryPath")->getString().c_str();
    
    std::vector<std::string> extensions;
    
    bool checkExtentions = true;
    
    
    // une seule extension
    if ( v->getParameter("extensions")->isString()  )
    {
        if (v->getParameter("extensions")->getString() == "*" )
            checkExtentions = false;
        else
            extensions.push_back( v->getParameter("extensions")->getString() );
    }
    
    // plusieurs extensions sous forme d'un array
    else if ( v->getParameter("extensions")->isArray()  )
    {
        for (int i = 0; i<v->getParameter("extensions")->getArrayLength() ; i++ )
        {
            if (v->getParameter("extensions")->getArrayIndex(i)->getString() != "*" )
                extensions.push_back( v->getParameter("extensions")->getArrayIndex(i)->getString() );
        }
    }
    
    /*
    printf("\n Check files for : ");
    
    for (auto e : extensions)
        printf(" '%s' ,",e.c_str() );
    */
    
    d = opendir ( dir_name );

    CScriptVar* vars =  new CScriptVar();
    
    if ( !d )
    {
        Log::log ( "Cannot open directory '%s': %s",
                 dir_name, strerror (errno));
        
        v->setReturnVar( vars ); // undef
        return;
    }
    
    // ok
    
    vars->setArray();
    
    int count = 0;
    
    while ( 1 )
    {
        struct dirent * entry;
        
        entry = readdir (d);
        
        if (! entry)
        {
            break;
        }
        
        if (   ( strcmp( entry->d_name , "."  ) != 0 )
            && ( strcmp( entry->d_name , ".." ) != 0 )
            )
        {
            
            if (checkExtentions)
                for (auto e : extensions)
                {
                    if (has_suffix(entry->d_name, e))
                    {
                        vars->setArrayIndex( count , new CScriptVar( std::string( entry->d_name ) ) );
                        count++;
                    }
                }
            else
            {
                vars->setArrayIndex( count , new CScriptVar( std::string( entry->d_name ) ) );
                count++;
            }

            
        }
    }
    
    // ABC Sort
    
    for ( int i = 0; i < count - 1 ; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            
            if (strcmp( vars->getArrayIndex( i )->getString( ).c_str() , vars->getArrayIndex( j )->getString( ).c_str()  ) > 0)
            {
                const std::string temp =  vars->getArrayIndex( i )->getString( );
                
                vars->getArrayIndex( i )->setString( vars->getArrayIndex( j )->getString( ) );
                
                vars->getArrayIndex( j )->setString( temp );
            }
        }
    }
    
    // end of sort
    
    v->setReturnVar( vars );
    
    /* Close the directory. */
    if ( closedir (d) )
    {
        Log::log ("Could not close '%s': %s",
                 dir_name, strerror (errno));
        return;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

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


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ std::unique_ptr<ArgumentsArray> JSMachine::getArgumentsFromJSArray( CScriptVar *vars )
{
    // TODO: rendre le bordel récursif
    ArgumentsArray* array = new ArgumentsArray();
    
    const int size  = vars->getArrayLength();
    
    // pas un array, mais une seule variable
    if ( size == 0)
    {
        if (vars->isInt() )
            array->addValue< int >( vars->getInt() );
        
        else if (vars->isDouble() )
            array->addValue< double >( vars->getDouble() );
        
        else if (vars->isString() )
            array->addValue< std::string >( vars->getString() );
        
    }
    
    else
    {
        for (int i = 0; i<size ; i++)
        {
            CScriptVar *var =vars->getArrayIndex( i );
            
            if (var->isInt() )
                array->addValue< int >( var->getInt() );
            
            else if (var->isDouble() )
                array->addValue< double >( var->getDouble() );
            
            else if (var->isString() )
                array->addValue< std::string >( var->getString() );
        }
    }
    
    return std::unique_ptr<ArgumentsArray>( array );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsStringList( const ArgumentsArray &array )
{
    std::ostringstream stream;
    

    for (int i =0; i<array.getSize() ; i++)
    {
        if ( i>0)
            stream << " , ";
        
        if (array.isType<int>(i) )
        {
            const int val = array.getValueAtIndex<int>( i );
            stream << val;
        }
        else if (array.isType<float>(i) )
        {
            const float val = array.getValueAtIndex<float>( i );
            stream << val;
        }
        
        else if (array.isType<std::string>(i) )
        {
            const std::string val = array.getValueAtIndex<std::string>( i );
            stream << "\"" << val << "\"" ;
        }
        
        else
        {
            /* No parsing method for this type, to be added ...*/
            DEBUG_ASSERT(false);
        }
        

    }
    

    
    return stream.str();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::string JSMachine::getArgumentsAsJSArrayString( const ArgumentsArray &array )
{
    std::ostringstream stream;
    
    stream << " [ ";
    for (int i =0; i<array.getSize() ; i++)
    {
        if ( i>0)
            stream << " , ";
        if (array.isType<int>(i) )
        {
            const int val = array.getValueAtIndex<int>( i );
            stream << val;
        }
        else if (array.isType<float>(i) )
        {
            const float val = array.getValueAtIndex<float>( i );
            stream << val;
        }
        
        else if (array.isType<std::string>(i) )
        {
            const std::string val = array.getValueAtIndex<std::string>( i );
            stream << "\"" << val << "\"" ;
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
CScriptVar* JSMachine::getArgumentsAsJSArray( const ArgumentsArray &array )
{
    CScriptVar *ret = new CScriptVar();
    ret->setArray();
    
    int count = 0;
    for (int i =0; i< array.getSize() ; i++)
    {
        if ( array.isType<int>(i) )
        {
            const int val = array.getValueAtIndex<int>( i );
            ret->setArrayIndex(count, new CScriptVar( val ) );
            count ++;
        }
        else if ( array.isType<float>(i) )
        {
            const float val = array.getValueAtIndex<float>( i );
            ret->setArrayIndex(count, new CScriptVar( val ) );
            count ++;
        }
        
        else if ( array.isType<std::string>(i) )
        {
            const std::string val = array.getValueAtIndex<std::string>( i );
            ret->setArrayIndex(count, new CScriptVar( val ) );
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




