#include <sys/isrs.h>
#include <sys/syscall.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

void syscall_install()
{
	//Stub
}

void syscall_handler(interrupt_frame_t int_frame, popa_t *r)
{
	//int_frame=NULL;
	//popa_t re=r;
}