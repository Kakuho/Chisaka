//global jump_usermode
//extern test_user_function
//jump_usermode:
//;enable system call extensions that enables sysret and syscall
//	mov rcx, 0xc0000082
//	wrmsr
//	mov rcx, 0xc0000080
//	rdmsr
//	or eax, 1
//	wrmsr
//	mov rcx, 0xc0000081
//	rdmsr
//	mov edx, 0x00180008
//	wrmsr
//
//	mov ecx, test_user_function ; to be loaded into RIP
//	mov r11, 0x202 ; to be loaded into EFLAGS
//	sysretq ;use "o64 sysret" if you assemble with NASM
//

.extern test_user_function

.globl jump_usermode
jump_usermode:
  //enable system call extensions that enables sysret and syscall
	movq $0xc0000082, %rcx
	wrmsr
	movq $0xc0000080, %rcx
	rdmsr
	orl $1, %eax
	wrmsr
	movq $0xc0000081, %rcx
	rdmsr
	movl $0x00180008, %edx
	wrmsr

	movq $test_user_function, %rcx // to be loaded into RIP
	mov $0x202, %r11 // to be loaded into EFLAGS
	sysretq 
