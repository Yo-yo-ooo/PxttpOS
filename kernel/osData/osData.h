#pragma once
#include "MStack/MStackS.h"

#include "../kernelStuff/stuff/stackmacro.h"
// #include "../kernelStuff/stuff/kernelUtil.h"


// #include "../WindowStuff/WindowManager/windowManager.h"


// #include "../cStdLib/list/list_all.h"

// #include "../devices/ac97/ac97.h"

#include <libm/audio/internal/audio.h>
#include "../devices/ac97/ac97.h"

// #include "../serialManager/serialManager.h"

// #include "../display/generic/genericDisplay.h"

// #include "../cStdLib/queue/queue_all.h"

#include "../diskStuff/Disk_Interfaces/generic/genericDiskInterface.h"
#include <libm/list/list_genDiskInterface.h>

#include "../fsStuff/fatfs/ff.h"

struct OSData
{
    bool exit;
    bool booting;
    bool verboseBoot;
    bool inBootProcess;

    bool NO_INTERRUPTS = false;

    bool drawBackground;
    bool enableStackTrace;
    int64_t crashCount = 0;
    int maxNonFatalCrashCount = 0;
    bool crashed = false;
    bool tempCrash = false;

    List<DiskInterface::GenericDiskInterface*> diskInterfaces;

    List<Audio::AudioInputDevice*> audioInputDevices;
    List<Audio::AudioOutputDevice*> audioOutputDevices;

    Audio::AudioInputDevice* defaultAudioInputDevice = NULL;
    Audio::AudioOutputDevice* defaultAudioOutputDevice = NULL;

    int mouseSensitivity = 100;


    AC97::AC97Driver* ac97Driver = NULL;
    Audio::AudioOutputDevice* pcSpeakerDev;

    List<FATFS*>FatFs;	/* Pointer to the filesystem objects (logical drives) */

};



extern OSData osData;