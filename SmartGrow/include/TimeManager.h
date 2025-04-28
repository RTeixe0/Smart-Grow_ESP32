#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>

void initTime();
String getFormattedTime();
void getCurrentTime(int& hour, int& minute);

#endif // TIME_MANAGER_H
