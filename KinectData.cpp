#include "KinectData.h"
#include "stdafx.h"

#include <strsafe.h>
#include <math.h>
#include <limits>
#include <Wincodec.h>

KinectData::KinectData(void):
	m_hWnd(NULL),
	m_nStartTime(0),
	m_nLastCounter(0),
	m_nFramesSinceUpdate(0),
	m_fFreq(0),
	m_nNextStatusTime(0LL),
	m_bSaveScreenshot(false),
	m_pKinectSensor(NULL),
	m_pCoordinateMapper(NULL),
	m_pMultiSourceFrameReader(NULL),
	m_pDepthCoordinates(NULL),
	m_pOutputRGBX(NULL),
	m_pBackgroundRGBX(NULL),
	m_pColorRGBX(NULL),
    nDepthWidth(0) ,
    nDepthHeight(0) ,
    nColorWidth(0) ,
    nColorHeight(0) 
{

	// create heap storage for composite image pixel data in RGBX format
	m_pOutputRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	// create heap storage for background image pixel data in RGBX format
	m_pBackgroundRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	// create heap storage for color pixel data in RGBX format
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];

	// create heap storage for the coorinate mapping from color to depth
	m_pDepthCoordinates = new DepthSpacePoint[cColorWidth * cColorHeight];
}


KinectData::~KinectData(void)
{
	if (m_pOutputRGBX)
	{
		delete [] m_pOutputRGBX;
		m_pOutputRGBX = NULL;
	}

	if (m_pBackgroundRGBX)
	{
		delete [] m_pBackgroundRGBX;
		m_pBackgroundRGBX = NULL;
	}

	if (m_pColorRGBX)
	{
		delete [] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}

	if (m_pDepthCoordinates)
	{
		delete[] m_pDepthCoordinates;
		m_pDepthCoordinates = NULL;
	}

	// done with frame reader
	SafeRelease(m_pMultiSourceFrameReader);

	// done with coordinate mapper
	SafeRelease(m_pCoordinateMapper);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}
HRESULT KinectData::InitKinect()
{

	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the frame reader

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | 
				FrameSourceTypes::FrameSourceTypes_BodyIndex|FrameSourceTypes::FrameSourceTypes_Body,
				&m_pMultiSourceFrameReader);
		}
	}

	if (!m_pKinectSensor || FAILED(hr))
	{

		return E_FAIL;
	}

	return hr;
}
void KinectData::Update()
{
	if (!m_pMultiSourceFrameReader)
	{
		return;
	}
	// depth color bodyindex body  frame
	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IColorFrame* pColorFrame = NULL;
	IBodyIndexFrame* pBodyIndexFrame = NULL;
	// get the laest frame

	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);

	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;
		// get depthframe
		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);			
		}

		SafeRelease(pDepthFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}

		SafeRelease(pColorFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);
		}

		SafeRelease(pBodyIndexFrameReference);
	}


	if (SUCCEEDED(hr))
	{
		IFrameDescription* pDepthFrameDescription = NULL;
		IFrameDescription* pColorFrameDescription = NULL;
		ColorImageFormat imageFormat = ColorImageFormat_None;
		IFrameDescription* pBodyIndexFrameDescription = NULL;
		// get depth frame data
		hr = pDepthFrame->get_RelativeTime(&nDepthTime);

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Width(&nDepthWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameDescription->get_Height(&nDepthHeight);
		}

		if (SUCCEEDED(hr))
		{
			nDepthBufferSize=nDepthHeight*nDepthWidth*sizeof(UINT16);
			hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);    	
			pDepthFrame->CopyFrameDataToArray(nDepthBufferSize,reinterpret_cast<UINT16*>(pDepthBuffer));
		}

		// get color frame data

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Width(&nColorWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameDescription->get_Height(&nColorHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
			}
			else if (m_pColorRGBX)
			{
				pColorBuffer = m_pColorRGBX;
				nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}

		// get body index frame data

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->get_FrameDescription(&pBodyIndexFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Width(&nBodyIndexWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameDescription->get_Height(&nBodyIndexHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);  
			nBodyIndexBufferSize=nBodyIndexHeight*nBodyIndexWidth*sizeof(BYTE);
			pBodyIndexFrame->CopyFrameDataToArray(nBodyIndexBufferSize,reinterpret_cast<BYTE*>(pBodyIndexBuffer));
		}

		if (SUCCEEDED(hr))
		{
			ProcessFrame(nDepthTime, pDepthBuffer, nDepthWidth, nDepthHeight, 
				pColorBuffer, nColorWidth, nColorHeight,
				pBodyIndexBuffer, nBodyIndexWidth, nBodyIndexHeight);
		}

		SafeRelease(pDepthFrameDescription);
		SafeRelease(pColorFrameDescription);
		SafeRelease(pBodyIndexFrameDescription);
	}

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pMultiSourceFrame);
	
}
void KinectData::ProcessJoints()
{
	IBodyFrame*   pBodyframe = NULL ;
	// get joint 
	IBody * ppbodies[BODY_COUNT] ={0};
	HRESULT hr ;
	hr=pBodyframe->GetAndRefreshBodyData(BODY_COUNT,ppbodies);
	if (hr)
	{
		for ( int i = 0 ; i < BODY_COUNT ; i++)
	    {
		   IBody * pbody= ppbodies[i];
		   boolean   bTracked = false;
		   hr=pbody->get_IsTracked(&bTracked);
	     	if (  SUCCEEDED(hr) && bTracked)
	     	{
			    Joint joints[JointType_Count];


				HandState  leftHandState = HandState_Unknown;
				HandState  rightHandState = HandState_Unknown ;
				pbody->get_HandLeftState(&leftHandState);
				pbody->get_HandRightState(&rightHandState) ;

 		    	hr = pbody->GetJoints(_countof(joints),joints);
				if (SUCCEEDED(hr))
				{
		    	for(int j = 0 ; j < JointType_Count ; j++)
		     	  {
                      
		    	  }
				}

		     }

	    }
	}
	// get 25 joints 
	
	SafeRelease(pBodyframe);
}
void KinectData::check_deth_frame()
{
}

