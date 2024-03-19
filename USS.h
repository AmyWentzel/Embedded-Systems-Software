//USS.c Header File

#ifndef USS
#define USS
void USS_Timer16_Init(void);
void USS_Timer3_Init(void);
void USSToggleSensorEnable(void);
int USSGetDistance(void);
void EXIT15_10IRQHandler(void);
#endif
