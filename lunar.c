#include "lunar.h"

/**
 * 各农历年各月天数
 * 
 * 从1900年到2049年每个农历年对应的月份信息。每年的月份信息由一个十六进制数表示。
 * 十六进制数的最低位表示该年的闰月（为0则不是闰年）。最高位1表示闰大月，0表示闰小月。
 * 中间三位转化为二进制数共12位，表示12个月份，1表示大月（30天），0表示小月（29天）。
 */
static unsigned int lunar_chart[]={
	0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
	0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
	0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
	0x06566,0x0d4a0,0x0ea50,0x06e95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
	0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
	0x06ca0,0x0b550,0x15355,0x04da0,0x0a5d0,0x14573,0x052d0,0x0a9a8,0x0e950,0x06aa0,
	0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
	0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b5a0,0x195a6,
	0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
	0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x055c0,0x0ab60,0x096d5,0x092e0,
	0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
	0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
	0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
	0x05aa0,0x076a3,0x096d0,0x04bd7,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
	0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0
};

static unsigned int month_days[2][12]={
	{31,28,31,30,31,30,31,31,30,31,30,31},
	{31,29,31,30,31,30,31,31,30,31,30,31}
};  

char *lunar_months_zh[] = {
    "正月","二月","三月","四月","五月", "六月",
    "七月","八月","九月","十月","十一月","腊月"
};

static struct date base_solar_date = {1900, 1, 31, 1};
static struct date last_solar_date = {2050, 1, 22, 1};

static struct date base_lunar_date = {1900, 1, 1, 0};
static struct date last_lunar_date = {2049, 12, 29, 0};

char *lunar_days_zh[]={
    "初一","初二","初三","初四","初五", "初六",
    "初七","初八","初九","初十","十一","十二",
    "十三","十四","十五","十六","十七", "十八",
    "十九","二十","廿一","廿二","廿三","廿四",
    "廿五","廿六","廿七","廿八","廿九", "三十",
};

char *zodiac_cn[] = {
    "鼠", "牛", "虎", "兔", "龙", "蛇", 
    "马", "羊", "猴", "鸡", "狗", "猪"
};

char *heaven[] = {
    "庚", "辛", "壬", "癸", "甲", 
    "乙", "丙", "丁", "戊", "己"
};

char *earth[] = {
    "申", "酉", "戌", "亥", "子", "丑", 
    "寅", "卯", "辰", "巳", "午", "未"
};

/**
 * 将公历日期转为农历日期
 * 
 * @param dat  待转化的公历日期
 * @param dat  转化后的农历日期
 */
void convert_to_lunar_date(struct date* dat, struct date* lunar_dat){
    int gap;
    int sum;
    int month;
    int days;
    struct date lunar_date = base_lunar_date;
    assert(dat->solar);
    assert(cmp_date(dat, &base_solar_date) >= 0 && cmp_date(dat, &last_solar_date) <= 0);
    gap = calc_gap(dat, &base_solar_date);

    while((sum = sum_days(lunar_date.year, 0)) < gap){
        gap -= sum;
        lunar_date.year += 1;
    }
    days = get_lunar_days(lunar_date.year);
    month = (days & 0xf0000) ? 13 : 12;
    while((sum = ((days & (1 << (month-lunar_date.month))) ? 30 : 29)) < gap){
        gap -= sum;
        lunar_date.month += 1;
    }
    while(gap-- > 0){
        lunar_date.day += 1;
    }
    *lunar_dat = lunar_date;
}

/**
 * 计算某个日期是星期几
 * 
 * @param dat  一个公历或农历日期
 * 
 * @return  当天对应星期几，范围为[1, 7]
 */
int calc_week_day(struct date* dat){
    int week_day = 0;
    struct date base_date;
    base_date.solar = dat->solar;
    // 2000年1月1日（农历1999年腊月廿五）是星期六
    if(dat->solar){
        base_date.year = 2000;
        base_date.month = 1;
        base_date.day = 1;
    }
    else{
        base_date.year = 1999;
        base_date.month = 12;
        base_date.day = 25;
    }
    int diff = calc_gap(dat, &base_date) % 7;
    if(diff < 0)  diff += 7;
    week_day = (6 + diff) % 7;
    if(week_day == 0)  week_day = 7;
    return week_day;
}

/**
 * 输出农历日期的部分信息
 * 
 * @param dat  一个农历日期
 * @param str  保存输出信息的字符串，包括该日期的中文表示，在当年中是第几天，对应生肖以及干支纪年
 */
void output_lunar_repr(struct date* dat, char* str){
    char num[10];
    int leap_flag = 0;
    int leap_month;
    int month = dat->month;
    int days = get_lunar_days(dat->year);
    assert(dat->solar == 0);
    assert(dat->year >= base_lunar_date.year && dat->year <= last_lunar_date.year);
    if(days & 0xf0000){
        leap_flag = 1;
        leap_month = (days & 0xf0000) >> 16;
        if(month > leap_month){
            month -= 1;
        }
    }
    sprintf(num, "%d", dat->year);
    //strcpy(str, itoa(dat->year, num, 10));
    strcpy(str, num);
    strcat(str, "年");
    if(leap_flag && dat->month == leap_month + 1){
        strcat(str, "闰");
    }
    strcat(str, lunar_months_zh[month-1]);
    strcat(str, lunar_days_zh[dat->day-1]);
    strcat(str, "，是本农历年的第");
    sprintf(num, "%d", calc_which_day(dat));
    strcat(str, num);
    //strcat(str, itoa(calc_which_day(dat), num, 10));
    strcat(str, "天。\n");
    strcat(str, heaven[dat->year % 10]);
    strcat(str, earth[dat->year % 12]);
    strcat(str, "年，");
    strcat(str, "生肖");
    strcat(str, zodiac_cn[(dat->year - 4) % 12]);
}

