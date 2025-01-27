/*
 ISC License

 Copyright (c) 2020, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

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

#include "albedo.h"

/*! Albedo module constructor
 @return void
 */
Albedo::Albedo()
{
    this->albOutMsgNames.clear();
    this->planetInMsgNames.clear();
    this->instInMsgNames.clear();
    this->sunPositionInMsgName = "";
    this->spacecraftStateInMsgName = "";
    this->sunPositionInMsgId = -1;
    this->spacecraftStateInMsgId = -1;
    this->albOutMsgIds.clear();
    this->planetMsgData.clear();
    this->dataPaths.clear();
    this->fileNames.clear();
    this->modelNames.clear();
    this->ALB_avgs.clear();
    this->REQ_planets.clear();
    this->RP_planets.clear();
    this->numLats.clear();
    this->numLons.clear();
    this->r_IB_Bs.clear();
    this->nHat_Bs.clear();
    this->fovs.clear();
    this->albOutData.clear();
    this->gdlat.clear();
    this->gdlon.clear();
    this->latDiff.clear();
    this->lonDiff.clear();
    this->ALB.clear();
    this->albArray.clear();
    this->readFile = false;
    this->AfluxAtInstrumentMax = -1.0;
    this->AfluxAtInstrument = -1.0;
    this->albedoAtInstrumentMax = -1.0;
    this->albedoAtInstrument = -1.0;
    this->numLon = 360;
    this->numLat = 180;
    this->nHat_B_default = { 1.0, 0.0, 0.0 };
    this->fov_default = 90. * D2R;
    this->OutputBufferCount = 2;
    this->eclipseCase = false;
    this->shadowFactorAtdA = 1.0;
    this->altitudeRateLimit = -1.0;
    return;
}

/*! Albedo module destructor
 @return void
 */
Albedo::~Albedo()
{
    return;
}

Config::Config() {
    this->fov = -1.0;
    this->nHat_B.fill(0.0);
    this->r_IB_B.fill(0.0);
    return;
}    

Config::~Config() {
    return;
}

/*! Adds the instrument configuration and automatically creates an output message name (overloaded function)
 @return void
 */
void Albedo::addInstrumentConfig(std::string instInMsgName, instConfig_t configMsg) {
    //! - Create albedo output message names
    std::string tmpAlbMsgName;
    this->instInMsgNames.push_back(instInMsgName);
    tmpAlbMsgName = this->ModelTag + "_" + std::to_string(this->instInMsgNames.size() - 1) + "_data";
    this->albOutMsgNames.push_back(tmpAlbMsgName);
    //! - Do a sanity check and push fov back to the vector (if not defined, use the default value.)
    if (configMsg.fov < 0.0) {
        this->fovs.push_back(this->fov_default);
        bskLogger.bskLog(BSK_WARNING, "Albedo Module (addInstrumentConfig): For the instrument (%s)'s half field of view angle (fov), the default value is used.", instInMsgName.c_str());
    } 
    else {
        this->fovs.push_back(configMsg.fov);
    }
    //! - Push r_IB_B back to the vector
    this->r_IB_Bs.push_back(configMsg.r_IB_B);
    //! - Do a sanity check and push nHat_B back to the vector (if not defined, use the default value.)
    if (!configMsg.nHat_B.isZero()) {
        this->nHat_Bs.push_back(configMsg.nHat_B / configMsg.nHat_B.norm());
    }
    else {
        this->nHat_Bs.push_back(nHat_B_default);
        bskLogger.bskLog(BSK_WARNING, "Albedo Module (addInstrumentConfig): For the instrument (%s)'s unit normal vector (nHat_B), the default vector is used.", instInMsgName.c_str());
    }
    return;
}

/*! Adds the instrument configuration and automatically creates an output message name (overloaded function)
 @return void
 */
void Albedo::addInstrumentConfig(std::string instInMsgName, double fov, Eigen::Vector3d nHat_B, Eigen::Vector3d r_IB_B) {
    //! - Create albedo output message names
    std::string tmpAlbMsgName;
    this->instInMsgNames.push_back(instInMsgName);
    tmpAlbMsgName = this->ModelTag + "_" + std::to_string(this->instInMsgNames.size() - 1) + "_data";
    this->albOutMsgNames.push_back(tmpAlbMsgName);
    //! - Do a sanity check and push fov back to the vector (if not defined, use the default value.)
    if (fov < 0.0) {
        this->fovs.push_back(this->fov_default);
        bskLogger.bskLog(BSK_WARNING, "Albedo Module (addInstrumentConfig): Instrument (%s)'s half field of view angle (fov) cannot be negative, the default value is used instead.", instInMsgName.c_str());
    }
    else {
        this->fovs.push_back(fov);
    }
    //! - Push r_IB_B back to the vector
    this->r_IB_Bs.push_back(r_IB_B);
    //! - Do a sanity check and push nHat_B back to the vector (if not defined, use the default value.)
    if (!nHat_B.isZero()) {
        this->nHat_Bs.push_back(nHat_B / nHat_B.norm());
    }
    else {
        this->nHat_Bs.push_back(nHat_B_default);
        bskLogger.bskLog(BSK_WARNING, "Albedo Module (addInstrumentConfig): Instrument (%s)'s unit normal vector (nHat_B) cannot be composed of all zeros, the default vector is used instead.", instInMsgName.c_str());
    }      
    return;
}

