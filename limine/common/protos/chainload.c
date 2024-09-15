#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <protos/chainload.h>
#include <lib/part.h>
#include <lib/config.h>
#include <lib/misc.h>
#include <drivers/disk.h>
#include <lib/term.h>
#include <lib/fb.h>
#include <lib/uri.h>
#include <lib/print.h>
#include <lib/libc.h>
#include <sys/idt.h>
#include <drivers/vga_textmode.h>
#include <mm/pmm.h>
#if defined (UEFI)
#  include <efi.h>
#endif

#if defined (BIOS)

__attribute__((noinline, section(".realmode")))
noreturn static void spinup(uint8_t drive, void *buf) {
    struct idtr real_mode_idt;
    real_mode_idt.limit = 0x3ff;
    real_mode_idt.ptr   = 0;

    asm volatile (
        "cli\n\t"
        "cld\n\t"

        // Safe stack location
        "mov $0x7c00, %%esp\n\t"

        // move buffer to final location
        "mov $0x7c00, %%edi\n\t"
        "mov $512, %%ecx\n\t"
        "rep movsb\n\t"

        "lidt (%%eax)\n\t"

        "pushl $0x08\n\t"
        "pushl $1f\n\t"
        "lret\n\t"
        "1: .code16\n\t"
        "movw $0x10, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"
        "movl %%cr0, %%eax\n\t"
        "andb $0xfe, %%al\n\t"
        "movl %%eax, %%cr0\n\t"
        "movl $1f, %%eax\n\t"
        "pushw $0\n\t"
        "pushw %%ax\n\t"
        "lret\n\t"
        "1:\n\t"
        "xorw %%ax, %%ax\n\t"
        "movw %%ax, %%ds\n\t"
        "movw %%ax, %%es\n\t"
        "movw %%ax, %%fs\n\t"
        "movw %%ax, %%gs\n\t"
        "movw %%ax, %%ss\n\t"

        "sti\n\t"

        "pushw $0\n\t"
        "pushw $0x7c00\n\t"
        "lret\n\t"

        ".code32\n\t"
        :
        : "a" (&real_mode_idt), "d" (drive), "S"(buf)
        : "memory"
    );

    __builtin_unreachable();
}

noreturn void chainload(char *config, char *cmdline) {
    (void)cmdline;

    uint64_t val;

    int part; {
        char *part_config = config_get_value(config, 0, "PARTITION");
        if (part_config == NULL) {
            part = 0;
        } else {
            val = strtoui(part_config, NULL, 10);
            if (val > 256) {
                panic(true, "chainload: BIOS partition number outside range 0-256");
            }
            part = val;
        }
    }
    int drive; {
        char *drive_config = config_get_value(config, 0, "DRIVE");
        if (drive_config == NULL) {
            drive = boot_volume->index;
        } else {
            val = strtoui(drive_config, NULL, 10);
            if (val < 1 || val > 256) {
                panic(true, "chainload: BIOS drive number outside range 1-256");
            }
            drive = val;
        }
    }

    struct volume *p = volume_get_by_coord(false, drive, part);

    char *gpt_guid_s = config_get_value(config, 0, "GPT_GUID");
    if (gpt_guid_s == NULL) {
        gpt_guid_s = config_get_value(config, 0, "GPT_UUID");
    }
    if (gpt_guid_s != NULL) {
        struct guid guid;
        if (!string_to_guid_be(&guid, gpt_guid_s)) {
            panic(true, "chainload: Malformed GUID");
        }

        p = volume_get_by_guid(&guid);
        if (p == NULL) {
            if (!string_to_guid_mixed(&guid, gpt_guid_s)) {
                panic(true, "chainload: Malformed GUID");
            }

            p = volume_get_by_guid(&guid);
        }

        if (p == NULL) {
            panic(true, "chainload: No matching GPT drive for GPT_GUID found");
        }

        if (p->partition != 0) {
            panic(true, "chainload: GPT_GUID is that of a partition, not a drive");
        }

        p = volume_get_by_coord(false, p->index, part);

        if (p == NULL) {
            panic(true, "chainload: Partition specified is not valid");
        }

        goto load;
    }

    char *mbr_id_s = config_get_value(config, 0, "MBR_ID");
    if (mbr_id_s != NULL) {
        uint32_t mbr_id = strtoui(mbr_id_s, NULL, 16);

        for (size_t i = 0; i < volume_index_i; i++) {
            p = volume_index[i];

            if (!is_valid_mbr(p)) {
                continue;
            }

            uint32_t mbr_id_1;
            volume_read(p, &mbr_id_1, 0x1b8, sizeof(uint32_t));

            if (mbr_id_1 == mbr_id) {
                p = volume_get_by_coord(false, p->index, part);

                if (p == NULL) {
                    panic(true, "chainload: Partition specified is not valid");
                }

                goto load;
            }
        }

        panic(true, "chainload: No matching MBR ID found");
    }

load:
    bios_chainload_volume(p);

    panic(true, "chainload: Volume is not bootable");
}

