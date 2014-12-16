//
//  StringOperations.h
//  Broadway_core
//
//  Created by Manuel Deneu on 14/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__StringOperations__
#define __Broadway_core__StringOperations__

#include <iostream>
#include <string>

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
    
}
#endif /* defined(__Broadway_core__StringOperations__) */