RGBQUAD * KinectData::getColorRGBX()
{
	return m_pColorRGBX;
}
void KinectData::ProcessFrame(INT64 nTime, 
							  const UINT16* pDepthBuffer, int nDepthWidth, int nDepthHeight, 
							  const RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight,
							  const BYTE* pBodyIndexBuffer, int nBodyIndexWidth, int nBodyIndexHeight)
{
	if (m_hWnd)
	{
		// handle fps  
		if (!m_nStartTime)
		{
			m_nStartTime = nTime;
		}
		double fps = 0.0;
		LARGE_INTEGER qpcNow = {0};
		if (m_fFreq)
		{
			if (QueryPerformanceCounter(&qpcNow))
			{
				if (m_nLastCounter)
				{
					m_nFramesSinceUpdate++;
					fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
				}
			}
		}
		WCHAR szStatusMessage[64];
		StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));
	}
	// Make sure we've received valid data
	if (m_pCoordinateMapper && m_pDepthCoordinates && m_pOutputRGBX && 
		pDepthBuffer && (nDepthWidth == cDepthWidth) && (nDepthHeight == cDepthHeight) && 
		pColorBuffer && (nColorWidth == cColorWidth) && (nColorHeight == cColorHeight) &&
		pBodyIndexBuffer && (nBodyIndexWidth == cDepthWidth) && (nBodyIndexHeight == cDepthHeight))
	{
		HRESULT hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(nDepthWidth * nDepthHeight, (UINT16*)pDepthBuffer, nColorWidth * nColorHeight, m_pDepthCoordinates);
		if (SUCCEEDED(hr))
		{
			// loop over output pixels
			for (int colorIndex = 0; colorIndex < (nColorWidth*nColorHeight); ++colorIndex)
			{
				// default setting source to copy from the background pixel
				const RGBQUAD* pSrc = m_pBackgroundRGBX + colorIndex;

				DepthSpacePoint p = m_pDepthCoordinates[colorIndex];

				// Values that are negative infinity means it is an invalid color to depth mapping so we
				// skip processing for this pixel
				if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
				{
					int depthX = static_cast<int>(p.X + 0.5f);
					int depthY = static_cast<int>(p.Y + 0.5f);

					if ((depthX >= 0 && depthX < nDepthWidth) && (depthY >= 0 && depthY < nDepthHeight))
					{
						BYTE player = pBodyIndexBuffer[depthX + (depthY * cDepthWidth)];

						// if we're tracking a play current pixel, draw from the color camera
						if (player != 0xff)
						{
							// set source for copy to the color pixel
							pSrc = m_pColorRGBX + colorIndex;
						}
					}
				}
				// write output
				m_pOutputRGBX[colorIndex] = *pSrc;
			}
			// Draw the data with Direct2D
			//m_pDrawCoordinateMapping->Draw(reinterpret_cast<BYTE*>(m_pOutputRGBX), cColorWidth * cColorHeight * sizeof(RGBQUAD));				
		}
	}
}