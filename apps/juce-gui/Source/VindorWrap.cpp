/*
  ==============================================================================

    VindorWrap.cpp
    Created: 5 Apr 2018 3:40:24pm
    Author:  Fernando Trias

  ==============================================================================
*/

#include "VindorWrap.h"
#include "VindorControl.h"

void FiatinoControlSub::triggerDataChange(int status)
{
    connectState = status;
    sendChangeMessage();
}

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX

bool FiatinoControlSub::updateFingeringData(var &data)
{
    if (updateFingeringSettingsData(data, settings.DevSettings)) {
        settings.setInt("KEYLAYOUT", 5); // set custom fingering
        sendSettingsFingering();
        sendSettings();
        return true;
    }
    return false;
}

bool FiatinoControlSub::updateFingeringSettingsData(var &data, SettingsData &settings)
{
    var bits_var = data.getProperty("bits", 12);
    int bits = int(bits_var);
    
    var mask_var = data.getProperty("mask", 0);
    int mask = int(mask_var);
    if (mask == 0) {
        // this is buggy!!!!!
        // mask = (1 << bits) - 1;

        // for 12 bits, 0x7FF
        mask = (1 << (bits-1)) - 1;
    }
    
    var useoctave_var = data.getProperty("useoctave", -1);
    int useoctave = int(useoctave_var);
    if (useoctave == -1) {
        if (bits > 12) useoctave = 0;
        else useoctave = 1;
    }
    
    settings.fingerlist_custom_mask = mask;
    settings.fingerlist_custom_bits = bits;
    settings.fingerlist_custom_useoctave = useoctave;
    
    var tree = data.getProperty("tree", "0");
    int sz = tree.size();
    
    if (sz < 2) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                    "Fingering data is missing");
        return false;
    }
    
    if (sz > settings.fingerlist_custom_max) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                    "Fingering data is too large to fit in Vindor");
        return false;
    }
    
    settings.fingerlist_custom_count = sz;
    
    for(int i=0; i<sz; i++) {
        backup_fingerlist_custom[i] = int(tree[i]);
    }
    return true;
}

void FiatinoControlSub::updateFingering(SettingsData &newsettings)
{
    if (! deviceControl->isValid()) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Not connected", "Please connect to the Vindor ES before updating the fingering");
        return;
    }
    
    FileChooser dialogBox ("Please choose a file to update Vindor...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.vnd");
    
    if (dialogBox.browseForFileToOpen())
    {
        File selectedFile (dialogBox.getResult());
        var data = JSON::parse(selectedFile);
        var dtype = data.getProperty("type", "none");
        
        if (dtype.toString() != "fingering") {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error",
                                        "File does not contain fingering data.");
            return;
        }
        
        updateFingeringSettingsData(data, newsettings);
    }
}

#else

bool FiatinoControlSub::updateFingeringData(var &data) {
    return false;
}

#endif

#if JUCE_MAC || JUCE_WINDOWS || JUCE_LINUX

void cpp_throw_exception(int n) {
    throw(n);
}

extern "C" {
    int firmware_dfu_upload(const char *filename, int vendor, int product);
    char dfu_error_buffer[10240];
    int dfu_error_length;
    int throw_exception(int num) {
        cpp_throw_exception(num);
        return 0;
    }
}

extern "C" {
    void firmware_dfu_init();
	int firmware_dfu_mode_probe();
    int libusb_init(void **ctx);
    void probe_devices(void *ctx);
    extern void *dfu_root;
}

bool FiatinoControlSub::unZip(String filename, File &ofile)
{
    String dfu_file = "firmware-R";
    dfu_file += deviceControl->settings.Revision;
    dfu_file += ".dfu";
    
    File inFile(filename);
    ZipFile zip(inFile);
    const ZipFile::ZipEntry *entry = zip.getEntry(dfu_file);
    if (! entry) {
        LastError = "This file is not compatible with your device";
        return false;
    }
    
    InputStream *istream = zip.createStreamForEntry(*entry);
    ofile = File::createTempFile(".dfu");
    auto ostream = ofile.createOutputStream();
    ostream->writeFromInputStream(*istream, 1024000);
    ostream->flush();
//    delete ostream;
    delete istream;
    
    return true;
}

