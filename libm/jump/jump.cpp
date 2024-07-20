#include "jump.h"

void setjmp(jmp_buf *ctx)
{
	asm volatile(
			"movq %%rax,0(%%rdi)\n\t"
			"movq %%rbx,8(%%rdi)\n\t"
			"movq %%rcx,16(%%rdi)\n\t"
			"movq %%rdx,24(%%rdi)\n\t"
			"movq %%rdi,32(%%rdi)\n\t"
			"movq %%rsi,40(%%rdi)\n\t"
			"movq %%rbp,%%rbx\n\t"
			"add $16,%%rbx\n\t"
			"movq %%rbx,48(%%rdi)\n\t"  //save esp
			"movq 0(%%rbp),%%rbx\n\t"
			"movq %%rbx,56(%%rdi)\n\t"	//save ebp
			"movq 8(%%rbp),%%rbx\n\t"
			"movq %%rbx,64(%%rdi)\n\t"  //save eip
			:
			:);
}

void longjmp(jmp_buf *ctx)
{
	asm volatile(
			"movq 0(%%rdi),%%rax\n\t"	
			"movq 16(%%rdi),%%rcx\n\t"
			"movq 24(%%rdi),%%rdx\n\t"
			"movq 48(%%rdi),%%rsp\n\t"
			"movq 56(%%rdi),%%rbp\n\t"
			"movq 64(%%rdi),%%rbx\n\t"
			"pushq %%rbx\n\t"			//push eip
			"movq 8(%%rdi),%%rbx\n\t"
			"movq 32(%%rdi),%%rdi\n\t"
			"movq 40(%%rdi),%%rsi\n\t"
			"ret\n\t"					//pop eip
			:
			:);
}

