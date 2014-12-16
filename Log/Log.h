//
//  Log.h
//  Broadway_test
//
//  Created by Manuel Deneu on 18/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__Log__
#define __Broadway_test__Log__

#include <iostream>
#include <stdarg.h>

#include <netinet/in.h>

#include "../Internal/Object.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Log : public Object
{
public:
    
    // dirty...
    static void cleanupLogger()
    {
        if (s_currentLogger != nullptr )
        {
            delete s_currentLogger;
            s_currentLogger = nullptr;
        }
    }
    
    
    static void setLogger( Log* logToUse)
    {
        if (s_currentLogger != nullptr )
            delete s_currentLogger;
        
        s_currentLogger = logToUse;
    }
    
    static void useLocalLogger();
    static void useNoLogger();
    static void useFileLogger( const std::string filepath);
    static void useUdpLogger(const std::string ip , int port);
    
    static void log( const char * format , ... )
    {
        if ( s_currentLogger != nullptr )
        {            
            char buffer[512];
            va_list args;
            va_start (args, format);
            vsprintf (buffer,format, args);
            s_currentLogger->print( buffer );
            va_end (args);
        }
    }

    
protected:
    
    virtual void print( const char * c  ) = 0;
    
    // subclasses only
    Log();
    virtual ~Log();
    
private:
    
    static Log *s_currentLogger;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
// does nothing ;-)
class LogDummy : public Log
{
public:
    LogDummy()
    {

    }
    
    ~LogDummy()
    {

    }
    
protected:
    
    virtual void print(const char * c)
    {}
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
// log on screen (terminal)
class LogLocal : public Log
{
public:
    LogLocal()
    {}
    
    ~LogLocal()
    {}
    
protected:
    
    virtual void print(const char * c)
    {
        printf("\nLog:> %s" , c );
    }
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
// Log via local file
class LogFile : public Log
{
public:
    LogFile( const char *filename);
    
    ~LogFile();
    
protected:
    virtual void print(const char * c);
    
private:
    FILE *m_file;
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
// Log via UDP
class LogUdp : public Log
{
public:
    LogUdp(const char*ip , int port);
    
    ~LogUdp();
    
protected:
    
    int m_socket;
    struct sockaddr_in m_endPoint;
    
    
    virtual void print(const char * c);
};

#endif /* defined(__Broadway_test__Log__) */