/*! This method adds planet name and albedo average model name (overloaded function)
 @return void
 */
void Albedo::addPlanetandAlbedoAverageModel(std::string planetSpiceName)
{
    if (std::binary_search(this->planetInMsgNames.begin(), this->planetInMsgNames.end(), planetSpiceName + "_planet_data")) {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (addPlanetandAlbedoAverageModel): Same planet cannot be added more than once!");
        return;
    }
    else {
        std::string modelName = "ALBEDO_AVG";
        this->planetInMsgNames.push_back(planetSpiceName + "_planet_data");
        this->modelNames.push_back(modelName);
        this->fileNames.push_back("");
        this->dataPaths.push_back("");
        this->numLats.push_back(-1.0);
        this->numLons.push_back(-1.0);
        double ALB_avg = getAlbedoAverage(planetSpiceName);
        this->ALB_avgs.push_back(ALB_avg);
        this->albArray.push_back(false);
    }
}

/*! This method adds planet name and albedo average model name (overloaded function)
 @return void
 */
void Albedo::addPlanetandAlbedoAverageModel(std::string planetSpiceName, double ALB_avg, int numLat, int numLon)
{
    if (std::binary_search(this->planetInMsgNames.begin(), this->planetInMsgNames.end(), planetSpiceName + "_planet_data")) {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (addPlanetandAlbedoAverageModel): Same planet cannot be added more than once!");
        return;
    }
    else {
        std::string modelName = "ALBEDO_AVG";
        this->planetInMsgNames.push_back(planetSpiceName + "_planet_data");
        this->modelNames.push_back(modelName);
        this->fileNames.push_back("");
        this->dataPaths.push_back("");
        this->numLats.push_back(numLat);
        this->numLons.push_back(numLon);
        this->ALB_avgs.push_back(ALB_avg);
        this->albArray.push_back(false);
    }
}

/*! This method adds planet name and albedo data model
 @return void
 */
void Albedo::addPlanetandAlbedoDataModel(std::string planetSpiceName, std::string dataPath, std::string fileName)
{
    if (std::binary_search(this->planetInMsgNames.begin(), this->planetInMsgNames.end(), planetSpiceName + "_planet_data")) {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (addPlanetandAlbedoDataModel): Same planet cannot be added more than once!");
        return;
    }
    else {
        std::string modelName = "ALBEDO_DATA";
        this->planetInMsgNames.push_back(planetSpiceName + "_planet_data");
        this->modelNames.push_back(modelName);
        this->fileNames.push_back(fileName);
        this->dataPaths.push_back(dataPath);
        this->numLats.push_back(-1.0);
        this->numLons.push_back(-1.0);
        this->ALB_avgs.push_back(-1.0);
        this->albArray.push_back(true);
    }
}

/*! This method initializes the object. It creates the module's output
 @return void
 */
void Albedo::SelfInit()
{
    auto messagingSystem = SystemMessaging::GetInstance();
    int64_t tmpAlbedoMsgId;
    std::vector<std::string>::iterator it;
    int id = 0;
    //! - Create all the albedo output messages for each instrument
    for (it = this->albOutMsgNames.begin(); it != this->albOutMsgNames.end(); it++) {
        tmpAlbedoMsgId = messagingSystem->CreateNewMessage(*it,
            sizeof(AlbedoSimMsg),
            this->OutputBufferCount,
            "AlbedoSimMsg",
            moduleID);
        this->albOutMsgIds.push_back(tmpAlbedoMsgId);
        id++;
    }
    return;
}

/*! This method subscribes to the spice planet states,
 spaceccraft and instrument's configuration state messages.
 @return void
 */