/**
 * 输出某个公历日期所在月份的日历
 * 
 * @param dat  一个公历日期
 * @param str  存放日历的字符串
 * @param week_day  当天对应星期几，范围为[1, 7]
 */
void output_month_calendar(struct date* dat, char* str, int* week_day){
    int i;
    int week_day_first;
    char num[10];
    assert(dat->solar);
    strcpy(str, "\nSun\tMon\tTue\tWed\tThu\tFri\tSat\n");
    *week_day = calc_week_day(dat);
    week_day_first = (*week_day - dat->day + 1) % 7;
    if(week_day_first < 0)  week_day_first += 7;
    for(i = 0; i < week_day_first; ++i){
        strcat(str, "\t");
    }
    for(i = 1; i <= month_days[is_leap_year(dat->year)][dat->month-1]; ++i){
        sprintf(num, "%d", i);
        strcat(str, num);
        //strcat(str, itoa(i, num, 10));
        strcat(str, "\t");
        if((i + week_day_first) % 7 == 0){
            strcat(str, "\n");
        }
    }
}

/**
 * 返回某个农历年各月的天数
 * 
 * @param year  一个农历年
 * 
 * @return  一个32位整型数，低12位或13位表示各月的天数(1表示30天，0表示29天)，第17到20为表示闰月的月份
 */
int get_lunar_days(int year){	
    int leap_month;
    int code = lunar_chart[year-1900];
	int days = (code & 0xfff0) >> 4;
    assert(year >= base_lunar_date.year && year <= last_lunar_date.year);
    leap_month = code & 0x0f;
    if(leap_month){
        int big = (code & 0x10000) >> 16;
        int tmp = days;
        days = 0;
        days |= tmp & ((1<<(12-leap_month))-1);
        days |= big << (12-leap_month);
        days |= (tmp >> (12-leap_month)) << (13-leap_month);
        days |= leap_month << 16;
    }
    return days;
}

/*
int get_leap_month(int year){
    int code;
    assert(year >= base_lunar_date.year && year <= last_lunar_date.year);
    code = lunar_chart[year-1900];
    return (lunar_chart[year-1900] & 0x0f);
}
*/

/**
 * 计算一个日期是其对应年的第几天
 * 
 * @param year  一个公历或农历日期
 * 
 * @return  该日期在当年中的编号
 */
int calc_which_day(struct date* dat){
    int which_day = dat->day;
    // 计算其为本年度的第几天
    if(dat->solar){
    int is_leap = is_leap_year(dat->year);
    for (int i = 0; i < dat->month - 1; i++)
        which_day += month_days[is_leap][i];
    }
    else{
    int days = get_lunar_days(dat->year);
    int month = (days & 0xf0000) ? 13 : 12;
    for (int i = 1; i < dat->month; i++)
        which_day += (days & (1 << (month-i))) ? 30 : 29;
    }
    return which_day;
}

/**
 * 计算两个日期之差，二者必须同为公历或者农历日期
 * 
 * @param dat1  一个日期
 * @param dat2  另一个日期
 * 
 * @return  二者相差的天数
 */
int calc_gap(struct date* dat1, struct date* dat2){
    assert(dat1->solar == dat2->solar);
    int diff = 0;
    if(cmp_date(dat1, dat2) >= 0){
        int i;
        int which1 = calc_which_day(dat1);
        int which2 = calc_which_day(dat2);
        diff = which1 - which2;
        for(i = dat2->year; i < dat1->year; ++i)
        diff += sum_days(i, dat2->solar);
    }
    else
    diff = -calc_gap(dat2, dat1);
    return diff;
}

/**
 * 计算一个年份的总天数
 * 
 * @param year  年份
 * @param solar  该年是公历年还是农历年，1表示公历年，0表示农历年
 * 
 * @return  该年的总天数
 */
int sum_days(int year, int solar){
    if(solar){
    // solar
        return is_leap_year(year) ? 366 : 365;
    }
    
    // lunar
    int sum = 0;
    int days = get_lunar_days(year);
	int month = (days & 0xf0000) ? 13 : 12;
	for(int j = 1; j <= month; j++)
	    sum += (days & (1 << (month-j))) ? 30 : 29;
	return sum;
}

/**
 * 比较两个日期大小，二者必须同为公历或者农历日期
 * 
 * @param dat1  一个日期
 * @param dat2  另一个日期
 * 
 * @return  返回1（dat1比dat2早），0（同一天）或者-1
 */
int cmp_date(struct date* dat1, struct date* dat2){
    assert(dat1->solar == dat2->solar);
    if(dat1->year == dat2->year && dat1->month == dat2->month && dat1->day == dat2->day){
        return 0;
    }
    else if(dat1->year > dat2->year){
        return 1;
    }
    else if(dat1->year == dat2->year && dat1->month > dat2->month){
        return 1;
    }
    else if(dat1->year == dat2->year && dat1->month == dat2->month && dat1->day > dat2->day){
        return 1;
    }
    return -1;
}

/**
 * 判断一个公历年是否为闰年
 * 
 * @param year  年份
 * 
 * @return  返回1（闰年）或者0（平年）
 */
inline int is_leap_year(int year){
    // 判断y所表示的年份是否为闰年，是则返回1，否则返回0. 
	if ((year % 4 == 0) && (year % 100 != 0))
       return 1;
    else if ((year % 400==0) && (year % 3200 != 0))
       return 1;
    else
       return 0;
}