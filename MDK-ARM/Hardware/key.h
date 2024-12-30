#ifndef _KEY_H
#define _KEY_H

#define IF_KEY_UP_PRSD    !HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin)
#define IF_KEY_DOWN_PRSD  !HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin)
#define IF_KEY_ENC_PRSD   HAL_GPIO_ReadPin(KEY_ENC_GPIO_Port, KEY_ENC_Pin)

#endif
