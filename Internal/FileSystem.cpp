
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

#include "../Data/StringOperations.h"
#include "FileSystem.h"

std::string FileSystem::getPathRelativeTo( const std::string &pathToGet , const std::string &pathRelativeTo)
{
    std::istringstream pa1( pathToGet );
    std::istringstream pa2( pathRelativeTo );
    
    std::vector<std::string> tok1;
    std::vector<std::string> tok2;
    
    std::string s;
    
    while( std::getline(pa1, s, '/') )
        tok1.push_back(s);
    
    while( std::getline(pa2, s, '/') )
        tok2.push_back(s);
    

    const int smallSize = static_cast<int>( (tok1.size() < tok2.size() ? tok1.size() : tok2.size() ) );
    
    int i=0;
    
    for (; i<smallSize ; ++i)
    {
        if ( tok1.at(i) != tok2.at(i) )
            break;
        
    }
    
    tok1.erase( tok1.begin(), tok1.begin() + i);
    tok2.erase( tok2.begin(), tok2.begin() + i);
    
    std::string ret;
    
    for (int j=0; j< tok2.size() ; ++j)
        ret += "../";
    
    for (int j=0; j< tok1.size() ; ++j)
    {
        ret += tok1.at(j);// +  "/";
        if ( j< ( tok1.size() - 1 ) )
            ret += "/";
    }
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::hasSuffix( const std::string &str, const std::string &suffix )
{
    return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::fileExists( const std::string &filepath)
{
    struct stat results;
    
    return ( stat( filepath.c_str() , &results) >= 0 && S_ISREG(results.st_mode) );

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::folderExists( const std::string &path)
{
    struct stat results;

    return ( stat( path.c_str() , &results) >= 0 && S_ISDIR(results.st_mode) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::isFile( const std::string &path)
{
    return fileExists( path );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::isFolder( const std::string &path)
{
    return folderExists( path );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::createFile( const std::string &path)
{
    FILE *ifp = nullptr;
    ifp = fopen( path.c_str() , "w");
    
    if (ifp)
    {
        fclose(ifp);
        return true;
    }
    

    
    return false;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

long long FileSystem::getFileSize( const std::string &filepath)
{
    struct stat st;
    stat(filepath.c_str() , &st);
    
    return st.st_size;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string FileSystem::correctPathIfNeeded( const std::string &path)
{
    if (hasSuffix(path, "/"))
        return path;
    
    return path + "/";
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string FileSystem::locateFileFromFoldersList( const std::string &filename , const std::vector<std::string> &vector)
{
    for (const auto p : vector)
    {
        const std::string fullPath = p + filename;
        
        if ( fileExists( fullPath) )
            return fullPath;
    }
    
    return "";
}

const std::string FileSystem::locateFolderFromFoldersList( const std::string &folder , const std::vector<std::string> &vector)
{
    const std::string fol = correctPathIfNeeded( folder );
    
    for (const auto p : vector)
    {
        const std::string fullPath = p + fol;
        
        if ( folderExists( fullPath) )
            return fullPath;
    }
    
    return "";
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::vector< std::string > FileSystem::getFilesListFromFolder( const std::string &path ,
                                                                     bool  withFullPath,
                                                                     const std::string &beginWith ,
                                                                     const std::string withExtention
                                                                    )
{
    DIR * d = nullptr;
    if ( isFolder( path) && (d = opendir ( path.c_str() ) ))
    {
        std::vector< std::string > list;
        while (1)
        {
            struct dirent * entry;
            
            entry = readdir (d);
            
            if (! entry)
            {
                break;
            }
            
            if (   ( strcmp( entry->d_name , "."  ) != 0 )
                && ( strcmp( entry->d_name , ".." ) != 0 )
                )
            {
                std::string item( entry->d_name );
                

                
                if ( (!beginWith.empty() && StringOperations::beginWith( item, beginWith ) ) )
                {
                    if (withFullPath)
                        item.insert( 0 , path);
                    
                    list.push_back( item );
                }
            
            }
        }
        return list;
    }
    
    // error
    return  std::vector< std::string >();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string FileSystem::getFileText( const std::string &fileUrl)
{
    std::ifstream in( fileUrl );
        
    return std::string((std::istreambuf_iterator<char>(in)),
                                std::istreambuf_iterator<char>());
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FileSystem::setFileText(const std::string &file , const std::string &data)
{
    FILE *f = fopen(file.c_str() , "w");
    
    if (f)
    {
        fprintf(f, "%s", data.c_str() );
        
        fclose( f );
        return true;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
