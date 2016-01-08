/* ofxffmv.h
*
*  Created on 12/03/11.
*  Copyright 2011 NUI Group. All rights reserved.
*  Author: Anatoly Churikov
*
*/
#ifndef OFXGS3_H_INCLUDED
#define OFXGS3_H_INCLUDED

#include "ofxCameraBase.h"
//#include "pgrflycapture.h"
//#include "pgrcameragui.h"
#include "ofxXMLSettings.h"

class ofxgs3 : ofxCameraBase
{
public:
	ofxgs3();
	~ofxgs3();
	void setCameraFeature(CAMERA_BASE_FEATURE featureCode, int firstValue, int secondValue, bool isAuto, bool isEnabled);
	void getCameraFeature(CAMERA_BASE_FEATURE featureCode, int* firstValue, int* secondValue, bool* isAuto, bool* isEnabled, int* minValue, int* maxValue);
	int getCameraBaseCount();
	GUID* getBaseCameraGuids(int* camCount);
	CAMERA_BASE_FEATURE* getSupportedFeatures(int* featuresCount);
	void callSettingsDialog();
protected:
	void getNewFrame(unsigned char* newFrame);
	void cameraInitializationLogic();
	void cameraDeinitializationLogic();
	void setCameraType();
private:
};
#endif // OFXGS3_H_INCLUDED