void Albedo::CrossInit()
{
    auto messagingSystem = SystemMessaging::GetInstance();
    /*! - subscribe to planet position message */
    std::vector<std::string>::iterator it;
    if (this->planetInMsgNames.empty()) {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (CrossInit): Planet message name vector (planetInMsgNames) is empty.");
    }
    else {
        int id = 0;
        for (it = this->planetInMsgNames.begin(); it != this->planetInMsgNames.end(); it++)
        {
            int64_t msgID = messagingSystem->subscribeToMessage((*it), sizeof(SpicePlanetStateSimMsg), this->moduleID);
            this->planetMsgData[msgID] = SpicePlanetStateSimMsg();
            id++;
        }
    }
    /*! - subscribe to sun position message */
    if (this->sunPositionInMsgName != "") {
        this->sunPositionInMsgId = messagingSystem->subscribeToMessage(this->sunPositionInMsgName,
            sizeof(SpicePlanetStateSimMsg), this->moduleID);
    }
    else {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (CrossInit): Sun message name (sunPositionInMsgName) is empty.");
    }
    /*! - subscribe to spacecraft state message */
    if (this->spacecraftStateInMsgName != "") {
        this->spacecraftStateInMsgId = messagingSystem->subscribeToMessage(this->spacecraftStateInMsgName,
            sizeof(SCPlusStatesSimMsg), this->moduleID);
    }
    else {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (CrossInit): Spacecraft message name (spacecraftStateInMsgName) is empty.");
    }
    return;
}

/*! Read Messages, calculate albedo then write it out
 @return void
 */
void Albedo::UpdateState(uint64_t CurrentSimNanos)
{
    this->readMessages();
    this->albOutData.clear();
    std::vector<std::string>::iterator instIt;
    std::map<int64_t, SpicePlanetStateSimMsg>::iterator planetIt;
    int instIdx = 0;
    int idx;
    for (instIt = this->instInMsgNames.begin(); instIt != this->instInMsgNames.end(); instIt++)
    {
        idx = 0;
        double tmpTot[4] = {};
        double outData[4] = {};
        for (planetIt = this->planetMsgData.begin(); planetIt != this->planetMsgData.end(); planetIt++)
        {
            this->computeAlbedo(idx, instIdx, planetIt->second, albArray.at(idx), outData);
            tmpTot[0] += outData[0]; tmpTot[1] += outData[1];
            tmpTot[2] += outData[2]; tmpTot[3] += outData[3];
            idx++;
        }
        this->albOutData.push_back(cArray2EigenMatrixXd(tmpTot, 4, 1));
        instIdx++;
    }
    this->writeMessages(CurrentSimNanos);
}

/*! This method resets the module
 @return void
 */
void Albedo::Reset(uint64_t CurrentSimNanos)
{
    auto messagingSystem = SystemMessaging::GetInstance();
    SingleMessageHeader tmpHeader;
    memset(&tmpHeader, 0x0, sizeof(tmpHeader));
    if (this->modelNames.empty()) {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (Reset): Albedo model was not set.");
        return;
    }
    int idx = 0;
    this->ALB.clear();
    this->gdlat.clear(); this->gdlon.clear();
    this->latDiff.clear(); this->lonDiff.clear();
    this->REQ_planets.clear();  this->RP_planets.clear();
    std::map<int64_t, SpicePlanetStateSimMsg>::iterator planetIt;
    for (planetIt = this->planetMsgData.begin(); planetIt != this->planetMsgData.end(); planetIt++)
    {
        messagingSystem->ReadMessage(planetIt->first, &tmpHeader, sizeof(SpicePlanetStateSimMsg),
            reinterpret_cast<uint8_t*>(&planetIt->second), this->moduleID);
        this->getPlanetRadius(planetIt->second.PlanetName);  //! - [m] get the planet radius
        this->evaluateAlbedoModel(idx);
        idx++;
    }
}

/*! This method reads the messages and saves the values to member attributes
 @return void
 */
void Albedo::readMessages() {
    SingleMessageHeader tmpHeader;
    memset(&tmpHeader, 0x0, sizeof(tmpHeader));
    auto messagingSystem = SystemMessaging::GetInstance();
    //! - Read in planet state message (required)
    std::map<int64_t, SpicePlanetStateSimMsg>::iterator planetIt;
    for (planetIt = this->planetMsgData.begin(); planetIt != this->planetMsgData.end(); planetIt++)
    {
        messagingSystem->ReadMessage(planetIt->first, &tmpHeader, sizeof(SpicePlanetStateSimMsg),
            reinterpret_cast<uint8_t*>(&planetIt->second), this->moduleID);
        if (m33Determinant(planetIt->second.J20002Pfix) == 0.0) {
            m33SetIdentity(planetIt->second.J20002Pfix);
        }
    }
    //! - Read in sun state message (required)
    messagingSystem->ReadMessage(this->sunPositionInMsgId, &tmpHeader, sizeof(SpicePlanetStateSimMsg),
        reinterpret_cast<uint8_t*>(&this->sunMsgData), this->moduleID);
    //! - Read in spacecraft state message (required)
    messagingSystem->ReadMessage(this->spacecraftStateInMsgId, &tmpHeader, sizeof(SCPlusStatesSimMsg),
        reinterpret_cast<uint8_t*>(&this->scStatesMsgData), this->moduleID);
}

