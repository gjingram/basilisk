/*
 ISC License

 Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */
/*
    Planet Limb Finding

    Note:   This module takes an image and writes out a message containing all the pixel points that are on the limb.
    Author: Thibaud Teil
    Date:   September 16, 2019
 
 */

/* modify the path to reflect the new module names */
#include <string.h>
#include "limbFinding.h"


/*! The constructor for the HoughCircles module. It also sets some default values at its creation.  */
LimbFinding::LimbFinding()
{
    this->filename = "";
    this->saveDir ="";
    this->saveImages = 0;
    this->blurrSize = 3;
    this->cannyThreshHigh = 200;
    this->cannyThreshLow = 100;
    this->OutputBufferCount = 2;
    this->limbNumThresh = 50;

}


/*! @brief
 \verbatim embed:rst
    This method creates the module output message of type :ref:`LimbOpNavMsg`.
 \endverbatim
 @return void
 */

void LimbFinding::SelfInit()
{
    /*! - Create output message for module */
    this->opnavLimbOutMsgID = SystemMessaging::GetInstance()->CreateNewMessage(this->opnavLimbOutMsgName,sizeof(LimbOpNavMsg),this->OutputBufferCount,"LimbOpNavMsg",moduleID);
}


/*! CrossInit performs the second stage of initialization for this module.
 It's primary function is to link the input messages that were created elsewhere.
 @return void
 */
void LimbFinding::CrossInit()
{
    /*! - Get the image data message ID*/
    this->imageInMsgID = SystemMessaging::GetInstance()->subscribeToMessage(this->imageInMsgName,sizeof(CameraImageMsg), moduleID);
}

/*! This is the destructor */
LimbFinding::~LimbFinding()
{
    return;
}


/*! This method performs a complete reset of the module.  Local module variables that retain time varying states between function calls are reset to their default values.
 @return void
 @param CurrentSimNanos The clock time at which the function was called (nanoseconds)
 */
void LimbFinding::Reset(uint64_t CurrentSimNanos)
{
    return;
}

/*! This module reads an OpNav image and extracts limb points from its content using OpenCV's Canny Transform. It performs a greyscale, and blur on the image to facilitate edge-detection.
 @return void
 @param CurrentSimNanos The clock time at which the function was called (nanoseconds)
 */
void LimbFinding::UpdateState(uint64_t CurrentSimNanos)
{
    std::string dirName;
    CameraImageMsg imageBuffer;
    LimbOpNavMsg limbMsg;
    memset(&imageBuffer, 0x0, sizeof(CameraImageMsg));
    memset(&limbMsg, 0x0, sizeof(LimbOpNavMsg));

    cv::Mat imageCV, blurred, edgeImage;
    if (this->saveDir != ""){
        dirName = this->saveDir + std::to_string(CurrentSimNanos*1E-9) + ".jpg";
    }
    else{dirName = "./"+ std::to_string(CurrentSimNanos*1E-9) + ".jpg";}
    
    /*! - Read in the bitmap*/
    SingleMessageHeader localHeader;
    if(this->imageInMsgName != "")
    {
        SystemMessaging::GetInstance()->ReadMessage(this->imageInMsgID, &localHeader,
                                                    sizeof(CameraImageMsg), reinterpret_cast<uint8_t*>(&imageBuffer), this->moduleID);
        this->sensorTimeTag = localHeader.WriteClockNanos;
    }
    /* Added for debugging purposes*/
    if (!this->filename.empty()){
        imageCV = imread(this->filename, cv::IMREAD_COLOR);
    }
    else if(imageBuffer.valid == 1 && imageBuffer.timeTag >= CurrentSimNanos){
        /*! - Recast image pointer to CV type*/
        std::vector<unsigned char> vectorBuffer((char*)imageBuffer.imagePointer, (char*)imageBuffer.imagePointer + imageBuffer.imageBufferLength);
        imageCV = cv::imdecode(vectorBuffer, cv::IMREAD_COLOR);
        if (this->saveImages == 1){
            cv::imwrite(dirName, imageCV);
        }
    }
    else{
        /*! - If no image is present, write zeros in message */
        SystemMessaging::GetInstance()->WriteMessage(this->opnavLimbOutMsgID, CurrentSimNanos, sizeof(LimbOpNavMsg), reinterpret_cast<uint8_t *>(&limbMsg), this->moduleID);
        return;}
    /*! - Greyscale the image */
    cv::cvtColor( imageCV, imageCV, cv::COLOR_BGR2GRAY);
    /*! - Lightly blur it */
    cv::GaussianBlur(imageCV, blurred, cv::Size(this->blurrSize,this->blurrSize), 1);
    /*! - Apply the Canny Transform to find the limbPoints*/
    cv::Canny(blurred, edgeImage, this->cannyThreshLow, this->cannyThreshHigh,  3, true);
    if (cv::countNonZero(edgeImage)>this->limbNumThresh){
        std::vector<cv::Point2i> locations;
        cv::findNonZero(edgeImage, locations);
        limbMsg.numLimbPoints =0;
        for(size_t i = 0; i<locations.size() && i<MAX_LIMB_PNTS; i++ )
        {
            /*! - Store the non zero pixels of the canny transform and count them*/
            limbMsg.limbPoints[2*i] = locations[i].x;
            limbMsg.limbPoints[2*i+1] = locations[i].y;
            limbMsg.numLimbPoints += 1;
        }
        limbMsg.valid = 1;
        limbMsg.planetIds = 2;
    }
    
    limbMsg.timeTag = this->sensorTimeTag;
    limbMsg.cameraID = imageBuffer.cameraID;

    SystemMessaging::GetInstance()->WriteMessage(this->opnavLimbOutMsgID, CurrentSimNanos, sizeof(LimbOpNavMsg), reinterpret_cast<uint16_t *>(&limbMsg), this->moduleID);

    return;
}

