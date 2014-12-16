//
//  WebServer.cpp
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#include "WebServer.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../Config.h"
#include "../Scheduler/Event.h"



#include "../Log/Log.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


WebServerDelegate::WebServerDelegate()
{

}

WebServerDelegate::~WebServerDelegate()
{
    
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


std::string  WebServer::s_noFilesContent =
"<b> Broadway Web server Error : </b> "
"<p> No Html files found on server, you should do something with HtmlFiles parameter in the config.txt file, "
"and check if 'index.html'  exists in the folder </p> ";

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
std::string  WebServer::s_404 =
"<b> Broadway Web server 404 Error : </b> ";


WebServer::WebServer( int portNum ) :
    Thread             ( "WebServer" ),
    AbstractController ( "webServer" ),

    m_workingDirectory ( "" ),
    m_delegate         ( nullptr     ),
    m_server           ( nullptr     ),
    m_port             ( portNum     )
{
    className = "Web server";
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

WebServer::~WebServer()
{


    mg_destroy_server(&m_server);
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool WebServer::start()
{
       
    
    m_defaultContent = getHtmlFile( m_workingDirectory+ "index.html");
    
    m_server = mg_create_server(this, ev_handler);
    return startThread();
}

bool WebServer::stop()
{
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool WebServer::setWorkingDirectory( const std::string &dir)
{
    if ( !FileSystem::folderExists( dir))
    {
        return false;
    }
    
    m_workingDirectory = dir;
    
    
    
    settingsChanged();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void WebServer::settingsChanged()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void WebServer::setPort( int port)
{
    if ( m_port != port )
    {
        m_port = port;
        
        settingsChanged();
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void WebServer::run()
{

   const std::string port = std::to_string( m_port );
    
    
    mg_set_option(m_server, "listening_port", port.c_str() );
    
    Log::log("Starting Web Server on port %s\n", mg_get_option(m_server, "listening_port"));
    
    setReady();
    
    while (!threadShouldStop())
    {
        mg_poll_server(m_server, 1000);
    }
    
    setUnReady();
    
    Log::log("Web server stopped");
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


std::string WebServer::getHtmlFile( const std::string & filename)
{
    
    if ( !FileSystem::fileExists( filename ) )
    {
        Log::log("Cannot find file '%s' !", filename.c_str() );
        return s_noFilesContent;
    }
    
    const int size = ( int ) FileSystem::getFileSize( filename );
    
    FILE *file = fopen( filename.c_str() , "rb" );
    
    if( !file )
    {
        Log::log("Unable to open file! '%s'", filename.c_str() );
        return s_noFilesContent;
    }
    
    char *buffer = new char[size+1];
    long actualRead = fread(buffer,1,size,file);
    buffer[actualRead]=0;
    buffer[size]=0;
    
    fclose(file);
    
    return buffer;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


void WebServer::send_reply(struct mg_connection *conn)
{

    const std::string uri(conn->uri);
    
    ///////////////////////////////////////
    // racine 
    if ( uri == "/" )
    {
        const std::string index =  getHtmlFile( m_workingDirectory + "index.html");
        mg_send_data(conn, index.c_str() , ( int ) strlen( index.c_str() ) );

        
        return;

        
    }
    

    // uri to html file
    if ( uri.find('.') != std::string::npos )
    {

        ///////////////////////////////////////
        // on cherche dans la liste des fichiers dans m_workingDirectory
        
        auto filelist = ArgumentsArray::getArrayFromFolder( m_workingDirectory );
        
        std::string file = "";
        for (int i =0 ; i<filelist->getSize(); i++)
        {
            
            if ( uri == ("/" + filelist->getValueAtIndex<std::string>(i) ) )
            {
                file = filelist->getValueAtIndex<std::string>(i);
                break;
                
            }
            
            
        }
        
        if (file !="")
        {
            const std::string content = getHtmlFile( m_workingDirectory+ file );
            

            mg_send_data(conn, content.c_str() , ( int ) strlen( content.c_str() ) );

            

            return;
            
        }
        else
        {

            mg_send_data(conn, s_404.c_str() , ( int ) strlen( s_404.c_str() ) );
        }
    }
    else
    {
        ///////////////////////////////////////
        // Pas une page web, on envoie la requete au WebServerDelegate
        
        

        char var1[500];
        ArgumentsArray array;
        
        mg_get_var(conn, "Command", var1, sizeof(var1));
        
        array.addValue<std::string>( var1 );
        array.addValue<std::string>( conn->content );
        
        
        std::string ret = m_delegate->getRequest( conn->remote_ip  , m_port , uri.c_str() , array);
        
        
        if ( !ret.empty())
            mg_printf_data(conn,
                           "%s",
                           ret.c_str()
                           );
        else
            printf("\n no data to send back ...");
    }
    
    

   
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


int WebServer::event(struct mg_connection *conn, enum mg_event ev)
{
    if (ev == MG_REQUEST)
    {
        send_reply(conn);
        
        return MG_TRUE;
    }
    
    else if (ev == MG_AUTH)
    {
        return MG_TRUE;
    }
    
    else
    {
        return MG_FALSE;
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ int WebServer::ev_handler(struct mg_connection *conn, enum mg_event ev)
{
    
    WebServer* self = reinterpret_cast<WebServer*>( conn->server_param ); 
    return self->event(conn , ev);

}


