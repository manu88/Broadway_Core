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

class ApplicationBase
{
    
public:
    virtual ~ApplicationBase();
    
    /* *** *** */
    // Main mechanims
    
    bool initializeApp();
    bool start();
    bool releaseApp();
    
    
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
    
    void sendQuitSignal()
    {
        _shouldQuit = true;
    }
    
    bool shouldQuit() const
    {
        return _shouldQuit;
    }
    
    bool isRunning() const
    {
        return !_shouldQuit;
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
    
    bool parseConfig();
    
private:
    Database               _appData;
    std::string           _fileConfig;
    
    bool  _shouldQuit;
    bool  _hasQuit;
    
};

#endif /* defined(__MediaCenter__ApplicationBase__) */
