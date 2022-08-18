
#ifndef __ARCHIVE_H
#define __ARCHIVE_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

extern xQueueHandle xDataQueue, xDataResolution, xDataStatusQueue;
extern xQueueHandle xTotalizerQueue, xTotalizerResolution;
extern xQueueHandle xChannelsQueue, xMessageQueue;

void CreateArchiveTask(void);
void ARCHIVE_SafeStop(void);
void ARCHIVE_SafeStart(void);
void ARCHIVE_CreateNewArchives(void);
void ARCHIVE_SendEvent(char* eventDesc);

uint8_t ARCHIVE_GetFrequencyState(void);
void ARCHIVE_SetFrequencyState(uint8_t NewState);
int ARCHIVE_GetState(void);
void ARCHIVE_SaveBuffersToSDCard(void);

uint32_t ARCHIVE_TakeSemaphore(uint32_t timeout);
void ARCHIVE_GiveSemaphore(void);
uint8_t ARCHIVE_IsCurrent(const char *archive);
uint8_t ARCHIVE_IsFileCurrentArchive(char * fileName);

#endif /*__ARCHIVE_H */
