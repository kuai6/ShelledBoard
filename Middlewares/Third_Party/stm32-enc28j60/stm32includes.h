#ifndef __STM32INCLUDES_H
#define __STM32INCLUDES_H

#if STM32F091xC | STM32F030x6
/*#	if STM32F091xC
#		include "stm32f091xc.h"
#	endif*/
#	include "stm32f0xx.h"
#	include "stm32f0xx_hal_def.h"
#	include "stm32f0xx_hal_spi.h"
#endif
#if STM32F103xB | STM32F103xE
//#	include "stm32f103xb.h"
#	include "stm32f1xx.h"
#	include "stm32f1xx_hal_def.h"
#	include "stm32f1xx_hal_spi.h"
#endif

#endif
