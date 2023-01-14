/*
  ==============================================================================

    VindorWrap.h
    Created: 5 Apr 2018 3:40:24pm
    Author:  Fernando Trias

  ==============================================================================
*/

#pragma once

#include "VindorControl.h"
#include "Settings.h"

class FiatinoControlSub : public VindorControl, public ChangeBroadcaster
{
private:    
    bool loadFromURL(const char *purl);
    String UseURL;
    bool unZip(String zip, File &of);
public:
    String LastError;
    String UseDesc;

    virtual void triggerDataChange(int state) override;

    bool updateFingeringData(var &data);
    bool updateFingeringSettingsData(var &data, SettingsData &settings);
    void updateFingering(SettingsData &settings);
    void updateFingeringCallback();

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX

    void updateFirmware();
    void updateFirmwareZDF();
    void updateFirmwareDFU();
    bool updateFirmwareURL(String purl);
    bool updateFirmwareFile(const char *pfile);

    void testUpdate(int showNotFound=0);
    bool hasUpdate();
    bool performUpdate();
    
#else
    
    bool hasUpdate() { return false; }
    
#endif
};

extern std::unique_ptr<FiatinoControlSub> deviceControl;
