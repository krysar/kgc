#ifndef KGC_LED_H
#define KGC_LED_H

#define LED_CNT 10
#define LED_CON 2
#define LED_FLD 3
#define LED_ELC 7
#define LED_MOP 9
#define LED_LFO 8
#define LED_OTR 10
#define LED_SAS 6
#define LED_RCS 11
#define LED_GEA 4
#define LED_BRK 12
#define BUZZ    14

#define LED_STAT_CON 0
#define LED_STAT_FLD 1
#define LED_STAT_ELC 2
#define LED_STAT_MOP 3
#define LED_STAT_LFO 4
#define LED_STAT_OTR 5
#define LED_STAT_SAS 6
#define LED_STAT_RCS 7
#define LED_STAT_GEA 8
#define LED_STAT_BRK 9
#define LED_STAT_BUZZ 10
#define LED_STAT_DISP1 11

#include "connection.h"

extern bool led_stat[12];
extern bool blink_stat;
extern struct repeating_timer led_blink_tmr;

void led_init();
void led_update(KSP_DATA ksp_in);
bool led_blinker(__unused struct repeating_timer *t);


#endif //KGC_LED_H