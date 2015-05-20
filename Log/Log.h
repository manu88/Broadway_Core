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
#include <vector>

#include <netinet/in.h>

#include "../Internal/Object.h"

/*
 
 You can use as many loggers as you want, in the same time.
 */

/*
typedef enum
{
    LOG_NONE            = 0,
    
    // will log on standard outupt
    LOG_LOCAL           = ( 1 << 0 ),
    
    // will log on a text file
    LOG_FILE            = ( 1 << 1 ),
    
    // will log using upd socket to (address/port)
    LOG_UDP             = ( 1 << 2 ),
    
    // wil log on a special html file
    // accessible if the Web server is running
    LOG_WEB             = ( 1 << 3 ),
    
    LOG_PD              = ( 1 << 4 )
    
}
 

LogMode;
*/


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Log : public Object
{
public:
    
    enum { BUFFER_SIZE = 512};
    
    /* C style logging, like printf() */
    static void log( const char * format , ... );

    /* TODO : C++ style logging, with streams */

    static void cleanupLogger();
        
    /*
     Conveniencies function to use predefined loggers. 
     To pass a spectific one, use  addLogger() below.
     */

    // will clear add call delete on every log previously registered.
    static void useNoLogger();
    
    static void addLocalLogger();
    static void addFileLogger( const std::string filepath);
    static void addUdpLogger(const std::string ip , int port);

    static void addLogger( Log* logToUse );
    static void removeLogger( Log* logToUse );
    
    static int getNumLoggers()
    {
        return (int) s_logList.size();
    }
    
    static const Log* at( int index)
    {
        return s_logList.at( index );
    }
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    typedef typename std::vector<Log*>::iterator iterator;
    typedef typename std::vector<Log*>::const_iterator const_iterator;
    
 //   static iterator begin() {return s_logList.begin();}
    static const_iterator begin()  {return s_logList.begin();}
    static const_iterator cbegin() {return s_logList.cbegin();}
   // static iterator end() {return s_logList.end();}
    static const_iterator end()  {return s_logList.end();}
    static const_iterator cend()  {return s_logList.cend();}
    
protected:
    // subclasses only
    Log();
    virtual ~Log();
    
    virtual void print( const char * c  ) const = 0;


private:

    static std::vector<Log*> s_logList;
    
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
    
    void print(const char * c) const
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
    
    void print(const char * c) const
    {
        printf("\nLog:>%s" , c );
    }
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
// Log via local file
class LogFile : public Log
{
public:
    LogFile( const std::string &filename);
    
    ~LogFile();
    
protected:
    void print(const char * c) const;
    
private:
    std::string _filepath;
    bool        _fileExists;
//    FILE *m_file;
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
    
    
    void print(const char * c) const;
};

#endif /* defined(__Broadway_test__Log__) */
