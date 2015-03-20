//
//  HTMLParser.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 19/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <gumbo.h>

#include "HTMLParser.h"


HTMLParser::HTMLParser()
{
    GumboOutput* output = gumbo_parse("<h1>Hello, World!</h1>");
    // Do stuff with output->root
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

HTMLParser::~HTMLParser()
{
    
}