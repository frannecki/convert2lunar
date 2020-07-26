#ifndef LUNAR_H
#define LUNAR_H
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct date{
    int year;
    int month;
    int day;
    int solar;
};

void convert_to_lunar_date(struct date*, struct date*);
int calc_week_day(struct date*);
void output_month_calendar(struct date*, char*, int*);
void output_lunar_repr(struct date*, char*);
int get_lunar_days(int);
int get_leap_month(int);
int calc_which_day(struct date*);
int calc_gap(struct date*, struct date*);
int sum_days(int, int);
int cmp_date(struct date*, struct date*);
int is_leap_year(int);

#endif