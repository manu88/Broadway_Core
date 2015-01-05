//
//  TimeDefs.h
//  Broadway_core
//
//  Created by Manuel Deneu on 04/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_core_TimeDefs_h
#define Broadway_core_TimeDefs_h

#include <time.h>
#include <stdio.h>
#include <assert.h>

#include "../Data/StringOperations.h"

typedef enum
{
    Monday     = 0,
    Tuesday    = 1,
    Wednesday  = 2,
    Thursday   = 3,
    Friday     = 4,
    Saturday   = 5,
    Sunday     = 6
} Day ;

typedef enum
{
    January    = 1,
    February   = 2,
    March      = 3,
    April      = 4,
    May        = 5,
    June       = 6,
    July       = 7,
    August     = 8,
    September  = 9,
    October    = 10,
    November   = 11,
    December   = 12
    
} Month ;


struct Date_t
{
    
    Date_t()
    {
        getCurrent();
    }
    
    void update()
    {
        getCurrent();
    }
    
    bool operator==(const Date_t & rhs)
    {
        return (dayNum == rhs.dayNum) && ( day == rhs.day );
    }
    
    bool operator!=(const Date_t & rhs)
    {
        return !(*this == rhs);
        
    }
    
    std::string toString() const
    {
        std::string ret = "";
        
        if (day == Monday)
            ret += "Monday";
        else if (day == Tuesday)
            ret += "Tuesday";
        else if (day == Wednesday)
            ret += "Wednesday";
        else if (day == Thursday)
            ret += "Thursday";
        else if (day == Friday)
            ret += "Friday";
        else if (day == Saturday)
            ret += "Saturday";
        else if (day == Sunday)
            ret += "Sunday";
        
        ret += " ";
        ret += std::to_string(dayNum);
        ret += " ";
        
        if ( month == January)
            ret +="January";
        else if ( month == February)
            ret +="February";
        else if ( month == March)
            ret +="March";
        else if ( month == April)
            ret +="April";
        else if ( month == May)
            ret +="May";
        else if ( month == June)
            ret +="June";
        else if ( month == July)
            ret +="July";
        else if ( month == August)
            ret +="August";
        else if ( month == September)
            ret +="September";
        else if ( month == October)
            ret +="October";
        else if ( month == November)
            ret +="November";
        else if ( month == December)
            ret +="December";
        
        ret += " ";
        ret += std::to_string(year);
        ret += " ";
        
        return ret;
    }
    
    

    Day   day;
    Month month;
    
    unsigned int dayNum;
    unsigned int year;
    
private:
    
    void getCurrent()
    {
        time_t mytime;
		mytime = time(NULL);
        
        // somthing like 'Sun Jan  4 19:07:25 2015'
        // ie Day Month DayNum HH:MM:SS Year
        const std::string formated = ctime(&mytime);
        
        auto list = StringOperations::split(formated, ' ');
        /*
         0 -> day
         1 -> month
         2 -> NULL
         3 -> daynum
         4 -> time
         5 -> year
         */
        
        if ( list.at(0) == "Sun" )
            day = Sunday;
        else if ( list.at(0) == "Mon" )
            day = Monday;
        else if ( list.at(0) == "Tue" )
            day = Tuesday;
        else if ( list.at(0) == "Wed" )
            day = Wednesday;
        else if ( list.at(0) == "Thu" )
            day = Thursday;
        else if ( list.at(0) == "Fri" )
            day = Friday;
        else if ( list.at(0) == "Sat" )
            day = Saturday;
        else
            assert(false);
        
        /* **** */
        
        if ( list.at(1) == "Jan" )
            month = January;
        else if ( list.at(1) == "Feb" )
            month = February;
        else if ( list.at(1) == "Mar" )
            month = March;
        else if ( list.at(1) == "Apr" )
            month = April;
        else if ( list.at(1) == "May" )
            month = May;
        else if ( list.at(1) == "Jun" )
            month = June;
        else if ( list.at(1) == "Jul" )
            month = July;
        else if ( list.at(1) == "Aug" )
            month = August;
        else if ( list.at(1) == "Sep" )
            month = September;
        else if ( list.at(1) == "Oct" )
            month = October;
        else if ( list.at(1) == "Nov" )
            month = November;
        else if ( list.at(1) == "Dec" )
            month = December;
        else
            assert(false);
        
        dayNum = atoi( list.at(3).c_str() );
        year = atoi( list.at(5).c_str() );
    }
    
};

typedef struct Date_t Date;

/* *** **** **** **** **** **** **** **** **** */


static Day dayFromExplicitFrench( const std::string &text)
{
    const std::string day = StringOperations::toLowerCase( text);
    
    printf("\n DAY = '%s' ", day.c_str() );
    
    if ( day == "lundi")
        return Monday;
    
    else if ( day == "mardi" )
        return Tuesday;
    
    else if ( day == "mercredi")
        return Wednesday;
    
    else if ( day == "jeudi")
        return Thursday;
    
    else if ( day == "vendredi")
        return Friday;
    
    else if ( day == "samedi")
        return Saturday;

    return Sunday;
    
}

static Date dateFromStringWithDelimiter( const std::string &string , const char delim)
{
    Date date;
    
    auto list = StringOperations::split( string , delim);
    
    date.dayNum =         atoi( list.at(0).c_str() );
    date.month  = (Month) atoi( list.at(1).c_str() );
    date.year   =         atoi( list.at(2).c_str() );
    
    
    return date;
    
}





#endif
