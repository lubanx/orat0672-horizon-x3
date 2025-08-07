/***************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2019 Horizon Robotics.
 * All rights reserved.
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

// #include "inc/logging.h"
#include "logging.h"
#include "hb_cam_utility.h"
#include "inc/orat0672_setting.h"
#include "inc/sensor_effect_common.h"

#define ORAT0672_REG_CHIP_ID (0xA0A4)
#define CHIP_IRS2875_ID (0x2875)

int orat0672_linear_data_init(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	uint32_t open_cnt = 0;
	sensor_turning_data_t turning_data;
	uint32_t *stream_on = turning_data.stream_ctrl.stream_on;
	uint32_t *stream_off = turning_data.stream_ctrl.stream_off;

	memset(&turning_data, 0, sizeof(sensor_turning_data_t));

	// common data
	turning_data.bus_num = sensor_info->bus_num;
	turning_data.bus_type = sensor_info->bus_type;
	turning_data.port = sensor_info->port;
	turning_data.reg_width = sensor_info->reg_width;
	turning_data.mode = sensor_info->sensor_mode;
	turning_data.sensor_addr = sensor_info->sensor_addr;
	strncpy(turning_data.sensor_name, sensor_info->sensor_name,
			sizeof(turning_data.sensor_name));

	return ret;
}

int sensor_poweron(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	int gpio = 0;

	printf("orat0672 sensor_poweron\n");

	for (gpio = 0; gpio < sensor_info->gpio_num; gpio++)
	{
		if (sensor_info->gpio_pin[gpio] >= 0)
		{
			printf("gpio_num %d  %d %d\n", sensor_info->gpio_num, sensor_info->gpio_pin[gpio], sensor_info->gpio_level[gpio]);
			printf("power_delay %d\n", sensor_info->power_delay);
			ret = camera_power_ctrl(sensor_info->gpio_pin[gpio], sensor_info->gpio_level[gpio]);
			usleep(sensor_info->power_delay * 1000);
			ret |= camera_power_ctrl(sensor_info->gpio_pin[gpio], 1 - sensor_info->gpio_level[gpio]);
			if (ret < 0)
			{
				pr_err("camera_power_ctrl fail\n");
				return -HB_CAM_SENSOR_POWERON_FAIL;
			}
			usleep(5 * 1000);
		}
	}

	return ret;
}

int orat0672_read_register(int bus, uint8_t i2c_addr, uint16_t reg)
{
	int ret = RET_OK;
	uint16_t val;
	if (!i2c_addr)
	{
		pr_err("i2c addr not set\n");
		return -RET_ERROR;
	}
	val = hb_i2c_read_reg16_data16(bus, i2c_addr, reg);
	printf("orat0672 read reg=%x,data=%x\n", reg, val);

	return val;
}

int sensor_init(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	int setting_size = 0;

	/*======sensor_poweron======*/
	ret = sensor_poweron(sensor_info);
	if (ret < 0)
	{
		pr_err("sensor_poweron %s fail\n", sensor_info->sensor_name);
		return ret;
	}

	printf("**** orat0672 sensor_init ****\n");
	usleep(10000); // 10ms

	ret = orat0672_read_register(sensor_info->bus_num, sensor_info->sensor_addr, ORAT0672_REG_CHIP_ID);
	if (ret == CHIP_IRS2875_ID)
		printf("Detected orat0672 sensor(%06x)!\n", ret);
	else
		printf("Unexpected sensor id (%06x)\n", ret);

	ret = orat0672_linear_data_init(sensor_info);
	if (ret < 0)
	{
		pr_debug("%d : linear data init %s fail\n", __LINE__, sensor_info->sensor_name);
		return ret;
	}

	return ret;
}

int sensor_start(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	int setting_size = 0;

	printf("**** orat0672 sensor_start ****\n");
	orat0672_read_register(sensor_info->bus_num, sensor_info->sensor_addr, 0x9400);

	setting_size = sizeof(orat0672_stream_on_setting) / sizeof(uint16_t) / 2;

	for (int i = 0; i < setting_size; i++)
	{
		ret = hb_i2c_write_reg16_data16(sensor_info->bus_num,
										sensor_info->sensor_addr, orat0672_stream_on_setting[i * 2],
										orat0672_stream_on_setting[i * 2 + 1]);
		if (ret < 0)
		{
			printf("%d : init %s fail\n", __LINE__, sensor_info->sensor_name);
			return ret;
		}
	}

	printf("**** orat0672 stream_on ****\n");
	orat0672_read_register(sensor_info->bus_num, sensor_info->sensor_addr, 0x9400);

	return ret;
}

int sensor_stop(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	int setting_size = 0;

	printf("orat0672 sensor_stop\n");

	setting_size = sizeof(orat0672_stream_off_setting) / sizeof(uint16_t) / 2;
	for (int i = 0; i < setting_size; i++)
	{
		ret = hb_i2c_write_reg16_data16(sensor_info->bus_num,
										sensor_info->sensor_addr, orat0672_stream_off_setting[i * 2],
										orat0672_stream_off_setting[i * 2 + 1]);
		if (ret < 0)
		{
			printf("%d : init %s fail\n", __LINE__, sensor_info->sensor_name);
			return ret;
		}
	}

	// orat0672_read_register(sensor_info->bus_num, sensor_info->sensor_addr,0x9400);

	return ret;
}

int sensor_poweroff(sensor_info_t *sensor_info)
{
	int ret = RET_OK;

	printf("orat0672  sensor_poweroff \n");

	ret = sensor_deinit(sensor_info);

	return ret;
}

int sensor_deinit(sensor_info_t *sensor_info)
{
	int ret = RET_OK;
	int gpio = 0;

	printf("orat0672  sensor_deinit \n");

	for (gpio = 0; gpio < sensor_info->gpio_num; gpio++)
	{
		if (sensor_info->gpio_pin[gpio] != -1)
		{
			ret = camera_power_ctrl(sensor_info->gpio_pin[gpio], sensor_info->gpio_level[gpio]);
			if (ret < 0)
			{
				printf("camera_power_ctrl fail\n");
				return -HB_CAM_SENSOR_POWERON_FAIL;
			}
			usleep(5 * 1000);
		}
	}

	return ret;
}

sensor_module_t orat0672 = {
	.module = "orat0672",
	.init = sensor_init,
	.start = sensor_start,
	.stop = sensor_stop,
	.deinit = sensor_deinit,
	.power_on = sensor_poweron,
	.power_off = sensor_poweroff,
};
