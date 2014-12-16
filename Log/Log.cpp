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

#define BUFLEN 512
#define NPACK 10


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

Log *Log::s_currentLogger = nullptr; // new LogDummy();

Log::Log()
{
    className = "Log";
}

Log::~Log()
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void Log::useLocalLogger()
{
    setLogger( new LogLocal() );
    
}

/*static*/ void Log::useNoLogger()
{
    setLogger( new LogDummy() );
}

/*static */void Log::useFileLogger( const std::string filepath)
{
    setLogger( new LogFile( filepath.c_str() ) );
}

/*static*/ void Log::useUdpLogger(const std::string ip , int port)
{
    setLogger( new LogUdp(ip.c_str() , port ) );
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

LogFile::LogFile( const char *filename)
{
    m_file = fopen( filename, "a");
    
    if ( !m_file )
    {
        printf("\n ERROR while opening file %s" , filename);
        m_file = nullptr;
    }
    
    fprintf(m_file, "\n############ START ####################" );
}

LogFile::~LogFile()
{
    fprintf(m_file, "\n############ STOP ####################" );    
    fclose( m_file );
}


void LogFile::print(const char * c)
{
    fprintf(m_file, "\n%s" , c);
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



void LogUdp::print(const char * c)
{
    if (m_socket == -1 )
        return ;
    
    char buf[BUFLEN];
    

    
    sprintf(buf, "%s", c);
    
    if ( sendto ( m_socket , buf, BUFLEN, 0,( const struct sockaddr * ) &m_endPoint, (socklen_t) sizeof(m_endPoint) )==-1)
        perror("sendto()");

    
}
