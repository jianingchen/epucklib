/* 
 * File:   TaskTrigger_ObjectFollowing.h
 * Author: Jianing Chen
 *
 * Created on 04 November 2014, 21:37
 */

#ifndef TASKTRIGGER_OBJECTFOLLOWING_H
#define	TASKTRIGGER_OBJECTFOLLOWING_H

#include <el.h>
#include "ServiceTrigger_CameraImageProcessing.h"

extern el_handle TT_ObjectFollowing;
extern el_uint8 TT_ObjectColor;

#define TT_OBJECT_COLOR_RED     0
#define TT_OBJECT_COLOR_GREEN   1
#define TT_OBJECT_COLOR_BLUE    2

void Trigger_ObjectFollowing_Setup();
void Trigger_ObjectFollowing_Clear();

#endif	/* TASKTRIGGER_OBJECTFOLLOWING_H */