void FiatinoControlSub::updateFirmware()
{
    if (! deviceControl->isValid()) {
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::WarningIcon, "Not connected", "Please connect to the Vindor ES before attempting to update the firmware. Do you want to continue anyway?", "Yes", "No");
        if (result == 1) {
            updateFirmwareDFU();
        }
        return;
    }
    
    updateFirmwareZDF();
}

void FiatinoControlSub::updateFirmwareDFU()
{
    FileChooser dialogBox ("Please choose a firmware file to update Vindor...",
                           File::getSpecialLocation (File::userHomeDirectory),
                           "*.dfu");
    
    if (dialogBox.browseForFileToOpen())
    {
        File selectedFile (dialogBox.getResult());
        const char *fn = selectedFile.getFullPathName().toRawUTF8();
        updateFirmwareFile(fn);
    }
}

#ifdef JUCE_MAC

#include <sys/utsname.h>

int getOSXVersion() {

/*
 17.x.x. macOS 10.13.x High Sierra
 16.x.x  macOS 10.12.x Sierra
 15.x.x  OS X  10.11.x El Capitan
 14.x.x  OS X  10.10.x Yosemite
 13.x.x  OS X  10.9.x  Mavericks
 12.x.x  OS X  10.8.x  Mountain Lion
 11.x.x  OS X  10.7.x  Lion
 10.x.x  OS X  10.6.x  Snow Leopard
 9.x.x  OS X  10.5.x  Leopard
 8.x.x  OS X  10.4.x  Tiger
 7.x.x  OS X  10.3.x  Panther
 6.x.x  OS X  10.2.x  Jaguar
 5.x    OS X  10.1.x  Puma
 */

    struct utsname name;
    uname(&name);
    return atoi(name.release);
}

#endif

void FiatinoControlSub::updateFirmwareZDF()
{
#ifdef JUCE_MAC
    if (getOSXVersion() < 16) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Upgrage MacOS",
                                    "Upgrading firmware only supported on MacOS Sierra 10.12 and above.");
        return;
    }
#endif
    
    /*
    File defdir = File::getSpecialLocation(File::SpecialLocationType::userHomeDirectory);
    WildcardFileFilter wildcardFilter ("*.zdf;*.dfu", String(), "Vindor firmware file");
    FileBrowserComponent browser (FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode,
                                  defdir,
                                  &wildcardFilter,
                                  nullptr);
    FileChooserDialogBox dialogBox ("Open firmware",
                                    "Please choose a firmware file to update Vindor...",
                                    browser,
                                    false,
                                    Colours::lightgrey);
    if (dialogBox.show())
    {
        File selectedFile = browser.getSelectedFile (0);
*/
    
    FileChooser dialogBox ("Please choose a firmware file to update Vindor...",
                               File::getSpecialLocation (File::userHomeDirectory),
                           "*.zdf;*.dfu");
    
    if (dialogBox.browseForFileToOpen())
    {
        File selectedFile (dialogBox.getResult());
            
        if (selectedFile.getFileName().endsWith(".dfu")) {
            const char *fn = selectedFile.getFullPathName().toRawUTF8();
            updateFirmwareFile(fn);
        }
        else {
            String dfu_file = "firmware-R";
            dfu_file += deviceControl->settings.Revision;
            dfu_file += ".dfu";
            
            ZipFile zip(selectedFile);
            const ZipFile::ZipEntry *entry = zip.getEntry(dfu_file);
            if (! entry) {
                AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Motherboard Not found",
                                            "This file is not compatible with your device.");
                return;
            }
            
            InputStream *istream = zip.createStreamForEntry(*entry);
            File ofile = File::createTempFile(".dfu");
            auto ostream = ofile.createOutputStream();
            ostream->writeFromInputStream(*istream, 1024000);
            ostream->flush();
//            delete ostream;
            delete istream;
            const char *fn = ofile.getFullPathName().toRawUTF8();
            
            updateFirmwareFile(fn);
            ofile.deleteFile();
        }
    }
}

