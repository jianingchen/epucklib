/* 
 * File:   Task_ObjectFollowing.h
 * Author: Jianing Chen
 *
 * Created on 04 November 2014, 21:37
 */

#ifndef TASK_OBJECTFOLLOWING_H
#define	TASK_OBJECTFOLLOWING_H

#include <el.h>

#define TASK_OBJECT_RED     0
#define TASK_OBJECT_GREEN   1
#define TASK_OBJECT_BLUE    2

extern el_uint8 Task_ObjectColor;
extern int vision_mass_red;
extern int vision_bias_red;
extern int vision_mass_green;
extern int vision_bias_green;
extern int vision_mass_blue;
extern int vision_bias_blue;


void Task_ObjectFollowing_Setup();
void Task_ObjectFollowing_Clear();

#endif	/* TASK_OBJECTFOLLOWING_H */

