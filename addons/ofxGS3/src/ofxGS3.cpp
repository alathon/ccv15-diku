#include <iostream>
#include <sstream>
#include "FlyCapture2.h"
#include "FlyCapture2Defs.h"
#include "ofxGS3.h"
#include <ofxFFMV/src/pgrflycapture.h>

using namespace FlyCapture2;
using namespace std;

ofxgs3::ofxgs3()
{
}

ofxgs3::~ofxgs3()
{
	deinitializeCamera();
}

void ofxgs3::callSettingsDialog()
{
	// TODO:
	//pgrcamguiInitializeSettingsDialog(cameraContext, guiContext);
}

CAMERA_BASE_FEATURE* ofxgs3::getSupportedFeatures(int* featuresCount)
{
	*featuresCount = 7;
	CAMERA_BASE_FEATURE* features = (CAMERA_BASE_FEATURE*)malloc(*featuresCount * sizeof(CAMERA_BASE_FEATURE));
	features[0] = BASE_BRIGHTNESS;
	features[1] = BASE_EXPOSURE;
	features[2] = BASE_SATURATION;
	features[3] = BASE_GAMMA;
	features[4] = BASE_SHUTTER;
	features[5] = BASE_GAIN;
	features[6] = BASE_FRAMERATE;
	return features;
}

void ofxgs3::setCameraFeature(CAMERA_BASE_FEATURE featureCode, int firstValue, int secondValue, bool isAuto, bool isEnabled)
{
	PropertyType deviceProperty = (PropertyType)0xFFFFFFFF;
	//FlyCaptureProperty deviceProperty = (FlyCaptureProperty)0xFFFFFFFF;
	switch (featureCode)
	{
	case BASE_BRIGHTNESS:
		deviceProperty = BRIGHTNESS;
		break;
	case BASE_EXPOSURE:
		deviceProperty = AUTO_EXPOSURE;
		break;
	case BASE_SATURATION:
		deviceProperty = SATURATION;
		break;
	case BASE_GAMMA:
		deviceProperty = GAMMA;
		break;
	case BASE_SHUTTER:
		deviceProperty = SHUTTER;
		break;
	case BASE_GAIN:
		deviceProperty = GAIN;
		break;
	case BASE_FRAMERATE:
		deviceProperty = FRAME_RATE;
		framerate = firstValue;
		break;
	}
	if (deviceProperty != 0xFFFFFFFF)
	{
		// TODO: Use Camera.setProperty() to set property (See Camera.h)
		// Old code: flycaptureSetCameraPropertyEx(cameraContext, deviceProperty, true, isEnabled, isAuto, firstValue, secondValue);
	}
}

void ofxgs3::getCameraFeature(CAMERA_BASE_FEATURE featureCode, int* firstValue, int* secondValue, bool* isAuto, bool* isEnabled, int* minValue, int* maxValue)
{
	*firstValue = 0;
	*secondValue = 0;
	*isAuto = false;
	*isEnabled = false;
	*minValue = 0;
	*maxValue = 0;
	PropertyType deviceProperty = (PropertyType)0xFFFFFFFF;
	//FlyCaptureProperty deviceProperty = (FlyCaptureProperty)0xFFFFFFFF;
	switch (featureCode)
	{
	case BASE_BRIGHTNESS:
		deviceProperty = BRIGHTNESS;
		break;
	case BASE_EXPOSURE:
		deviceProperty = AUTO_EXPOSURE;
		break;
	case BASE_SATURATION:
		deviceProperty = SATURATION;
		break;
	case BASE_GAMMA:
		deviceProperty = GAMMA;
		break;
	case BASE_SHUTTER:
		deviceProperty = SHUTTER;
		break;
	case BASE_GAIN:
		deviceProperty = GAIN;
		break;
	case BASE_FRAMERATE:
		deviceProperty = FRAME_RATE;
		break;
	}
	bool bpPresent, bpOnePush, pbReadOut, pbOnOff, pbAuto, pbManual;
	if (deviceProperty != 0xFFFFFFFF)
	{
		// TODO: Use Camera.getProperty(). See Camera.h
		//*isEnabled = (flycaptureGetCameraProperty(cameraContext, deviceProperty, (long*)firstValue, (long*)secondValue, isAuto) == FLYCAPTURE_OK);
		//flycaptureGetCameraPropertyRangeEx(cameraContext, deviceProperty, &bpPresent, &bpOnePush, &pbReadOut, &pbOnOff, &pbAuto, &pbManual, minValue, maxValue);
	}
	else
		*isEnabled = false;
}

