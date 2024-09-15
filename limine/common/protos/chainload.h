#ifndef PROTOS__CHAINLOAD_H__
#define PROTOS__CHAINLOAD_H__

#include <stdnoreturn.h>

noreturn void chainload(char *config, char *cmdline);

#if defined (UEFI)
#include <fs/file.h>
noreturn void efi_chainload_file(char *config, char *cmdline, struct file_handle *image);
#endif

#if defined (BIOS)
#include <lib/part.h>
void bios_chainload_volume(struct volume *v);
#endif

#endif
