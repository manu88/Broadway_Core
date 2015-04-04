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
#include <algorithm>
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
    _delegate         ( nullptr     ),
    _server           ( nullptr     ),
    _port             ( portNum     )
{
    className = "Web server";
    
    setInactive();
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

WebServer::~WebServer()
{


    mg_destroy_server( &_server );
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool WebServer::start()
{
       
    
    m_defaultContent = getHtmlFile( m_workingDirectory+ "index.html" );
    
    _server = mg_create_server(this, ev_handler);

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
    if ( _port != port )
    {
        _port = port;
        
        settingsChanged();
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void WebServer::run()
{

   const std::string port = std::to_string( _port );
    
    
    mg_set_option( _server, "listening_port", port.c_str() );
    mg_set_option( _server, "document_root" , m_workingDirectory.c_str() );
    setReady();
    
    Controllers::waitForAllControllersToBeReady();
    

    while (!threadShouldStop())
    {
        mg_poll_server( _server, 1000);


    }
    
    setUnReady();
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


std::string WebServer::getHtmlFile( const std::string & filename)
{
    
    if ( !FileSystem::fileExists( filename ) )
    {
        return s_noFilesContent;
    }
    
    return FileSystem::getFileText( filename );

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


mg_result WebServer::send_reply(struct mg_connection *conn)
{

    const std::string uri(conn->uri);
    

    ///////////////////////////////////////
    // racine 
    if ( uri == "/" )
    {
        const std::string index = getHtmlFile( m_workingDirectory + "index.html" );
        mg_send_data(conn, index.c_str() , ( int ) strlen( index.c_str() ) );

        
        return MG_TRUE;

        
    }
    

    // uri to html file
    if ( uri.find('.') != std::string::npos )
    {

        ///////////////////////////////////////
        // on cherche dans la liste des fichiers dans m_workingDirectory
        
        auto filelist =  FileSystem::getFilesListFromFolder(m_workingDirectory, false); //ArgumentsArray::getArrayFromFolder( m_workingDirectory );
        

        std::string file = "";
        

        for (const std::string &tok : filelist)
        {
            
            if ( uri == ("/" + tok ) )
            {
                file = tok;
                break;
                
            }
            
            
        }
        
        if (file != "")
        {
            const std::string content = getHtmlFile( m_workingDirectory+ file );
            

//            mg_send_data(conn, content.c_str() , ( int ) strlen( content.c_str() ) );
            mg_send_file(conn ,  (m_workingDirectory+ file ).c_str() );

            

            return MG_MORE;
            
        }
        else
        {

            mg_send_data(conn, s_404.c_str() , ( int ) strlen( s_404.c_str() ) );
            return MG_TRUE;
        }
    }
    else
    {
        ///////////////////////////////////////
        // Pas une page web, on envoie la requete au WebServerDelegate
        
        
        std::string content = "";
        if ( conn->query_string )
            content = conn->query_string;
        
        std::string ret = "";
        
        if (_delegate->delegateReadyForController( this ) )
        {

            ret = _delegate->getRequest( conn->remote_ip  , _port , uri.c_str() , *getUriArguments( content));
        }


        
        
/*
        if ( !ret.empty())
        {*/
            mg_printf_data(conn,
                           "%s",
                           ret.c_str()
                           );
        //}

        return MG_TRUE;
    }
   
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


int WebServer::event(struct mg_connection *conn, enum mg_event ev)
{
    if (ev == MG_REQUEST)
    {
        
        
        return send_reply(conn);
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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const std::string WebServer::getDecodedUrl( const std::string &buffer)
{
    std::string ret = buffer;
    StringOperations::replaceAll(ret, "+", " ");
    StringOperations::replaceAll(ret, "%28", "(");
    StringOperations::replaceAll(ret, "%29", ")");
    
    StringOperations::replaceAll(ret, "%3B", ";");
    StringOperations::replaceAll(ret, "%2C", ",");
    
    StringOperations::replaceAll(ret, "%3D", "=");
    StringOperations::replaceAll(ret, "%2B", "+");

    StringOperations::replaceAll(ret, "%5C", "\\");
    StringOperations::replaceAll(ret, "%2F", "/");

    
    StringOperations::replaceAll(ret, "%0D", "\n");
    StringOperations::replaceAll(ret, "%09", "\t");
    
    StringOperations::replaceAll(ret, "%0A", ""); // ?

    StringOperations::replaceAll(ret, "%7B", "{");
    StringOperations::replaceAll(ret, "%7D", "}");
    
    StringOperations::replaceAll(ret, "%5B", "[");
    StringOperations::replaceAll(ret, "%5D", "]");
    
    StringOperations::replaceAll(ret, "%22", "\"");
    
    StringOperations::replaceAll(ret, "%21", "!");
    StringOperations::replaceAll(ret, "%23", "#");
    StringOperations::replaceAll(ret, "%24", "$");
    StringOperations::replaceAll(ret, "%26", "&");
    StringOperations::replaceAll(ret, "%3F", "?");
    StringOperations::replaceAll(ret, "%27", "'");
    StringOperations::replaceAll(ret, "%40", "@");
    StringOperations::replaceAll(ret, "%3A", ":");    
    return ret;
}

/*static*/ std::unique_ptr<Database > WebServer::getUriArguments( const std::string &uri)
{
    
    Database* array = new Database();
    
    if (!uri.empty())
    {
    

    // 1 split uri with '&' delimiter
    
    for (auto tok : StringOperations::split(uri, '&') )
        {
            // split with '='
            auto pair = StringOperations::split(tok, '=');

            std::string val = "";
            
            if ( pair.size() == 2)
                val = pair.at(1) ;
            
            array->setValueForItemName(pair.at(0), Variant( val ) );
            

        }
    }
    
    return std::unique_ptr< Database >( array );
}






