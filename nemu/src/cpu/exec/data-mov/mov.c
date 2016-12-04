#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "mov-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "mov-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(mov_i2r)
make_helper_v(mov_i2rm)
make_helper_v(mov_r2rm)
make_helper_v(mov_rm2r)
make_helper_v(mov_a2moffs)
make_helper_v(mov_moffs2a)

make_helper(mov_cr2r){
	uint8_t reg = instr_fetch(eip+1,1);
	uint8_t dest = reg & 0x7;
	uint8_t src = (reg>>3)& (0x7);
	if(src!=3){
		printf("mov_cr2r: wrong crx!\n");
	}else{
		reg_l(dest)=cpu.cr3.val;
	}
	return 2;
}

make_helper(mov_r2cr){
	uint8_t reg=instr_fetch(eip+1,1);
	uint8_t src=reg&0x7;
	uint8_t dest=(reg>>3)&(0x7);
	if(dest==3){
		cpu.cr3.val=reg_l(src);
	}else if(dest==0){
		cpu.cr0.val=reg_l(src);
	}else{
		printf("mov_r2cr: wrong crx!\n");
	}
	print_asm("mov_r2cr");
	return 2;
}

make_helper(mov_rm2sreg){
	int len=decode_rm_w(eip+1);
	uint8_t sreg=instr_fetch(eip+1,1);
	sreg=(sreg>>3)&0x7;
	seg(sreg).selector.val=op_src->val;
	seg(sreg).dirty=false;
	print_asm("mov_r2cr");
	return len+1;
}
