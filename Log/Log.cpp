//
//  Log.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 18/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "Log.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include <unistd.h>

#include "../Scheduler/TimeDefs.h"

#define BUFLEN 512
#define NPACK 10


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
std::vector<Log*> Log::s_logList = std::vector<Log*>();


Log::Log() 
{
    className = "Log";
}

Log::~Log()
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Log::addLocalLogger()
{
    addLogger( new LogLocal() );
    
}

/*static*/ void Log::useNoLogger()
{
    cleanupLogger();
}

/*static */void Log::addFileLogger( const std::string filepath)
{
    addLogger( new LogFile( filepath.c_str() ) );
}

/*static*/ void Log::addUdpLogger(const std::string ip , int port)
{
    addLogger( new LogUdp(ip.c_str() , port ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Log::addLogger( Log* logToUse)
{
    s_logList.push_back( logToUse );
}

/*static*/void Log::removeLogger( Log* logToUse )
{
    auto found = std::find(s_logList.begin(), s_logList.end(), logToUse);
    
    if ( found != s_logList.end() )
    {
        s_logList.erase( found);
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Log::log( const char * format , ... )
{
    char buffer[ BUFFER_SIZE ];
    va_list args;
    va_start (args, format);
    vsprintf (buffer,format, args);
    
    for (const Log* log : s_logList )
    {
        log->print( buffer );
    }
    
    
    va_end (args);

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Log::cleanupLogger()
{
    for (auto log : s_logList)
        delete log;
    
    s_logList.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

LogFile::LogFile( const std::string &filename ):
_filepath( filename )
{
    FILE *file = fopen( _filepath.c_str(), "a");
    
    if ( !file )
    {
        printf("\n ERROR while opening file %s" , _filepath.c_str() );
        _fileExists = false;

    }
    else
    {
        _fileExists = true;
        
        fprintf( file, "\n############ START ####################" );
        
        Date now;
        
        fprintf( file, "\nDate : %s",now.toString().c_str() );
        fclose( file );
    }
}

LogFile::~LogFile()
{
    FILE *file = fopen( _filepath.c_str() , "a");
    
    if (file)
    {
        fprintf( file, "\n############ STOP ####################" );
        fclose( file );
    }
}


void LogFile::print(const char * c) const
{
    FILE *file = fopen( _filepath.c_str() , "a");
    
    if (file)
    {
        fprintf( file, "\n%s" , c);
        fclose( file );
    }

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

LogUdp::LogUdp(const char*ip , int port)
{


    if ( ( m_socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) ==-1 )
    {
        perror("socket");
        m_socket = -1;
        return;
    }
    
    memset((char *) &m_endPoint, 0, sizeof( m_endPoint ) );
    
    m_endPoint.sin_family = AF_INET;
    m_endPoint.sin_port = htons(  port );
    
    if ( inet_aton( ip , &m_endPoint.sin_addr)==0)
    {
        printf( "\n error while connecting to %s on port %i" , ip , port );
        m_socket = -1;
        return;
        
    }
    
    printf("\n succesfully connected to %s on port %i" , ip , port );
}

LogUdp::~LogUdp()
{
    close( m_socket );
}



void LogUdp::print(const char * c) const
{
    if (m_socket == -1 )
        return ;
    
    char buf[BUFLEN];
    

    
    sprintf(buf, "%s", c);
    
    if ( sendto ( m_socket , buf, BUFLEN, 0,( const struct sockaddr * ) &m_endPoint, (socklen_t) sizeof(m_endPoint) )==-1)
        perror("sendto()");

    
}
