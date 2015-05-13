//
//  ApplicationBase.h
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __MediaCenter__ApplicationBase__
#define __MediaCenter__ApplicationBase__

#include "../Data/Database.h"
#include "../Scheduler/Scheduler.h"

class ApplicationBase : public SchedulerDelegate
{
    
public:
    virtual ~ApplicationBase();
    
    virtual void eventReceived( const Event &event) = 0;

    
    /* *** *** */
    // Main mechanims
    

    bool start();
    bool sendQuitSignal()
    {
        if (!_isInitialized )
        
            return false;
        
        _shouldQuit = true;
        releaseApp();

        return true;
    }
    
    
    Scheduler &getScheduler() 
    {
        return _scheduler;
    }
    
    
    
    
    /* *** *** */
    // Application configuration base
    
    //! Perform a deep reload of the Database. Everything will be erased prior to read!
    inline bool reloadDatabase()
    {
        return parseConfig();
    }
    
    //! return the database object.
    /* See @class Database. */
    const Database &getDatabase() const
    {
        return _appData;
    }
    
    const Variant* getDataValue( const std::string &name , const Variant &def )
    {
        if ( _appData.itemExists( name ) )
            return _appData.getValueForItemName( name );
        else
            return &def;
        
        
    }
    
    //! Change the application datafile. ReloadDatabase needs to be called in order to parse the new file
    void changeDataFile( const std::string &file )
    {
        _fileConfig = file;
    }
    
    //! get the app datafile's  full path
    const std::string &getDataFile() const
    {
        return _fileConfig;
    }
    
    /* *** *** */
    // quit signal
    

    
    bool shouldQuit()
    {
        releaseApp();
        
        return _shouldQuit;
    }
    
    bool isRunning() const
    {
        return !_shouldQuit;
    }
    
    bool isInitialized() const
    {
        return _isInitialized;
    }

    /* *** *** */
    
    const Database &getConfig() const
    {
        return _appData;
    }
    
    bool saveConfig() const;
    bool reloadConfig();
    
    /* *** *** */
protected:
    
    ApplicationBase( const std::string &fileConfig);
    
    bool sendCommand( const std::string & addressPattern, const VariantList &arguments);
    
    bool parseConfig();
    
    bool initializeApp();
    bool releaseApp();
    
    virtual bool applicationWillStart();
    virtual void applicationDidStart();
    virtual void applicationWillStop();
    virtual void applicationDidStop();
    
    
    void scheduledEventReceived( Event &event);

    
private:
    Database               _appData;
    std::string           _fileConfig;
    
    bool  _isInitialized;
    bool  _shouldQuit;
    bool  _hasQuit;
    
    Scheduler _scheduler;
    int _didStartEvent;
    
};

#endif /* defined(__MediaCenter__ApplicationBase__) */
