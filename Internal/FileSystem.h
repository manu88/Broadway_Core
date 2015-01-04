//
//  FileSystem.h
//  Broadway_core
//
//  Created by Manuel Deneu on 30/11/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__FileSystem__
#define __Broadway_core__FileSystem__

#include <iostream>


namespace FileSystem
{
    // return pathToGet relative to PathRelativeTo,
    // ie the relative path to go from pathRelativeTo to pathToGet
    std::string getPathRelativeTo( const std::string &pathToGet , const std::string &pathRelativeTo);
    
    bool hasSuffix(const std::string &str, const std::string &suffix);
    
    bool fileExists( const std::string &filepath);
    bool folderExists( const std::string &path);
    
    bool isFile( const std::string &path);
    bool isFolder( const std::string &path);
    
    bool createFile( const std::string &path);
    
    // will return -1 if invalid (?)
    long long getFileSize( const std::string &filepath);
    
    const std::string correctPathIfNeeded( const std::string &path);
    
    // will return an empty string if nothing found;
    const std::string locateFileFromFoldersList( const std::string &filename , const std::vector<std::string> &vector);
    
    
    const std::vector< std::string > getFilesListFromFolder( const std::string &path ,
                                                             bool  withFullPath,
                                                             const std::string &beginWith = "" ,
                                                             const std::string withExtention = "*"
                                                            );
    
    
} /*namespace FileSystem*/

#endif /* defined(__Broadway_core__FileSystem__) */