/*! This method writes the output albedo messages
 @return void
 */
void Albedo::writeMessages(uint64_t CurrentSimNanos) {
    AlbedoSimMsg localMessage;
    memset(&localMessage, 0x0, sizeof(localMessage));
    std::vector<int64_t>::iterator it;
    std::vector<AlbedoSimMsg>::iterator albedoIt;
    int idx = 0;
    //! - Write albedo output messages for each instrument
    for (it = this->albOutMsgIds.begin(); it != this->albOutMsgIds.end(); it++) {
        localMessage.albedoAtInstrumentMax = this->albOutData.at(idx)[0];
        localMessage.albedoAtInstrument = this->albOutData.at(idx)[1];
        localMessage.AfluxAtInstrumentMax = this->albOutData.at(idx)[2];
        localMessage.AfluxAtInstrument = this->albOutData.at(idx)[3];
        SystemMessaging::GetInstance()->WriteMessage(*it,
            CurrentSimNanos,
            sizeof(AlbedoSimMsg),
            reinterpret_cast<uint8_t*>(&localMessage),
            moduleID);
        idx++;
    }
}

/*! Planet's equatorial radii and polar radii (if exists) in meters
 @return void
 */
void Albedo::getPlanetRadius(std::string planetSpiceName)
{
    if (planetSpiceName == "mercury") {
        this->REQ_planets.push_back(REQ_MERCURY * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);                  // [m]
    }
    else if (planetSpiceName == "venus") {
        this->REQ_planets.push_back(REQ_VENUS * 1000.0);   // [m]
        this->RP_planets.push_back(-1.0);                  // [m]
    }
    else if (planetSpiceName == "earth") {
        this->REQ_planets.push_back(REQ_EARTH * 1000.0); // [m]
        this->RP_planets.push_back(RP_EARTH * 1000.0);   // [m]
    }
    else if (planetSpiceName == "moon") {
        this->REQ_planets.push_back(REQ_MOON * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);               // [m]
    }
    else if (planetSpiceName == "mars" || planetSpiceName == "mars barycenter") {
        this->REQ_planets.push_back(REQ_MARS * 1000.0); // [m]
        this->RP_planets.push_back(RP_MARS * 1000.0);   // [m]
    }
    else if (planetSpiceName == "jupiter") {
        this->REQ_planets.push_back(REQ_JUPITER * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);                  // [m]
    }
    else if (planetSpiceName == "saturn") {
        this->REQ_planets.push_back(REQ_SATURN * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);                 // [m]
    }
    else if (planetSpiceName == "uranus") {
        this->REQ_planets.push_back(REQ_URANUS * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);                 // [m]
    }
    else if (planetSpiceName == "neptune") {
        this->REQ_planets.push_back(REQ_NEPTUNE * 1000.0); // [m]
        this->RP_planets.push_back(-1.0);                  // [m]
    }
    else {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (getPlanetRadius): The planet's radius cannot be obtained. The planet (%s) is not found.", planetSpiceName.c_str());
    }
    return;
}

/*! This method gets the average albedo value of the planet
 @return double
 */
double Albedo::getAlbedoAverage(std::string planetSpiceName)
{
    double ALB_avg;
    if (planetSpiceName == "mercury") {
        ALB_avg = 0.119; return ALB_avg;
    }
    else if (planetSpiceName == "venus") {
        ALB_avg = 0.75; return ALB_avg;
    }
    else if (planetSpiceName == "earth") {
        ALB_avg = 0.29; return ALB_avg;
    }
    else if (planetSpiceName == "moon") {
        ALB_avg = 0.123; return ALB_avg;
    }
    else if (planetSpiceName == "mars" || planetSpiceName == "mars barycenter") {
        ALB_avg = 0.16; return ALB_avg;
    }
    else if (planetSpiceName == "jupiter") {
        ALB_avg = 0.34; return ALB_avg;
    }
    else if (planetSpiceName == "saturn") {
        ALB_avg = 0.34; return ALB_avg;
    }
    else if (planetSpiceName == "uranus") {
        ALB_avg = 0.29; return ALB_avg;
    }
    else if (planetSpiceName == "neptune") {
        ALB_avg = 0.31; return ALB_avg;
    }
    else {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (getAlbedoAverage): The average albedo value is not defined for the specified planet (%s).", planetSpiceName.c_str()); return 0.;
    }
}

