
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
    
    for (int j=0; j< ( int ) tok2.size() ; ++j)
        ret += "../";
    
    for (int j=0; j< ( int ) tok1.size() ; ++j)
    {
        ret += tok1.at(j);// +  "/";
        if ( j< ( int) ( tok1.size() - 1 ) )
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

const std::string FileSystem::getFileExtension( const std::string &filePath )
{
    return filePath.substr( filePath.find_last_of(".") + 1);
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
                                                                     bool  sortABC,
                                                                     const std::string &beginWith ,
                                                                     const std::string withExtention
                                                                    )
{

    
    std::vector< std::string > ret;
    
    getFilesListFromFolder( ret, path , "" , withFullPath , beginWith , withExtention);
    
    if ( sortABC )
        std::sort(ret.begin(), ret.end() );
    
    return ret;

    
}

const bool FileSystem::getFilesListFromFolder(
                                  std::vector< std::string> &vector,
                                  const std::string &fullPath ,
                                  const std::string &relPath ,
                                  bool  withFullPath,
                                  const std::string &beginWith,
                                  const std::string withExtention
                                  )
{

    DIR * d;
    
    /* Open the directory specified by "dir_name". */
    
    const std::string realPath  = fullPath+relPath;
    
//    printf("\n entering dir '%s'" , realPath.c_str()  );
    
    d = opendir ( realPath.c_str() );
    
    /* Check it was opened. */
    if (! d)
    {
        fprintf (stderr, "Cannot open directory '%s': %s\n", realPath.c_str(), strerror (errno));
        return false;
    }
    
    while (1)
    {
        struct dirent * entry;
        const char * d_name;
        
        /* "Readdir" gets subsequent entries from "d". */
        entry = readdir (d);
        if (! entry)
        {
            /* There are no more entries in this directory, so break
             out of the while loop. */
            break;
        }
        
        d_name = entry->d_name;
        
        
        if (   ( strcmp( entry->d_name , "."  ) != 0 )
            && ( strcmp( entry->d_name , ".." ) != 0 )
            )
        {
            if (entry->d_type & DT_REG)
            {
                
                const std::string item = (withFullPath?fullPath :"") + (relPath.empty()?"":(relPath + "/") ) + std::string( d_name );
            
                if ( (beginWith.empty() ) || StringOperations::beginWith( d_name, beginWith ) )
                {
                    vector.push_back( item );
                }
            }

        
            else if (entry->d_type & DT_DIR)
            {
                

                    int path_length;
                    char path[PATH_MAX];
                    
                    path_length = snprintf (path, PATH_MAX, "%s%s", fullPath.c_str() , d_name);


                    
                    if (path_length >= PATH_MAX)
                    {
                        fprintf (stderr, "Path length has got too long.\n");

                        return false;
                    }
                    /* Recursively call "list_dir" with the new path. */
                getFilesListFromFolder(vector, correctPathIfNeeded(fullPath),(relPath.empty()?"":(relPath + "/") ) + std::string( d_name ), withFullPath , beginWith , withExtention);
                    


            }
            
        }

    } // end of while

    closedir (d);

    return true;

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
