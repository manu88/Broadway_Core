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
    
    /* Application configuration base */
    const Database &getDatabase() const
    {
        return _appData;
    }
    
    bool initializeApp();
    bool start();
    bool releaseApp();
    
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
