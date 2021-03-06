
#include <mips/regdef.h>
#include <cp0_regdef.h>
#include "asm_regs.h"

	.text
	.globl syscall_none
	.globl syscall_one
	.globl syscall_two
	.globl syscall_three

# my_system_call:
#   A user mode interface to the kernel mode function
#   'kmy_system_call'.
#

syscall_none:
	move v0, a0
	syscall
	nop
	jr ra			# Back in user mode, return to caller

syscall_one:
	move v0, a1
	syscall
	nop
	jr ra			# Back in user mode, return to caller

syscall_two:
	move v0, a2
	syscall
	nop
	jr ra			# Back in user mode, return to caller

syscall_three:
	move v0, a3
	syscall
	nop
	jr ra			# Back in user mode, return to caller
