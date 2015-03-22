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
    /*
    
    bool configValueExists( const std::string &item ) const
    {
        return _appData.itemExists( item);
    }
    
    const std::string getConfigValue( const std::string &item ) const
    {
        return _appData.getValueForItemNameAsString( item );
    }
    
*/
    
    
protected:
    
    ApplicationBase();
    
private:
    Database _appData;
    std::string           _fileConfig;
    
};

#endif /* defined(__MediaCenter__ApplicationBase__) */
