#ifndef _beep_H
#define _beep_H

#include "system.h"

/*  蜂鸣器时钟端口、引脚定义 */


#define BP PAout(7)

void BEEP_Init(void);

#endif