/*! This method evaluates the albedo model
 @return void
 */
void Albedo::evaluateAlbedoModel(int idx)
{
    this->readFile = false;
    auto modelName = modelNames.at(idx);
    auto fileName = fileNames.at(idx);
    auto dataPath = dataPaths.at(idx);
    auto numLat = numLats.at(idx);
    auto numLon = numLons.at(idx);
    //! - Obtain the parameters of the specified model    
    if (modelName == "ALBEDO_AVG") {
        //! - Albedo model based on an average value
        if (numLat < 0.0) { numLat = this->numLat; }
        if (numLon < 0.0) { numLon = this->numLon; }
    }
    else if (modelName == "ALBEDO_DATA") {
        //! - Albedo model based on data
        //! - Check that required module variables are set
        if (fileName == "") {
            bskLogger.bskLog(BSK_ERROR, "Albedo Module (evaluateAlbedoModel): Albedo fileName was not set.");
        }
        else {
            this->readFile = true;
        }
        fileName = dataPath + "/" + fileName;
    }
    else {
        bskLogger.bskLog(BSK_ERROR, "Albedo Module (evaluateAlbedoModel): Check the model name (%s).", modelName.c_str());
    }
    //! - Read the albedo coefficient file if the model requires
    if (this->readFile) {
        //! - Check that required module variables are set
        if (dataPath == "") {
            bskLogger.bskLog(BSK_ERROR, "Albedo Module (evaluateAlbedoModel): Albedo data path was not set.");
        }
        std::ifstream input(fileName);
        if (!input) {
            bskLogger.bskLog(BSK_ERROR, "Albedo Module (evaluateAlbedoModel): Albedo module is unable to load file %s", fileName.c_str());
        }
        //! - Read the albedo coefficients
        std::string line, field;
        std::vector< std::vector<std::string> > array;  // the 2D array
        std::vector<std::string> v;                     // array of values for one line only
        while (getline(input, line))
        {
            v.clear();
            std::stringstream ss(line);
            while (std::getline(ss, field, ','))  // break line into comma delimitted fields
            {
                v.push_back(field);
            }
            array.push_back(v);
        }
        numLat = array.size();
        numLon = array[0].size();
        //! - Compare if the numLat/numLon are not zero
        if (!numLat || !numLon) {
            bskLogger.bskLog(BSK_ERROR, "Albedo Module (evaluateAlbedoModel): There has been an error in reading albedo data from (%s).", fileName.c_str());
        }
        //! - Define the albedo array based on the number of grid points
        this->ALB[idx] = std::vector < std::vector < double > >(numLat, std::vector < double >(numLon, 0.0));

        //! - Albedo coefficients (2d array) from string array to double
        for (auto i = 0; i < numLat; ++i)
        {
            for (auto j = 0; j < numLon; ++j)
            {
                this->ALB[idx][i][j] = atof(array[i][j].c_str()); // Lat: -90 to +90 deg, Lon: -180 to +180 deg
            }
        }
    }
    else { this->ALB[idx] = std::vector < std::vector < double > >(1, std::vector < double >(1, 0.0)); }
    //! - Construct the latitude and longitude points
    //! - Geodetic latitude and longitude grid points in radian for albedo calculations
    double ilat = 0.0; double ilon = 0.0;
    double halfLat = numLat / 2; double halfLon = numLon / 2;
    this->gdlat[idx] = std::vector < double >(numLat, 0.0);
    this->gdlon[idx] = std::vector < double >(numLon, 0.0);
    this->latDiff[idx] = (180.0 / numLat) * M_PI / 180.0;
    this->lonDiff[idx] = (360.0 / numLon) * M_PI / 180.0;
    for (ilat = -halfLat; ilat < halfLat; ilat++) {
        this->gdlat[idx][ilat + halfLat] = (ilat + 0.5) * this->latDiff[idx];
    }
    for (ilon = -halfLon; ilon < halfLon; ilon++) {
        this->gdlon[idx][ilon + halfLon] = (ilon + 0.5) * this->lonDiff[idx];
    }
    this->numLons.at(idx) = numLon;
    this->numLats.at(idx) = numLat;
}

/*! This method calculates the albedo at instrument
 @return void
 */
