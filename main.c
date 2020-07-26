#include "lunar.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv){
    int i;
    int j;
    int start;
    int week_day;
    int calendar_flag = 0;
    char tmp[20];
    char dte[20] = {'\0'};
    char opt;
    char* strs[3] = {NULL};
    struct date solar_date;
    struct date lunar_date;
    //extern char *lunar_months_zh[];
    //extern char *lunar_days_zh[];
    //extern char *zodiac_cn[];
    //extern char *heaven[];
    //extern char *earth[];
    char lunar_represent[200];
    if(argc < 2){
        fprintf(stderr, "Usage: %s <year>-<month>-<day>\n", argv[0]);
        return 0;
    }
    while((opt = getopt(argc, argv, "cd:")) != -1){
        switch (opt)
        {
        case 'c':
            calendar_flag = 1;
            break;
        case 'd':
            strcpy(dte, optarg);
            break;
        default:
            break;
        }
        
    }

    strcpy(tmp, dte);
    for(i = j = start = 0; i <= strlen(tmp); ++i){
        if(dte[i] == '-' || dte[i] == '/' || i == strlen(tmp)){
            strs[j++] = dte + start;
            dte[i] = '\0';
            start = i+1;
            if(j >= 3)
            break;
        }
        else if(dte[i] < '0' || dte[i] > '9'){
            fprintf(stderr, 
                    "The dte string should only contain digits, '-' or '/'.\n"
            );
            return 0;
        }
    }
    if(j < 3){
        fprintf(stderr, 
                "Failed to parse date string. \
                Usage: %s <year>-<month>-<day>\n",
                argv[0]
        );
        return 0;
    }

    // To  calculate
    solar_date.year = atoi(strs[0]);
    solar_date.month = atoi(strs[1]), 
    solar_date.day = atoi(strs[2]);
    solar_date.solar = 1;

    convert_to_lunar_date(&solar_date, &lunar_date);
    output_lunar_repr(&lunar_date, lunar_represent);
    fprintf(stdout, "%s\n", lunar_represent);
    /*
    if(days & 0x10000){
        int leap_month = get_leap_month(lunar_date.year);
        if(month > leap_month){
            month -= 1;
        }
    }
    fprintf(stdout, "%d年%s%s，", 
            lunar_date.year, 
            lunar_months_zh[month-1], 
            lunar_days_zh[lunar_date.day-1]
    );
    int month = lunar_date.month;
    int days = get_lunar_days(lunar_date.year);
    fprintf(stdout, "是本农历年的第%d天。\n", calc_which_day(&lunar_date));
    fprintf(stdout, 
            "%s%s年，生肖%s。\n", 
            heaven[(lunar_date.year % 10)], 
            earth[(lunar_date.year) % 12], 
            zodiac_cn[(lunar_date.year - 4) % 12]
    );
    */

    if(calendar_flag){
        char calendar[200];
        output_month_calendar(&solar_date, calendar, &week_day);
        fprintf(stdout, "%s\n", calendar);
    }
    else{
        week_day = calc_week_day(&solar_date);
        fprintf(stdout, "星期%d\n", week_day);
    }

    return 0;
}