#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* */
#include <stdint.h>

/* */
#include "stm32f1xx_hal.h"
#include "tfmini.h"
#include "lidar.h"
#include "responses.h"
#include "commands.h"

void error_handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
