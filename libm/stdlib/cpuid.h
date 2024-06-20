#pragma once

struct cpuinfo_x86 {
	unsigned char			x86;			
	unsigned char			x86_vendor;		
	unsigned char			x86_model;
	unsigned char			x86_stepping;
	int			     		cpuid_level;
	char					x86_vendor_id[16];
	char					x86_model_id[64];
	int						x86_cache_alignment;	
	unsigned short			x86_clflush_size;
	unsigned char			x86_virt_bits;
	unsigned char			x86_phys_bits;
	unsigned char			x86_cache_bits;
};

void cpuid(unsigned int op, unsigned int *eax, unsigned int *ebx,
			 				unsigned int *ecx, unsigned int *edx);
void get_cpu_address_sizes(struct cpuinfo_x86 *c);
void cpu_detect(struct cpuinfo_x86 *c);
void get_model_name(struct cpuinfo_x86 *c);