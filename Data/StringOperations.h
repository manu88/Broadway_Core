//
//  StringOperations.h
//  Broadway_core
//
//  Created by Manuel Deneu on 14/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__StringOperations__
#define __Broadway_core__StringOperations__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace StringOperations
{
    bool beginWith( const std::string &stringToTest , const std::string &stringToFind);

    std::string trim( const std::string& str, const std::string& whitespace = " \t");
    
    
    /*
        Reduce a string by removing char passed in whitespace an filling internal separators with fill.
     
        Examples:
            reduce("      test \t foo \t\t   bar \t\t\ " );
                
            ->Result = 'test foo bar';
     
             reduce("      test \t foo  \t  bar\t\ ","_" );
             
             ->Result = 'test_foo_bar';
     */
    std::string reduce( const std::string& str , const std::string& fill = " ", const std::string& whitespace = " \t");

    /*
        Write a formatted string like printf and others
     */
    std::string stringWithFormat(const std::string fmt_str, ...);
    
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);
    
    
    inline std::string toLowerCase(const std::string &string )
    {
        std::string ret = string;
        
        std::transform( ret.begin(), ret.end(), ret.begin(), ::tolower);
        
        return ret;
    }
    
    inline void replaceAll(std::string &str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        
    }
    
    
    
    
}
#endif /* defined(__Broadway_core__StringOperations__) */