int ofxgs3::getCameraBaseCount()
{
	unsigned int numCams = 0;
	BusManager busMgr;
	Error error;
	error = busMgr.GetNumOfCameras(&numCams);
	if (error != PGRERROR_OK)
	{
		return -1;
	}
	return numCams;
}

void ofxgs3::getNewFrame(unsigned char* newFrame)
{
	Image rawImage;
	Error error;
	error = cam.RetrieveBuffer(&rawImage);
	if (error != PGRERROR_OK)
	{
		cout << "Error grabbing frame with GS3 camera!" << endl;
		return;
	}

	// Convert from RAW8 to MONO8
	Image convertedImage;
	error = rawImage.Convert(PIXEL_FORMAT_MONO8, &convertedImage);
	if (error != PGRERROR_OK)
	{
		cout << "Error converting frame to MONO8" << endl;
		return;
	}

	memcpy(static_cast<void*>(newFrame), convertedImage.GetData(), width * height * depth * sizeof(unsigned char));
	/*flycaptureGrabImage2(cameraContext, &fcImage);
	memcpy((void*)newFrame, fcImage.pData, width * height * depth * sizeof(unsigned char));*/
}

void ofxgs3::setCameraType()
{
	cameraType = GS3;
	cameraTypeName = "GS3";
}

void ofxgs3::cameraInitializationLogic()
{
	FlyCaptureInfoEx* tempArInfo = new FlyCaptureInfoEx[_MAX_CAMERAS_];
	unsigned int camNum = _MAX_CAMERAS_;
	flycaptureBusEnumerateCamerasEx(tempArInfo, &camNum);
	for (int i = 0; i<camNum; i++)
	{
		if ((unsigned long)tempArInfo[i].SerialNumber == guid.Data1)
			index = i;
	}
	arInfo = tempArInfo[index];
	flycaptureCreateContext(&cameraContext);
	flycaptureInitialize(cameraContext, index);
	flycaptureStartCustomImage(cameraContext, (unsigned int)cameraPixelMode, left, top, width, height, 100, depth == 1 ? (arInfo.CameraType == FLYCAPTURE_COLOR ? FLYCAPTURE_RAW8 : FLYCAPTURE_MONO8) : FLYCAPTURE_RGB8);
	flycaptureGrabImage2(cameraContext, &fcImage);
	width = fcImage.iCols;
	height = fcImage.iRows;
	depth = (width*height != 0) ? fcImage.iRowInc / fcImage.iRows : 0;
	flycaptureSetColorProcessingMethod(cameraContext, depth == 1 ? FLYCAPTURE_DISABLE : FLYCAPTURE_NEAREST_NEIGHBOR_FAST);
	for (int i = 0; i<cameraBaseSettings->propertyType.size(); i++)
		setCameraFeature(cameraBaseSettings->propertyType[i], cameraBaseSettings->propertyFirstValue[i], cameraBaseSettings->propertySecondValue[i], cameraBaseSettings->isPropertyAuto[i], cameraBaseSettings->isPropertyOn[i]);
	pgrcamguiCreateContext(&guiContext);
	delete tempArInfo;
}

void ofxffmv::cameraDeinitializationLogic()
{
	flycaptureStop(cameraContext);
	flycaptureDestroyContext(cameraContext);
	pgrcamguiDestroyContext(guiContext);
}


GUID* ofxffmv::getBaseCameraGuids(int* camCount)
{
	*camCount = 0;
	unsigned int tcamNum = _MAX_CAMERAS_;
	FlyCaptureInfoEx* tArInfo = new FlyCaptureInfoEx[_MAX_CAMERAS_];
	flycaptureBusEnumerateCamerasEx(tArInfo, &tcamNum);
	*camCount = tcamNum;
	GUID* guids = (GUID*)malloc(tcamNum*sizeof(GUID));
	for (int i = 0; i<tcamNum; i++)
	{
		GUID guid2;
		guid2.Data1 = tArInfo[i].SerialNumber;
		guid2.Data2 = guid2.Data3 = 0;
		memset((void*)guid2.Data4, 0, 8 * sizeof(unsigned char));
		guids[i] = guid2;
	}
	delete tArInfo;
	return guids;
}
