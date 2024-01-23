#pragma once

#include "rendering/Framebuffer.h"
#include "devices/acpi/acpi.h"
#include "kernelStuff/kernelFiles/kernelFiles.h"
#include "kernelStuff/kernelFiles/ImageStruct.h"
#include "kernelStuff/kernelFiles/FileStruct.h"
#include "kernelStuff/kernelFiles/ZIPStruct.h"

#include <libm/rendering/S-Font.h>

struct SystemAssetStruct
{
    // Images
	kernelFiles::DefaultFile* bootImage;

    // Modules
    kernelFiles::DefaultFile* programs;
};

struct limineSmpInfo {
    uint32_t processor_id;
    uint32_t lapic_id;
    uint64_t reserved;
    uint64_t* goto_address;
    uint64_t extra_argument;
};

struct limineSmpResponse {
    uint64_t revision;
    uint32_t flags;
    uint32_t bsp_lapic_id;
    uint64_t cpu_count;
    limineSmpInfo** cpus;
};

void boot(void* bootInfo);


volatile void bootTest(Framebuffer fb, ACPI::RSDP2* rsdp, PSF1_FONT* psf1_font, SystemAssetStruct* assets, void* freeMemStart, void* extraMemStart, uint64_t freeMemSize, void* kernelStart, uint64_t kernelSize, void* kernelStartV, limineSmpResponse* smpData, void* memMap, uint64_t memEntryCount);

// void RenderLoop();
// void RecoverDed();