void Albedo::computeAlbedo(int idx, int instIdx, SpicePlanetStateSimMsg planetMsg, bool albArray, double outData[]) {
    //! - Letters denoting the frames:
    //! - P: planet frame
    //! - B: spacecraft body frame
    //! - N: inertial frame
    //! - S: sun (helio) frame
    //! - I: instrument body frame    
    auto fov = this->fovs[instIdx];                                         //! - [rad] instrument's field of view half angle
    auto nHat_B = this->nHat_Bs[instIdx];                                   //! - [-] unit normal vector of the instrument (spacecraft body)
    auto r_IB_B = this->r_IB_Bs[instIdx];                                   //! - [m] instrument's misalignment vector wrt spacecraft's body frame
    this->r_PN_N = Eigen::Vector3d(planetMsg.PositionVector);               //! - [m] Planet's position vector (inertial)
    this->r_SN_N = Eigen::Vector3d(this->sunMsgData.PositionVector);        //! - [m] Sun's position vector (inertial)
    this->r_BN_N = Eigen::Vector3d(this->scStatesMsgData.r_BN_N);           //! - [m] Spacecraft's position vector (inertial)
    this->sigma_BN = Eigen::Vector3d(this->scStatesMsgData.sigma_BN);       //! - [m] Spacecraft's MRPs (inertial)
    Eigen::Matrix3d dcm_BN = this->sigma_BN.toRotationMatrix().transpose(); //! - inertial to sc body transformation
    this->nHat_N = dcm_BN.transpose() * nHat_B;                             //! - instrument's normal vector (inertial)
    //! - [m] sun's position wrt planet (inertial)
    auto r_SP_N = this->r_SN_N - this->r_PN_N;
    //! - Vectors related to spacecraft
    auto r_BP_N = this->r_BN_N - this->r_PN_N;        //! - [m] spacecraft's position wrt planet (inertial)
    //! - Vectors related to instrument
    auto r_IB_N = dcm_BN.transpose() * r_IB_B;        //! - [m] instrument's position vector wrt spacecraft (inertial)
    auto r_IP_N = r_IB_N + r_BP_N;                    //! - [m] instrument's position vector wrt planet (inertial)
    this->rHat_PI_N = -r_IP_N / r_IP_N.norm();        //! - [-] direction vector from instrument to planet (inertial)
    auto r_SI_N = r_SP_N - r_IP_N;                    //! - [m] sun's position wrt instrument (inertial)
    //! - Calculate the authalic radius, if the polar radius available
    double t[3], t_aut[3], e, RA_planet, shadowFactorAtdA;
    if (this->RP_planets.at(idx) > 0.0) {
        e = sqrt(1 - pow(this->RP_planets.at(idx), 2) / pow(this->REQ_planets.at(idx), 2));
        t[0] = pow(this->REQ_planets.at(idx), 2) * 0.5;
        t[1] = (1 - e * e) / 2.0 / e;
        t[2] = log((1 + e) / (1 - e));
        RA_planet = sqrt(t[0] * (1 + t[1] * t[2]));  //! - Autalic radius of the planet
        //! - Truncated series expansion for geodetic to authalic latitude
        t_aut[0] = pow(e, 2) / 3.0 + 31.0 * pow(e, 4) / 180.0 + 59.0 * pow(e, 6) / 560.0;
        t_aut[1] = 17.0 * pow(e, 4) / 360.0 + 61.0 * pow(e, 6) / 1260.0;
        t_aut[2] = 383.0 * pow(e, 6) / 45360.0;
    }
    else {
        RA_planet = this->REQ_planets.at(idx);
        v3SetZero(t_aut);
    }
    //! - Spacecraft's lla position (lat [rad], lon[rad], alti[m])
    auto LLA_B = PCI2LLA(r_BP_N, planetMsg.J20002Pfix, RA_planet);
    this->scLat = LLA_B[0] * 180 / M_PI;
    this->scLon = LLA_B[1] * 180 / M_PI;
    //! - [m] altitude of the instrument
    auto alti_I = r_IP_N.norm() - RA_planet;
    /* Return zeross if the rate of the instrument's altitude
    to the planet's radius exceeds the specified limit */
    if (this->altitudeRateLimit >=0 && alti_I / RA_planet > this->altitudeRateLimit) {
        bskLogger.bskLog(BSK_WARNING, "Albedo Module (computeAlbedo): The rate (altitude to planet's radii) limit is exceeded for the planet (%s) and albedo set to zero.", planetMsg.PlanetName);
        outData[0] = 0.0;
        outData[1] = 0.0;
        outData[2] = 0.0;
        outData[3] = 0.0;
    }
    else {
        //! - Variable definitions needed for the albedo calculations
        Eigen::Vector3d gdlla;
        Eigen::Vector3d r_dAP_N, r_SdA_N, r_IdA_N;
        Eigen::Vector3d rHat_dAP_N, sHat_SdA_N, rHat_IdA_N;
        int ilat = 0, ilon = 0, ImaxIdx = 0, IIdx = 0;  // index
        double lon1 = 0.0, lon2 = 0.0, lat1 = 0.0, lat2 = 0.0, tempmax = 0.0, tempfov = 0.0;
        double f1 = 0.0, f2 = 0.0, f3 = 0.0;
        double dArea = 0.0, alb_I = 0.0, alb_Imax = 0.0;
        std::vector< double > albLon1, albLat1;
        for (ilon = 0; ilon < this->numLons.at(idx); ilon++) {
            lon1 = this->gdlon[idx][ilon] + 0.5 * this->lonDiff[idx];
            lon2 = this->gdlon[idx][ilon] - 0.5 * this->lonDiff[idx];
            for (ilat = 0; ilat < this->numLats.at(idx); ilat++) {
                lat1 = this->gdlat[idx][ilat] + 0.5 * this->latDiff[idx];
                lat2 = this->gdlat[idx][ilat] - 0.5 * this->latDiff[idx];
                if (this->RP_planets.at(idx) > 0.0) {
                    //! - Truncated series expansion relating geodetic to authalic latitude
                    lat1 -= t_aut[0] * sin(2.0 * lat1) - t_aut[1] * sin(4.0 * lat1) + t_aut[2] * sin(6.0 * lat1);
                    lat2 -= t_aut[0] * sin(2.0 * lat2) - t_aut[1] * sin(4.0 * lat2) + t_aut[2] * sin(6.0 * lat2);
                }
                gdlla[0] = this->gdlat[idx][ilat]; gdlla[1] = this->gdlon[idx][ilon]; gdlla[2] = 0.0;
                //! - Vectors related to incremental area
                //! - [m] position of the incremental area (inertial)
                r_dAP_N = LLA2PCI(gdlla, planetMsg.J20002Pfix, RA_planet); //! - Assumes that the planet is a sphere.
                r_SdA_N = r_SP_N - r_dAP_N;            //! - [m] position vector from dA to Sun (inertial)
                r_IdA_N = r_IP_N - r_dAP_N;            //! - [m] position vector from dA to instrument (inertial)
                rHat_dAP_N = r_dAP_N / r_dAP_N.norm(); //! - [-] -assuming- dA normal vector (inertial) 
                sHat_SdA_N = r_SdA_N / r_SdA_N.norm(); //! - [-] sun direction vector from dA (inertial)
                rHat_IdA_N = r_IdA_N / r_IdA_N.norm(); //! - [-] dA to instrument direction vector (inertial)
                //! - Portions of the planet
                f1 = rHat_dAP_N.dot(sHat_SdA_N);       //! - for sunlit
                f2 = rHat_dAP_N.dot(rHat_IdA_N);       //! - for instrument's max fov
                f3 = this->nHat_N.dot(-rHat_IdA_N);    //! - for instrument's config fov
                //! - Detect the sunlit region of the planet seen by the instrument
                if (f1 > 0 && f2 > 0) {
                    //! - Sunlit portion of the planet seen by the instrument's position (max)
                    //! - Shadow factor at dA (optional)
                    shadowFactorAtdA = this->shadowFactorAtdA;
                    if (this->eclipseCase) { shadowFactorAtdA = computeEclipseAtdA(RA_planet, r_dAP_N, r_SP_N); }
                    //! - Area of the incremental area
                    dArea = (fabs(lon1 - lon2) * fabs(sin(lat1) - sin(lat2)) * pow(r_dAP_N.norm(), 2));
                    //! - Maximum albedo flux ratio at instrument's position [-]
                    tempmax = f1 * f2 * dArea / (pow(r_IdA_N.norm(), 2) * M_PI);
                    //if (this->ALB_data >= 0.0) { tempmax = this->ALB_data * tempmax; }
                    if (albArray == false) {
                        tempmax = this->ALB_avgs.at(idx) * tempmax;
                    }
                    else {
                        tempmax = this->ALB[idx][ilat][ilon] * tempmax;
                    }
                    alb_Imax = alb_Imax + tempmax * shadowFactorAtdA;
                    ImaxIdx++;
                    if (f3 >= cos(fov)) {
                        //! - Sunlit portion of the planet seen by the instrument (fov)
                        //! - Albedo flux ratio at instrument's position [-]
                        tempfov = f1 * f2 * f3 * dArea / (pow(r_IdA_N.norm(), 2) * M_PI);
                        if (albArray == false) {
                            tempfov = this->ALB_avgs.at(idx) * tempfov;
                        }
                        else {
                            tempfov = this->ALB[idx][ilat][ilon] * tempfov;
                        }
                        alb_I = alb_I + tempfov * shadowFactorAtdA;
                        albLon1.push_back(this->gdlon[idx][ilon] * 180 / M_PI);
                        albLat1.push_back(this->gdlat[idx][ilat] * 180 / M_PI);
                        IIdx++;
                    }
                }
            }
        }
        //! - The sunlit area seen by the instrument fov
        auto num1 = new double[IIdx];
        auto num2 = new double[IIdx];
        for (int i = 0; i < IIdx; i++) {
            num1[i] = albLon1[i];
            num2[i] = albLat1[i];
        }
        this->albLon = cArray2EigenMatrixXd(num1, 1, IIdx);
        this->albLat = cArray2EigenMatrixXd(num2, 1, IIdx);
        //! - Total albedo flux ratio [-]
        auto albedoAtInstrumentMax = alb_Imax;
        auto albedoAtInstrument = alb_I;
        //! - Compute the solar flux value at instrument [W/m^2]
        this->SfluxAtInstrument = SOLAR_FLUX_EARTH * pow(AU * 1000, 2) / pow((r_SI_N.norm()), 2);
        //! - Total albedo flux [W/m^2]
        auto AfluxAtInstrumentMax = (this->SfluxAtInstrument) * alb_Imax;
        auto AfluxAtInstrument = (this->SfluxAtInstrument) * alb_I;
        outData[0] = (albedoAtInstrumentMax);
        outData[1] = (albedoAtInstrument);
        outData[2] = (AfluxAtInstrumentMax);
        outData[3] = (AfluxAtInstrument);
    }
    return;
}

