// Filename: rvct_stdio.cpp
//
// Project: Horizon

#include <nn/types.h>
#include <nn/assert.h>
#include <nn/os/CTR/os_ThreadLocalRegion.h>

#include <rt_sys.h>
#include <rt_misc.h>
#include <time.h>
#include <stdio.h>
#include <typeinfo>
#include <cstring>


typedef int FILEHANDLE;

#pragma import(__use_no_semihosting)
#pragma import(__use_no_semihosting_swi)

extern "C"{
    const char __stdin_name[]  = "";
    const char __stdout_name[] = "";
    const char __stderr_name[] = "";

    FILEHANDLE _sys_open(const char * name, int openmode) { NN_UNUSED_VAR(name); NN_UNUSED_VAR(openmode); return -1; }
    int _sys_close(FILEHANDLE fh) { NN_UNUSED_VAR(fh); return -1; }
    int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode) { NN_UNUSED_VAR(fh); NN_UNUSED_VAR(buf); NN_UNUSED_VAR(len); NN_UNUSED_VAR(mode); return -1; }
    int _sys_read(FILEHANDLE fh, unsigned char * buf, unsigned len, int mode) { NN_UNUSED_VAR(fh); NN_UNUSED_VAR(buf); NN_UNUSED_VAR(len); NN_UNUSED_VAR(mode); return len; }
    void _ttywrch(int ch) { NN_UNUSED_VAR(ch); }
    int _sys_istty(FILEHANDLE fh) { NN_UNUSED_VAR(fh); return -1; }
    int _sys_seek(FILEHANDLE fh, long pos) { NN_UNUSED_VAR(fh); NN_UNUSED_VAR(pos); return -1; }
    int _sys_ensure(FILEHANDLE fh) { NN_UNUSED_VAR(fh); return -1; }
    long _sys_flen(FILEHANDLE fh) { NN_UNUSED_VAR(fh); return -1; }
    int _sys_tmpnam(char *name, int fileno, unsigned maxlength) { NN_UNUSED_VAR(name); NN_UNUSED_VAR(fileno); NN_UNUSED_VAR(maxlength); return -1; }
    void _sys_exit(int return_code) { NN_UNUSED_VAR(return_code); }
    char* _sys_command_string (char* cmd, int len) { NN_UNUSED_VAR(cmd); NN_UNUSED_VAR(len); return NULL; }

    void __rt_raise(int sig, int type) { NN_UNUSED_VAR(sig); NN_UNUSED_VAR(type); }

    void abort() { NN_PANIC_("abort() called\n"); }

    void __aeabi_atexit() {}
    void __cxa_finalize() {}
    void __rt_SIGTMEM() {}
    void __rt_SIGABRT() {}
    void __rt_div0() {}

    void** __rt_eh_globals_addr(){ return &nn::os::CTR::GetThreadLocalRegion()->ehGlobalsAddr; }
}

namespace std{
    bool type_info::operator==(const type_info& rhs) const{
        return std::strcmp(this->name(), rhs.name()) == 0;
    }
    bool type_info::operator!=(const type_info& rhs) const{
        return std::strcmp(this->name(), rhs.name()) != 0;
    }
    bool type_info::before(const type_info& rhs) const{
        return std::strcmp(this->name(), rhs.name()) < 0;
    }
}