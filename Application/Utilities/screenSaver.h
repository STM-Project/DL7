/*
 * screenSaver.h
 *
 *  Created on: 27.01.2017
 *      Author: Tomaszs
 */

#ifndef SCREENSAVER_H_
#define SCREENSAVER_H_

void ScreenSaverInit(unsigned char brightness, unsigned char Time, unsigned char Type);
void ResetScreenSaverTimer(void);
void RestartScreenSaver(unsigned char brightness, unsigned char Time, unsigned char Type);
void StartScreenSaverTimer(void);

#endif /* SCREENSAVER_H_ */
