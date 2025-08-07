/***************************************************************************
* COPYRIGHT NOTICE
* Copyright 2019 Horizon Robotics, Inc.
* All rights reserved.
***************************************************************************/
#ifndef UTILITY_SENSOR_INC_ORAT0672_SETTING_H_
#define UTILITY_SENSOR_INC_ORAT0672_SETTING_H_

#ifdef __cplusplus
extern "C" {
#endif

static uint16_t orat0672_stream_on_setting[] = {
    0x9400, 0x0001,	 /* stream on */
};

static uint16_t orat0672_stream_off_setting[] = {
    0x9400, 0x0000,	 /* stream off */
};


#ifdef __cplusplus
}
#endif

#endif  //UTILITY_SENSOR_INC_ORAT0672_SETTING_H_

