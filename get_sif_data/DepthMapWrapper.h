/******************************************************************************
 *
 *COPYRIGHT(c) 2022 by OMS,Inc。
 *
 *This file belongs to OMS,Inc。It is considered a trade secret,
 *and is not to be divulged or used by parties who have not
 *received written authorization from the owner。
 *
 *****************************************************************************/
#ifndef __DEPTHMAP_WRAPPER_H__
#define __DEPTHMAP_WRAPPER_H__

//==============================================================================
// Included modules
//==============================================================================
#include <stdint.h>
#include <stdbool.h>

//==============================================================================
// MACROS
//==============================================================================
#ifdef _MSC_VER
#define CP_DLL_PUBLIC __declspec(dllexport)
#else
#define CP_DLL_PUBLIC __attribute__((visibility("default")))
#endif

#define IMG_UNUSED(x) (void)(x)

typedef enum
{
  TOFMODE_STERO_15FPS = 0,
} TOFMODE2875;

typedef enum
{
  IRS2381C,
  IRS2381C_MTP006,
  IRS2875C,
  IRS2875C_LIOW2,
  IRS2875C_OZT338B,
  IRS2875C_OZT613,
  IRS2875C_OZT613_20,
  IRS2875C_OZT797,
  IRS2875C_Pinocchio,
  IRS2875C_Pinocchio_EVO3,
  IRS2877A_OZT526,
  IRS2877C,
  IRS2877C_OZT224,
} SUPPORTMODULE;

typedef struct pc_pack
{
  float X;
  float Y;
  float Z;
  float c; // Confidence  范围0~1
} pc_pkt_t;

typedef struct
{
  float principalPoint[2];       //!< cx/cy
  float focalLength[2];          //!< fx/fy
  float distortionTangential[2]; //!< p1/p2
  float distortionRadial[3];     //!< k1/k2/k3
} WrapperDepthCamLensParameters;

// Class factories
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   SDK参数设置
 * 
 * @param[in]   i2c_name      i2c设备名
 * @param[in]   spcFilePath   标定文件路径
 * @param[in]   module        模组类型
 *
 * @return 无
*/
CP_DLL_PUBLIC
void DepthMapWrapperSetUp(char* i2c_name, char* spcFilePath, SUPPORTMODULE module);

/**
 * @brief   SDK注销
 * 
 * @return  无
*/
CP_DLL_PUBLIC
void DepthMapWrapperTearDown(); 

/**
 * @brief   SDK开始工作
 * 
 * @return  执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperStartCapture();

/**
 * @brief   SDK停止工作
 * 
 * @return  执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperStopCapture();

/**
 * @brief   设置SDK滤波等级
 * 
 * @return 执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperFilterLevel();	

/**
 * @brief   获取模组标定文件
 * 
 * @param[in]   i2c_name      i2c设备名
 * @param[in]   caliFilePath  标定文件路径
 * 
 * @return 执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperGetCalibrationData(char *i2cName, char *spcFilePath);	

/**
 * @brief   获取镜头参数
 * 
 * @param[out]   params      获取的镜头参数
 *
 * @return 执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC 
bool DepthMapWrapperGetLensParameters(WrapperDepthCamLensParameters *params);

/**
 * @brief   设置UseCase
 * 
 * @param[in]   ind      usecase编号
 *
 * @return 执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperSetUseCase(int ind);


/**
 * @brief   手动设置曝光时间
 * 
 * @param[in]   exposureTime     曝光时间
 *
 * @return 执行结果  true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperSetExposureTime(uint32_t exposureTime);

/**
 * @brief  更新自动曝光时间到模组
 * 
 * @return 自动曝光时间
*/
CP_DLL_PUBLIC
uint32_t DepthMapWrapperUpdateAutoExposureTime();

/**
 * @brief   自动曝光是否使能
 * 
 * @param[in]   aeState   true：自动曝光  false：手动曝光     
 *
 * @return 执行结果 true为成功 false为失败
*/
CP_DLL_PUBLIC
bool DepthMapWrapperSetAutoExposureEnabled(bool aeState);

/**
 * @brief   开始录制rrf文件
 * 
 * @param[in]   file_path      rrf文件存放路径
 * @param[in]   ind                 usecase编号
 *
 * @return 无
*/
CP_DLL_PUBLIC
void DepthMapWrapperStartRecRrf(const char* file_path, int ind);

/**
 * @brief   停止录制rrf文件
 *
 * @return 无
*/
CP_DLL_PUBLIC
void DepthMapWrapperStopRecRrf();

/**
 * @brief  tof数据解析
 * 
 * @param[in]    in_data       原始raw数据
 * @param[out]   ir_image      解析后的IR图数据
 * @param[out]   depth_image   解析后的深度图数据
 * @param[out]   pcloud_image  解析后的点云数据
 * @param[out]   pt_count      点云数据大小
 * 
 * @return 无
*/
CP_DLL_PUBLIC
void DepthMapWrapperProcessFrame(char* in_data,  char* ir_image, char* depth_image, char* pcloud_image, unsigned int* pt_count);
 

#ifdef __cplusplus
}
#endif

#endif //__DEPTHMAP_WRAPPER_H__
