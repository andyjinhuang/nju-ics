#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include "FLOAT.h"

extern char _vfprintf_internal;
extern char _fpmaxtostr;
extern char _ppfs_setargs;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* TODO: Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */

	char buf[80];
	long long new1 = ((long long)f);
	int len=0;
	if(new1<0){
		new1 = -new1;
		len = sprintf(buf,"-"); 
		
	}
	len = len+sprintf(len+buf,"%u",f>>16);
	len = len+sprintf(buf+len,".");
	len = len+sprintf(buf+len,"%.6llu",(((long long)(new1&(0xffff)))*1000000)>>16);
	/*int j;
	for(j=len+1;j>len-6;j--){
		buf[j]=buf[j-1];
	}
	buf[len-6]='.';
	len=len+1;*/
	return __stdio_fwrite(buf, len, stream);
	
	/*char buf[80];
	int len;
	if (f < 0){
					        len = sprintf(buf, "-");
						        f = -f;
							    }
		        else
				        len = 0;

			    len += sprintf(buf + len, "%u", f >> 16);
			        len += sprintf(buf + len, ".");

				    uint64_t frac = (uint64_t) (f & 0x0000ffff);
				        frac *= 1000000;
					if(frac>0)
						frac >>= 16;
					else
						frac=(frac+65535)>>16;

					        len += sprintf(buf+len, "%.6llu", frac);

							return __stdio_fwrite(buf, len, stream);
*/}

static void modify_vfprintf() {
	/* TODO: Implement this function to hijack the formating of "%f"
	 * argument during the execution of `_vfprintf_internal'. Below
	 * is the code section in _vfprintf_internal() relative to the
	 * hijack.
	 */

	int p;
	p=(int)(&_vfprintf_internal)+0x306+1;
//	mprotect((void*)((int)(p-100)&(0xfffff000)),4096*2,PROT_READ | PROT_WRITE | PROT_EXEC);
	*(uint32_t *)p=*(uint32_t *)p+(int)((int)&format_FLOAT-(int)&_fpmaxtostr);

	p=p-1; ///the start address of call
	*(uint8_t *)(p-0xa)=0x57;  //the start of fstpt -> push %edi
	*(uint8_t *)(p-0xb)=0x08;  //the change of sub
	*(uint16_t *)(p-0x9)=0x9090; //
	*(uint16_t *)(p-0x15)=0x3a8b;
	*(uint32_t *)(p-0x13)=0x90909090;
	*(uint8_t *)(p-0xf)=0x90;
	*(uint16_t *)(p-0x22)=0x9090;
	*(uint16_t *)(p-0x1e)=0x9090;

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

	/* You should modify the run-time binary to let the code above
	 * call `format_FLOAT' defined in this source file, instead of
	 * `_fpmaxtostr'. When this function returns, the action of the
	 * code above should do the following:
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

}

static void modify_ppfs_setargs() {
	/* TODO: Implement this function to modify the action of preparing
	 * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
	 * Below is the code section in _vfprintf_internal() relative to
	 * the modification.
	 */

	int p;
	p=(int)&_ppfs_setargs+0x71;
	*(uint16_t *)p = 0x30eb;

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

	/* You should modify the run-time binary to let the `PA_DOUBLE'
	 * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
	 * branch. Comparing to the original `PA_DOUBLE' branch, the
	 * target branch will also prepare a 64-bit argument, without
	 * introducing floating point instructions. When this function
	 * returns, the action of the code above should do the following:
	 */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif

}

void init_FLOAT_vfprintf() {
	modify_vfprintf();
	modify_ppfs_setargs();
}