void bios_chainload_volume(struct volume *p) {
    vga_textmode_init(false);

    void *buf = ext_mem_alloc(512);

    volume_read(p, buf, 0, 512);

    uint16_t *boot_sig = (uint16_t *)(buf + 0x1fe);

    if (*boot_sig != 0xaa55) {
        return;
    }

    spinup(p->drive, buf);
}

#elif defined (UEFI)

noreturn void chainload(char *config, char *cmdline) {
    char *image_path = config_get_value(config, 0, "IMAGE_PATH");
    if (image_path == NULL)
        panic(true, "chainload: IMAGE_PATH not specified");

    struct file_handle *image;
    if ((image = uri_open(image_path)) == NULL)
        panic(true, "chainload: Failed to open image with path `%s`. Is the path correct?", image_path);

    efi_chainload_file(config, cmdline, image);
}

noreturn void efi_chainload_file(char *config, char *cmdline, struct file_handle *image) {
    EFI_STATUS status;

    EFI_HANDLE efi_part_handle = image->efi_part_handle;

    void *ptr = freadall(image, MEMMAP_RESERVED);
    size_t image_size = image->size;

    memmap_alloc_range_in(untouched_memmap, &untouched_memmap_entries,
                          (uintptr_t)ptr, ALIGN_UP(image_size, 4096),
                          MEMMAP_RESERVED, MEMMAP_USABLE, true, false, true);

    fclose(image);

    term_notready();

    size_t req_width = 0, req_height = 0, req_bpp = 0;

    char *resolution = config_get_value(config, 0, "RESOLUTION");
    if (resolution != NULL)
        parse_resolution(&req_width, &req_height, &req_bpp, resolution);

    struct fb_info *fbinfo;
    size_t fb_count;
    fb_init(&fbinfo, &fb_count, req_width, req_height, req_bpp);

    size_t cmdline_len = strlen(cmdline);
    CHAR16 *new_cmdline;
    status = gBS->AllocatePool(EfiLoaderData, (cmdline_len + 1) * sizeof(CHAR16), (void **)&new_cmdline);
    if (status) {
        panic(true, "chainload: Allocation failure");
    }
    for (size_t i = 0; i < cmdline_len + 1; i++) {
        new_cmdline[i] = cmdline[i];
    }

    pmm_release_uefi_mem();

    MEMMAP_DEVICE_PATH memdev_path[2];

    memdev_path[0].Header.Type      = HARDWARE_DEVICE_PATH;
    memdev_path[0].Header.SubType   = HW_MEMMAP_DP;
    memdev_path[0].Header.Length[0] = sizeof(MEMMAP_DEVICE_PATH);
    memdev_path[0].Header.Length[1] = sizeof(MEMMAP_DEVICE_PATH) >> 8;

    memdev_path[0].MemoryType       = EfiLoaderCode;
    memdev_path[0].StartingAddress  = (uintptr_t)ptr;
    memdev_path[0].EndingAddress    = (uintptr_t)ptr + image_size;

    memdev_path[1].Header.Type      = END_DEVICE_PATH_TYPE;
    memdev_path[1].Header.SubType   = END_ENTIRE_DEVICE_PATH_SUBTYPE;
    memdev_path[1].Header.Length[0] = sizeof(EFI_DEVICE_PATH);
    memdev_path[1].Header.Length[1] = sizeof(EFI_DEVICE_PATH) >> 8;

    EFI_HANDLE new_handle = 0;

    status = gBS->LoadImage(0, efi_image_handle,
                            (EFI_DEVICE_PATH *)memdev_path,
                            ptr, image_size, &new_handle);
    if (status) {
        panic(false, "chainload: LoadImage failure (%x)", status);
    }

    // Apparently we need to make sure that the DeviceHandle field is the same
    // as us (the loader) for some EFI images to properly work (Windows for instance)
    EFI_GUID loaded_img_prot_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    EFI_LOADED_IMAGE_PROTOCOL *new_handle_loaded_image = NULL;
    status = gBS->HandleProtocol(new_handle, &loaded_img_prot_guid,
                                 (void **)&new_handle_loaded_image);
    if (status) {
        panic(false, "chainload: HandleProtocol failure (%x)", status);
    }

    if (efi_part_handle != 0) {
        new_handle_loaded_image->DeviceHandle = efi_part_handle;
    }

    new_handle_loaded_image->LoadOptionsSize = cmdline_len * sizeof(CHAR16);
    new_handle_loaded_image->LoadOptions = new_cmdline;

    UINTN exit_data_size = 0;
    CHAR16 *exit_data = NULL;
    EFI_STATUS exit_status = gBS->StartImage(new_handle, &exit_data_size, &exit_data);

    status = gBS->Exit(efi_image_handle, exit_status, exit_data_size, exit_data);
    if (status) {
        panic(false, "chainload: Exit failure (%x)", status);
    }

    __builtin_unreachable();
}

#endif
