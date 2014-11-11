/* 
 * File:   ServiceTrigger_CameraImageProcessing.h
 * Author: Jianing Chen
 *
 * Created on 04 November 2014, 21:07
 */

#ifndef SERVICETRIGGER_CAMERAIMAGEPROCESSING_H
#define	SERVICETRIGGER_CAMERAIMAGEPROCESSING_H

#include <el.h>

extern volatile int IMG_RedMass;
extern volatile int IMG_RedBias;
extern volatile int IMG_GreenMass;
extern volatile int IMG_GreenBias;
extern volatile int IMG_BlueMass;
extern volatile int IMG_BlueBias;

void Trigger_CameraImageProcessing_Setup();
void Trigger_CameraImageProcessing_Clear();

#endif	/* SERVICETRIGGER_CAMERAIMAGEPROCESSING_H */