/*! This method computes eclipse at the incremental area if eclipseCase is defined true
 @return double
 */
double Albedo::computeEclipseAtdA(double Rplanet, Eigen::Vector3d r_dAP_N, Eigen::Vector3d r_SP_N)
{
    //! - Compute the shadow factor at incremental area
    //! - Note that the eclipse module computes the shadow factor at the spacecraft position
    auto r_SdA_N = r_SP_N - r_dAP_N; //! - [m] position vector from dA to Sun (inertial)
    auto s = r_dAP_N.norm();
    auto f_1 = asin((REQ_SUN * 1000 + Rplanet) / r_SP_N.norm());
    auto f_2 = asin((REQ_SUN * 1000 - Rplanet) / r_SP_N.norm());
    auto s_0 = (-r_dAP_N.dot(r_SP_N)) / r_SP_N.norm();
    auto c_1 = s_0 + Rplanet / sin(f_1);
    auto c_2 = s_0 - Rplanet / sin(f_2);
    auto l = sqrt(s * s - s_0 * s_0);
    auto l_1 = c_1 * tan(f_1);
    auto l_2 = c_2 * tan(f_2);
    double area = 0.0;
    double shadowFactorAtdA = 1.0; //! - Initialise the value for no eclipse
    double a = asin(REQ_SUN * 1000 / r_SdA_N.norm());   //! - Apparent radius of sun
    double b = asin(Rplanet / r_dAP_N.norm()); //! - Apparent radius of occulting body
    double c = acos((-r_dAP_N.dot(r_SdA_N)) / (r_dAP_N.norm() * r_SdA_N.norm()));
    if (fabs(l) < fabs(l_2) || fabs(l) < fabs(l_1))
    {
        // The order of the conditionals is important.
        // In particular (c < a + b) must check last to avoid testing
        // with implausible a, b and c values
        if (c < b - a) { // total eclipse, implying a < b
            shadowFactorAtdA = 0.0;
        }
        else if (c < a - b) { // partial maximum eclipse, implying a > b
            double areaSun = M_PI * a * a;
            double areaBody = M_PI * b * b;
            area = areaSun - areaBody;
            shadowFactorAtdA = 1 - area / (M_PI * a * a);
        }
        else if (c < a + b) { // partial eclipse
            double x = (c * c + a * a - b * b) / (2 * c);
            double y = sqrt(a * a - x * x);
            area = a * a * acos(x / a) + b * b * acos((c - x) / b) - c * y;
            shadowFactorAtdA = 1 - area / (M_PI * a * a);
        }
    }
    if (shadowFactorAtdA < 0.0) {
        shadowFactorAtdA = 0.0;
    }
    else if (shadowFactorAtdA > 1.0) {
        shadowFactorAtdA = 1.0;
    }
    return shadowFactorAtdA;
}