bool FiatinoControlSub::updateFirmwareURL(String purl) {
    URL url(purl);
    URL::InputStreamOptions options(URL::ParameterHandling::inAddress);
    auto istream = url.createInputStream(options);
    
    if (istream == NULL) {
        LastError = "Failed to download " + purl;
        return false;
    }
    
    File ofile;
    
    if (String(purl).endsWith(".zdf")) {
        File zfile = File::createTempFile(".zdf");
        auto ostream = zfile.createOutputStream();
        ostream->writeFromInputStream(*istream, 1024000);
        ostream->flush();
    
        if (! unZip(zfile.getFullPathName(), ofile)) {
            return false;
        }
    }
    else{
        ofile = File::createTempFile(".dfu");
        auto ostream = ofile.createOutputStream();
        ostream->writeFromInputStream(*istream, 1024000);
        ostream->flush();
    }
        
    const char *fn = ofile.getFullPathName().toRawUTF8();
    bool ret = updateFirmwareFile(fn);
    ofile.deleteFile();
    return ret;
}

class WaitForFirmware : public ThreadWithProgressWindow
{
public:
    int result;
    int dfu;
    std::string fn;
    WaitForFirmware(const char *filename)    : ThreadWithProgressWindow ("Updating...", true, false)
    {
        fn = filename;
    }
    void run()
    {
        setProgress(0.1);
        sleep(100);

        dfu = 0;
        for(int i=0; i<10; i++) {
            Thread::sleep(500); // wait for DFU to come up
            dfu = firmware_dfu_mode_probe();
            if (dfu) break;
        }
        
        setProgress(0.2);
        result = 0;
        try {
            dfu_error_length = 0;
            dfu_error_buffer[0] = 0;
            result = firmware_dfu_upload(fn.c_str(), 0x15a2, 0x0500);
        }
        catch(int p) {
            if (p != 1) result = p;
        }
        setProgress(1.0);
    }
};

bool FiatinoControlSub::updateFirmwareFile(const char *fn) {
    deviceControl->enterBootloader();
    deviceControl->disconnect();

    WaitForFirmware m(fn);
    m.runThread();
    
    if (! m.dfu) {
        AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Update Failed",
                                    "Vindor ES did not enter DFU Bootloader mode. Please try again or contact customer support.");
        return false;
    }
    
    if (m.result == 1) {
#if 1
        int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::InfoIcon, "Update",
                                                  "Update successful. Restoring factory settings is recommended. Would you like to restore to factory settings?", "Yes", "No");
        if (result == 1) {
            sendRestoreFactory();
            Thread::sleep(1000);
        }
#endif
        Thread::sleep(1000);
        return true;
    }
    else {
        LastError = dfu_error_buffer;
        return false;
    }
}

bool FiatinoControlSub::hasUpdate()
{
    URL url("https://vindormusic.com/emfiles");
    URL::InputStreamOptions options(URL::ParameterHandling::inAddress);
    auto in = url.createInputStream(options);
    
    if (in == NULL) return false;

//    int ret = in->getStatusCode();

    String ver = String("R") + String(settings.Revision);
    UseURL = "";
    
    int isactive = 0;
    while(! in->isExhausted()) {
        auto line = in->readNextLine();
        line = line.trim();
        DBG(line);
        if (line.endsWith("<pre>")) {
            isactive = 1;
            continue;
        }
        if (line.startsWith("</pre>")) {
            break;
        }
        
        if (! isactive) continue;
        
        StringArray tokens;
        tokens.addTokens (line, ",", "\"");
        
        if (tokens[0] == "F") {
            if (tokens[1] == ver || tokens[1] == "*") {
                int fv = CharacterFunctions::getIntValue<int>(tokens[2].getCharPointer());
                if (settings.Firmware < fv) {
                    UseURL = tokens[4];
                    UseDesc = tokens[3];
                    break;
                }
            }
        }
    }
    
    if (UseURL == "") return false;
    return true;
}

bool FiatinoControlSub::performUpdate()
{
    if (UseURL == "") return false;
    return updateFirmwareURL(UseURL);
}

void FiatinoControlSub::testUpdate(int showNotFound)
{
    if (hasUpdate()) {
        int r = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Update " + UseDesc,
                                                "An update to the Vindor ES firmware is available. Would you like to apply it now?", "Yes", "No");
        if (r == 1) {
            bool res = performUpdate();
            if (! res) {
                AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Update Error",
                                            deviceControl->LastError);
            }
        }
    }
    else {
        if (showNotFound) {
            AlertWindow::showMessageBox(AlertWindow::AlertIconType::InfoIcon, "No Update",
                                         "An update was not found");
        }
    }
}

#else

//void FiatinoControlSub::testUpdate(int showNotFound) { }
//void FiatinoControlSub::updateFirmware() { }

#endif

std::unique_ptr<FiatinoControlSub> deviceControl;
