#include <Kinect.h>
#include "stdafx.h"
#pragma once
class KinectData
{
	static const int        cDepthWidth  = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth  = 1920;
	static const int        cColorHeight = 1080;

private:
	//kinect 
	IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	DepthSpacePoint*        m_pDepthCoordinates;
	RGBQUAD*                m_pOutputRGBX; 
	RGBQUAD*                m_pBackgroundRGBX; 
	RGBQUAD*                m_pColorRGBX;
	UINT16*                 pDepthBuffer;
	BYTE*                   pBodyIndexBuffer;
	RGBQUAD*                pColorBuffer;
// size width height
	HWND                    m_hWnd;
	INT64                   m_nStartTime;
	INT64                   m_nLastCounter;
	double                  m_fFreq;
	INT64                   m_nNextStatusTime;
	DWORD                   m_nFramesSinceUpdate;
	bool                    m_bSaveScreenshot;
	INT64                   nDepthTime;
	UINT                    nDepthBufferSize;
	UINT                    nColorBufferSize;
	int                     nBodyIndexWidth;
    int                     nBodyIndexHeight;
	UINT                    nBodyIndexBufferSize; 


	int nDepthWidth ;
	int nDepthHeight ;
	int nColorWidth ;
	int nColorHeight ;
	// Frame reader
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;
	void ProcessJoints();
public:
	KinectData(void);
	~KinectData(void);
	HRESULT KinectData::InitKinect();
	void KinectData::ProcessFrame(INT64 nTime, 
		const UINT16* pDepthBuffer, int nDepthWidth, int nDepthHeight, 
		const RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight,
		const BYTE* pBodyIndexBuffer, int nBodyIndexWidth, int nBodyIndexHeight);
	void KinectData::Update();
	void KinectData::check_deth_frame();
	int  KinectData::getColorHeight()  {return cColorHeight ;}
	int  KinectData::getColorWidth()   {return cColorWidth ;}
	int  KinectData::getDepthWidth()   {return cDepthWidth ;}
	int  KinectData::getDepthHeight()  {return cDepthHeight ;}
	int  KinectData::getnColorWidth()  {return nColorWidth ;}
	int  KinectData::getnColorHeight()  {return nColorHeight ;}
	int  KinectData::getnDepthWidth()  {return nDepthWidth ;}
	int  KinectData::getnDepthHeight()  {return nDepthHeight ;}
	RGBQUAD * KinectData::Getm_pBackgroundRGBX(){return m_pBackgroundRGBX;}
	RGBQUAD * KinectData::GetColorRGBX(){return m_pColorRGBX;}
	RGBQUAD*  KinectData::GetOutputRGBX(){return  m_pOutputRGBX;}
	UINT16*   KinectData::GetDepthBuffer(){return   pDepthBuffer;}
	BYTE*     KinectData::GetpBodyIndexBuffer(){return  pBodyIndexBuffer;}
};

