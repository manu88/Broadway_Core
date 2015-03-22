//
//  Database.cpp
//  
//
//  Created by Manuel Deneu on 06/09/14.
//
//

#include "Database.h"


Database::~Database()
{
    for( auto val : _dataList )
        delete val.second;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Database::parseFile(const std::string &fileName , const char delim)
{
    if (!FileSystem::fileExists( fileName))
        return false;
    
    FILE* file = fopen(fileName.c_str() , "r");
    
    if (!file)
        return false;
    
    char line[256];
    
    int count = 0;
    while (fgets(line, sizeof(line), file))
    {
        std::stringstream ss(line);
        std::string item = "";
        std::string val = "";
        
        if ( strncmp(line, "#", 1 ) != 0 ) // skipped if begin with '#' ( commantary )
        {
            
            std::getline(ss, item, delim);
            std::getline(ss, val, delim);
            
            if ( !StringOperations::isEmpty( item) && !StringOperations::isEmpty( val )) // acceptable pair item/value, ie. not empty
            {
                bool overwrite = true;
                
                
                if ( item.find("+") != std::string::npos )
                {
                    
                    
                    item.erase(item.end() -1);
                    
                    overwrite = false;
                }
                if (delim != ' ' )
                {
                    item.erase( remove_if(  item.begin(), item.end(), ::isspace ), item.end());
                    
                    /* trim value, ie remove leading & trailing spaces */
                    val = StringOperations::trim( val , " \t\n" );
                    
                    if (!StringOperations::contains(val, "\""))
                    {
                    }
                    
                    else // quoted token
                    {
                        // si un seul " trouvé et dans la premiere moité de la chaine, considérée comme premier,
                        // sinon considéré comme second
                        const std::string::size_type halfSize = val.size() / 2;
                        
                        std::string::size_type first  = 0;
                        
                        first = val.find( "\"", first );
                        
                        std::string::size_type second = first + 1 ;
                        
                        second = val.find( "\"", second );
                        
                        if ( second == std::string::npos)
                        {
                            const std::string::size_type pos = first;
                            if ( pos < halfSize)
                            {
                                val.append("\"");
                                second = val.size() -1;
                            }
                            else
                            {
                                second = pos;
                                first = 0;
                                val.insert(0, "\"");
                            }
                            
                        }
                        
                        val.erase( val.begin() + second  , val.end() );
                        val.erase( val.begin()           , val.begin() +  first +1 );
                        
                    }
                }
                
                if (val != "" ) // valid ( &nice ) pair
                {
                    if ( itemExists( item )) // existant
                    {
                        
                        
                        auto pos = findItemPosition( item );
                        std::string oldVal = getValueForItemName(item)->getString();
                        
                        if ( pos != _dataList.end() )
                        {
                            _dataList.erase( pos);
                            count--;
                            
                            if (!overwrite) // on écrase la valeur
                            {
                                
                                
                                val += " " + oldVal;
                                
                            }
                        }
                        
                    }
                    
                    count++;
                    insertValue ( item , Value<std::string>::value( val ) );
                    
                    
                    
                    //                        printf("\n add value '%s' for '%s'" , value.c_str() , item.c_str());
                }
            }
            
        }
        
    }
    
    if ( count != (int) _dataList.size() )
    {
        /*list size and iter count don't match */
        DEBUG_ASSERT( false );
        
    }
    
    fclose(file);
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Database::saveToFile(const std::string &fileName , const char delim  )
{
    
    std::ofstream file;
    
    file.open( fileName.c_str() );
    
    if ( !file.is_open() )
        return false;
    
    Date now;
    Timecode t;
    t.setToCurrentTime();
    
    file << "########## Database file generated on " << now.toString().c_str()
    << " at " << t.getString(true,true,true,false).c_str() << " ##########" ;
    
    
    file <<  "\n";
    file <<  "\n";
    
    for (const std::pair<std::string , Variant *> &pair : _dataList )
    {
        file << pair.first << " " << delim << " " << pair.second->getString();
        file <<  "\n";
        file <<  "\n";
        
    }
    
    
    
    file.close();
    return true;
}











