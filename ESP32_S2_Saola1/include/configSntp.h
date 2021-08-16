#ifndef CONFIGSNTP_H
#define CONFIGSNTP_H
#include <sys/time.h>
#include <esp_sntp.h>
#include <configWifi.h>

extern void initializeSntpUpdate();

extern void sntpUpdateNotification();

#endif // CONFIGSTNP_H
