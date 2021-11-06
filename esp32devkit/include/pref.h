#ifndef __PREF_H__
#define __PREF_H__

#include <Preferences.h>

extern Preferences pref;
#define EEPR_GS pref.getShort
#define EEPR_GD pref.getDouble
#define MAIN_PREF_KEY_LIST {"bleName", "deviceMode", "speedLeft", "speedRight", "servo", "servoMin", "servoMax", "servoMid", "motorP", "motorI", "motorD"}

#endif /* __PREF_H__ */