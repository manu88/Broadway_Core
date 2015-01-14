//
//  WebServer.h
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#ifndef ____WebServer__
#define ____WebServer__

#include "web/mongoose.h"

#include "../Internal/Thread.h"
#include "../Internal/AbstractController.h"
#include "../Internal/FileSystem.h"
#include "../Data/ArgumentsArray.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class WebServerDelegate
{
    friend class WebServer;
public:
    virtual ~WebServerDelegate();
    
protected:
    WebServerDelegate();
    virtual std::string getRequest(const std::string &ipAddress , const int port, const std::string &addressPattern, const ArgumentsArray &arguments) = 0;
};



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class WebServer : public Thread,
                  public AbstractController
{
public:
    
    WebServer( int portNum = 8080 );
    
    virtual ~WebServer();
    
    
    void setDelegate(WebServerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    void setPort( int port);
    
    int getPort() const
    {
        return _port;
    }
    
    //! Starts the internal thread.
    bool start();
    bool stop();

    bool setWorkingDirectory( const std::string &dir);

    
    const std::string &getWorkingDirectory() const
    {
        return m_workingDirectory;
    }
    
private:
    
    void settingsChanged();
    
    std::string m_workingDirectory;
    
    /*
        will return s_noFilesContent if not found
     */
    std::string getHtmlFile( const std::string & filename);
    
    // thread's entry point
    void run();
    
    void send_reply(struct mg_connection *conn);
    
    int event(struct mg_connection *conn, enum mg_event ev);
    
    // main static event handler from mongoose' server
    static int ev_handler(struct mg_connection *conn, enum mg_event ev);
    
    WebServerDelegate  *_delegate;
    struct mg_server   *_server;
    int                 _port;
    
    std::string m_defaultContent; // index.html in working dir
    
    static std::string s_noFilesContent;
    static std::string s_404;
};

#endif /* defined(____WebServer__) */
