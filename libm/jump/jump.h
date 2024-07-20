#pragma once

typedef struct task_ctx
{
	unsigned long eax;
	unsigned long ebx;
	unsigned long ecx;
	unsigned long edx;
	unsigned long edi;
	unsigned long esi;
	unsigned long esp;
	unsigned long ebp;
	unsigned long eip;
}jmp_buf;

void setjmp(jmp_buf *ctx);
void longjmp(jmp_buf *ctx);