/*=========================================================================*//**
@file    syscall.c

@author  Daniel Zorychta

@brief   System call handling - kernel space

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "config.h"
#include "fs/fsctrl.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "kernel/syscall.h"
#include "kernel/kwrapper.h"
#include "kernel/process.h"
#include "kernel/printk.h"
#include "kernel/kpanic.h"
#include "kernel/errno.h"
#include "kernel/time.h"
#include "lib/cast.h"
#include "lib/unarg.h"
#include "net/netm.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define SYSCALL_QUEUE_LENGTH    8

#define GETARG(type, var)       type var = va_arg(rq->args, type)
#define GETRETURN(type, var)    type var = rq->retptr
#define GETTASKHDL()            rq->task
#define GETPROCESS()            _process_get_container_by_task(rq->task_client, NULL)
#define GETTHREAD(_tid)         _process_thread_get_container(GETPROCESS(), _tid)
#define SETRETURN(type, var)    if (rq->retptr) {*((type*)rq->retptr) = (var);}
#define SETERRNO(var)           rq->err = var
#define GETERRNO()              rq->err
#define UNUSED_RQ()             UNUSED_ARG1(rq)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void      *retptr;
        task_t    *task_client;
        _thread_t *syscall_thread;
        syscall_t  syscall_no;
        va_list    args;
        int        err;
} syscallrq_t;

typedef void (*syscallfunc_t)(syscallrq_t*);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static task_t *destroy_top_process(void);
static void syscall_do(void *rq);
static void syscall_mount(syscallrq_t *rq);
static void syscall_umount(syscallrq_t *rq);
#if __OS_ENABLE_STATFS__ == _YES_
static void syscall_getmntentry(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKNOD__ == _YES_
static void syscall_mknod(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKDIR__ == _YES_
static void syscall_mkdir(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKFIFO__ == _YES_
static void syscall_mkfifo(syscallrq_t *rq);
#endif
#if __OS_ENABLE_DIRBROWSE__ == _YES_
static void syscall_opendir(syscallrq_t *rq);
static void syscall_closedir(syscallrq_t *rq);
static void syscall_readdir(syscallrq_t *rq);
#endif
#if __OS_ENABLE_REMOVE__ == _YES_
static void syscall_remove(syscallrq_t *rq);
#endif
#if __OS_ENABLE_RENAME__ == _YES_
static void syscall_rename(syscallrq_t *rq);
#endif
#if __OS_ENABLE_CHMOD__ == _YES_
static void syscall_chmod(syscallrq_t *rq);
#endif
#if __OS_ENABLE_CHOWN__ == _YES_
static void syscall_chown(syscallrq_t *rq);
#endif
#if __OS_ENABLE_STATFS__ == _YES_
static void syscall_statfs(syscallrq_t *rq);
#endif
#if __OS_ENABLE_FSTAT__ == _YES_
static void syscall_stat(syscallrq_t *rq);
static void syscall_fstat(syscallrq_t *rq);
#endif
static void syscall_fopen(syscallrq_t *rq);
static void syscall_fclose(syscallrq_t *rq);
static void syscall_fwrite(syscallrq_t *rq);
static void syscall_fread(syscallrq_t *rq);
static void syscall_fseek(syscallrq_t *rq);
static void syscall_ioctl(syscallrq_t *rq);
static void syscall_fflush(syscallrq_t *rq);
static void syscall_sync(syscallrq_t *rq);
#if __OS_ENABLE_TIMEMAN__ == _YES_
static void syscall_gettime(syscallrq_t *rq);
static void syscall_settime(syscallrq_t *rq);
#endif
static void syscall_driverinit(syscallrq_t *rq);
static void syscall_driverrelease(syscallrq_t *rq);
static void syscall_malloc(syscallrq_t *rq);
static void syscall_zalloc(syscallrq_t *rq);
static void syscall_free(syscallrq_t *rq);
static void syscall_syslogenable(syscallrq_t *rq);
static void syscall_syslogdisable(syscallrq_t *rq);
static void syscall_kernelpanicdetect(syscallrq_t *rq);
static void syscall_abort(syscallrq_t *rq);
static void syscall_exit(syscallrq_t *rq);
#if __OS_ENABLE_SYSTEMFUNC__ == _YES_
static void syscall_system(syscallrq_t *rq);
#endif
static void syscall_processcreate(syscallrq_t *rq);
static void syscall_processdestroy(syscallrq_t *rq);
static void syscall_processgetexitsem(syscallrq_t *rq);
static void syscall_processstatseek(syscallrq_t *rq);
static void syscall_processstatpid(syscallrq_t *rq);
static void syscall_processgetpid(syscallrq_t *rq);
static void syscall_processgetprio(syscallrq_t *rq);
#if __OS_ENABLE_GETCWD__ == _YES_
static void syscall_getcwd(syscallrq_t *rq);
static void syscall_setcwd(syscallrq_t *rq);
#endif
static void syscall_threadcreate(syscallrq_t *rq);
static void syscall_threaddestroy(syscallrq_t *rq);
static void syscall_threadexit(syscallrq_t *rq);
static void syscall_threadgetexitsem(syscallrq_t *rq);
static void syscall_semaphorecreate(syscallrq_t *rq);
static void syscall_semaphoredestroy(syscallrq_t *rq);
static void syscall_mutexcreate(syscallrq_t *rq);
static void syscall_mutexdestroy(syscallrq_t *rq);
static void syscall_queuecreate(syscallrq_t *rq);
static void syscall_queuedestroy(syscallrq_t *rq);
#if __ENABLE_NETWORK__ == _YES_
static void syscall_netifup(syscallrq_t *rq);
static void syscall_netifdown(syscallrq_t *rq);
static void syscall_netifstatus(syscallrq_t *rq);
static void syscall_netsocketcreate(syscallrq_t *rq);
static void syscall_netsocketdestroy(syscallrq_t *rq);
static void syscall_netbind(syscallrq_t *rq);
static void syscall_netlisten(syscallrq_t *rq);
static void syscall_netaccept(syscallrq_t *rq);
static void syscall_netrecv(syscallrq_t *rq);
static void syscall_netsend(syscallrq_t *rq);
static void syscall_netgethostbyname(syscallrq_t *rq);
static void syscall_netsetrecvtimeout(syscallrq_t *rq);
static void syscall_netsetsendtimeout(syscallrq_t *rq);
static void syscall_netconnect(syscallrq_t *rq);
static void syscall_netdisconnect(syscallrq_t *rq);
static void syscall_netshutdown(syscallrq_t *rq);
static void syscall_netsendto(syscallrq_t *rq);
static void syscall_netrecvfrom(syscallrq_t *rq);
static void syscall_netgetaddress(syscallrq_t *rq);
#endif

/*==============================================================================
  Local objects
==============================================================================*/
static queue_t *call_request;

/* syscall table */
static const syscallfunc_t syscalltab[] = {
        [SYSCALL_MOUNT            ] = syscall_mount,
        [SYSCALL_UMOUNT           ] = syscall_umount,
    #if __OS_ENABLE_STATFS__ == _YES_
        [SYSCALL_GETMNTENTRY      ] = syscall_getmntentry,
    #endif
    #if __OS_ENABLE_MKNOD__ == _YES_
        [SYSCALL_MKNOD            ] = syscall_mknod,
    #endif
    #if __OS_ENABLE_MKDIR__ == _YES_
        [SYSCALL_MKDIR            ] = syscall_mkdir,
    #endif
    #if __OS_ENABLE_MKFIFO__ == _YES_
        [SYSCALL_MKFIFO           ] = syscall_mkfifo,
    #endif
    #if __OS_ENABLE_DIRBROWSE__ == _YES_
        [SYSCALL_OPENDIR          ] = syscall_opendir,
        [SYSCALL_CLOSEDIR         ] = syscall_closedir,
        [SYSCALL_READDIR          ] = syscall_readdir,
    #endif
    #if __OS_ENABLE_REMOVE__ == _YES_
        [SYSCALL_REMOVE           ] = syscall_remove,
    #endif
    #if __OS_ENABLE_RENAME__ == _YES_
        [SYSCALL_RENAME           ] = syscall_rename,
    #endif
    #if __OS_ENABLE_CHMOD__ == _YES_
        [SYSCALL_CHMOD            ] = syscall_chmod,
    #endif
    #if __OS_ENABLE_CHOWN__ == _YES_
        [SYSCALL_CHOWN            ] = syscall_chown,
    #endif
    #if __OS_ENABLE_STATFS__ == _YES_
        [SYSCALL_STATFS           ] = syscall_statfs,
    #endif
    #if __OS_ENABLE_FSTAT__ == _YES_
        [SYSCALL_STAT             ] = syscall_stat,
    #endif
    #if __OS_ENABLE_FSTAT__ == _YES_
        [SYSCALL_FSTAT            ] = syscall_fstat,
    #endif
        [SYSCALL_FOPEN            ] = syscall_fopen,
        [SYSCALL_FCLOSE           ] = syscall_fclose,
        [SYSCALL_FWRITE           ] = syscall_fwrite,
        [SYSCALL_FREAD            ] = syscall_fread,
        [SYSCALL_FSEEK            ] = syscall_fseek,
        [SYSCALL_IOCTL            ] = syscall_ioctl,
        [SYSCALL_FFLUSH           ] = syscall_fflush,
        [SYSCALL_SYNC             ] = syscall_sync,
    #if __OS_ENABLE_TIMEMAN__ == _YES_
        [SYSCALL_GETTIME          ] = syscall_gettime,
        [SYSCALL_SETTIME          ] = syscall_settime,
    #endif
        [SYSCALL_DRIVERINIT       ] = syscall_driverinit,
        [SYSCALL_DRIVERRELEASE    ] = syscall_driverrelease,
        [SYSCALL_MALLOC           ] = syscall_malloc,
        [SYSCALL_ZALLOC           ] = syscall_zalloc,
        [SYSCALL_FREE             ] = syscall_free,
        [SYSCALL_SYSLOGENABLE     ] = syscall_syslogenable,
        [SYSCALL_SYSLOGDISABLE    ] = syscall_syslogdisable,
        [SYSCALL_KERNELPANICDETECT] = syscall_kernelpanicdetect,
        [SYSCALL_ABORT            ] = syscall_abort,
        [SYSCALL_EXIT             ] = syscall_exit,
    #if __OS_ENABLE_SYSTEMFUNC__ == _YES_
        [SYSCALL_SYSTEM           ] = syscall_system,
    #endif
        [SYSCALL_PROCESSCREATE    ] = syscall_processcreate,
        [SYSCALL_PROCESSDESTROY   ] = syscall_processdestroy,
        [SYSCALL_PROCESSGETEXITSEM] = syscall_processgetexitsem,
        [SYSCALL_PROCESSSTATSEEK  ] = syscall_processstatseek,
        [SYSCALL_PROCESSSTATPID   ] = syscall_processstatpid,
        [SYSCALL_PROCESSGETPID    ] = syscall_processgetpid,
        [SYSCALL_PROCESSGETPRIO   ] = syscall_processgetprio,
    #if __OS_ENABLE_GETCWD__ == _YES_
        [SYSCALL_GETCWD           ] = syscall_getcwd,
        [SYSCALL_SETCWD           ] = syscall_setcwd,
    #endif
        [SYSCALL_THREADCREATE     ] = syscall_threadcreate,
        [SYSCALL_THREADDESTROY    ] = syscall_threaddestroy,
        [SYSCALL_THREADEXIT       ] = syscall_threadexit,
        [SYSCALL_THREADGETEXITSEM ] = syscall_threadgetexitsem,
        [SYSCALL_SEMAPHORECREATE  ] = syscall_semaphorecreate,
        [SYSCALL_SEMAPHOREDESTROY ] = syscall_semaphoredestroy,
        [SYSCALL_MUTEXCREATE      ] = syscall_mutexcreate,
        [SYSCALL_MUTEXDESTROY     ] = syscall_mutexdestroy,
        [SYSCALL_QUEUECREATE      ] = syscall_queuecreate,
        [SYSCALL_QUEUEDESTROY     ] = syscall_queuedestroy,
#if __ENABLE_NETWORK__ == _YES_
        [SYSCALL_NETIFUP          ] = syscall_netifup,
        [SYSCALL_NETIFDOWN        ] = syscall_netifdown,
        [SYSCALL_NETIFSTATUS      ] = syscall_netifstatus,
        [SYSCALL_NETSOCKETCREATE  ] = syscall_netsocketcreate,
        [SYSCALL_NETSOCKETDESTROY ] = syscall_netsocketdestroy,
        [SYSCALL_NETBIND          ] = syscall_netbind,
        [SYSCALL_NETLISTEN        ] = syscall_netlisten,
        [SYSCALL_NETACCEPT        ] = syscall_netaccept,
        [SYSCALL_NETRECV          ] = syscall_netrecv,
        [SYSCALL_NETSEND          ] = syscall_netsend,
        [SYSCALL_NETGETHOSTBYNAME ] = syscall_netgethostbyname,
        [SYSCALL_NETSETRECVTIMEOUT] = syscall_netsetrecvtimeout,
        [SYSCALL_NETSETSENDTIMEOUT] = syscall_netsetsendtimeout,
        [SYSCALL_NETCONNECT       ] = syscall_netconnect,
        [SYSCALL_NETDISCONNECT    ] = syscall_netdisconnect,
        [SYSCALL_NETSHUTDOWN      ] = syscall_netshutdown,
        [SYSCALL_NETSENDTO        ] = syscall_netsendto,
        [SYSCALL_NETRECVFROM      ] = syscall_netrecvfrom,
        [SYSCALL_NETGETADDRESS    ] = syscall_netgetaddress,
#endif
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Initialize system calls.
 */
//==============================================================================
void _syscall_init()
{
        int result = ESUCC;

        result |= _queue_create(SYSCALL_QUEUE_LENGTH, sizeof(syscallrq_t*), &call_request);
        result |= _process_create("kworker", NULL, NULL);
        result |= _process_create(__OS_INIT_PROG__, NULL, NULL);

        if (result != ESUCC) {
                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
        }
}

//==============================================================================
/**
 * @brief  Function call selected syscall [USERSPACE].
 *
 * @param  syscall      syscall number
 * @param  retptr       pointer to return value
 * @param  ...          additional arguments
 */
//==============================================================================
void syscall(syscall_t syscall, void *retptr, ...)
{
        if (syscall < _SYSCALL_COUNT) {
                sem_t *syscall_sem = _builtinfunc(process_get_syscall_sem_by_task, _THIS_TASK);
                if (syscall_sem) {

                        syscallrq_t syscallrq = {
                                .syscall_no     = syscall,
                                .syscall_thread = NULL,
                                .task_client    = _builtinfunc(task_get_handle),
                                .retptr         = retptr,
                                .err            = ESUCC
                        };

                        va_start(syscallrq.args, retptr);
                        {
                                syscallrq_t *syscallrq_ptr = &syscallrq;

                                if (_builtinfunc(queue_send,
                                                 call_request,
                                                 &syscallrq_ptr,
                                                 MAX_DELAY_MS) ==  ESUCC) {

                                        _builtinfunc(process_set_syscall_by_task,
                                                     syscallrq.task_client,
                                                     syscallrq_ptr);

                                        if (_builtinfunc(semaphore_wait,
                                                         syscall_sem,
                                                         MAX_DELAY_MS) == ESUCC) {

                                                if (syscallrq.err) {
                                                        _errno = syscallrq.err;
                                                }
                                        }
                                        _builtinfunc(process_set_syscall_by_task,
                                                     syscallrq.task_client, NULL);
                                }
                        }
                        va_end(syscallrq.args);
                }
        } else {
                _errno = ENOSYS;
        }
}

//==============================================================================
/**
 * @brief  Main syscall process (master) [KERNELSPACE].
 *
 * @param  argc         argument count
 * @param  argv         arguments
 *
 * @return Never exit (0)
 */
//==============================================================================
int _syscall_kworker_process(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        static const thread_attr_t fs_blocking_thread_attr = {
                .stack_depth = STACK_DEPTH_CUSTOM(__OS_FILE_SYSTEM_STACK_DEPTH__),
                .priority    = PRIORITY_NORMAL
        };

        static const thread_attr_t net_blocking_thread_attr = {
                .stack_depth = STACK_DEPTH_CUSTOM(__OS_NETWORK_STACK_DEPTH__),
                .priority    = PRIORITY_NORMAL
        };

        for (;;) {
                syscallrq_t *rq;
                if (_queue_receive(call_request, &rq, MAX_DELAY_MS) == ESUCC) {
                        if (rq->syscall_no <= _SYSCALL_GROUP_0_OS_NON_BLOCKING) {
                                syscall_do(rq);
                        } else {
                                /* select stack size according to syscall group */
                                const thread_attr_t *thread_attr = NULL;
                                if (rq->syscall_no <= _SYSCALL_GROUP_1_FS_BLOCKING) {
                                        thread_attr = &fs_blocking_thread_attr;
                                } else if (rq->syscall_no <= _SYSCALL_GROUP_2_NET_BLOCKING) {
                                        thread_attr = &net_blocking_thread_attr;
                                } else {
                                        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                                        continue;
                                }

                                /* give time for kernel to free resources after last syscall task */
                                _sleep_ms(1);

                                /* create new syscall task */
                                _process_t *proc = _process_get_container_by_task(_THIS_TASK, NULL);
                                switch (_process_thread_create(proc,
                                                               syscall_do,
                                                               thread_attr,
                                                               true,
                                                               rq,
                                                               NULL,
                                                               NULL) ) {
                                case ESUCC:
                                        _task_yield();
                                        break;

                                // destroy top process to get free memory
                                case ENOMEM: {
                                        task_t *task = destroy_top_process();
                                        if (task == rq->task_client) {
                                                break;
                                        } else {
                                                // go through
                                        }
                                }

                                // all other errors
                                default:
                                        _queue_send(call_request, &rq, MAX_DELAY_MS);
                                        _sleep_ms(5);
                                        break;
                                }
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Function destroy top process to get free memory.
 * @return Client task handle.
 */
//==============================================================================
static task_t *destroy_top_process(void)
{
        _process_t  *proc    = _process_get_top();
        syscallrq_t *syscall = NULL;
        task_t      *task    = NULL;

        _process_get_task(proc, &task);
        _process_get_syscall_by_task(task, cast(void*, &syscall));

        // release syscall thread of current process if is pending
        if (syscall && syscall->syscall_thread) {

                _process_t *kworker = _process_get_active();
                task_t     *syscall_task = NULL;

                _process_thread_get_task(syscall->syscall_thread, &syscall_task);
                _task_suspend(syscall_task);

                _process_release_resource(kworker,
                                          cast(res_header_t*, syscall->syscall_thread),
                                          RES_TYPE_THREAD);

                syscall->syscall_thread = NULL;
        }

        // inform parent process that current is closed
        pid_t  pid      = 0;
        sem_t *exit_sem = NULL;
        _process_get_pid(proc, &pid);
        _process_get_exit_sem(pid, &exit_sem);
        _semaphore_signal(exit_sem);

        // print error message
        FILE *stderr = _process_get_stderr(proc);
        const char *msg = "*** Error: out of memory ***\n";
        size_t wrcnt;
        _vfs_fwrite(msg, strlen(msg), &wrcnt, stderr);

        _sleep_ms(20);

        // destroy process
        _process_destroy(pid, NULL);

        return task;
}

//==============================================================================
/**
 * @brief  Function inform syscall owner about finished request.
 *
 * @param  rq           request information
 */
//==============================================================================
static void syscall_do(void *rq)
{
        syscallrq_t *sysrq  = rq;
        _process_t  *client = _process_get_container_by_task(sysrq->task_client, NULL);
        _process_t  *server = _process_get_container_by_task(_THIS_TASK, NULL);
        sem_t       *sem    = _process_get_syscall_sem_by_task(sysrq->task_client);

        if (_process_set_CWD(server, _process_get_CWD(client)) != ESUCC) {
                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
        }

        if (_process_set_syscall_sem_by_task(_THIS_TASK, sem) != ESUCC) {
                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
        }

        sysrq->syscall_thread = _process_thread_get_container_by_task(_task_get_handle());

        syscalltab[sysrq->syscall_no](sysrq);

        _process_set_syscall_sem_by_task(_THIS_TASK, NULL);

        _semaphore_signal(sem);
}

//==============================================================================
/**
 * @brief  This syscall mount selected file system to selected path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mount(syscallrq_t *rq)
{
        GETARG(const char *, FS_name);
        GETARG(const char *, src_path);
        GETARG(const char *, mount_point);
        SETERRNO(_mount(FS_name, src_path, mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall unmount selected file system.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_umount(syscallrq_t *rq)
{
        GETARG(const char *, mount_point);
        SETERRNO(_umount(mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

#if __OS_ENABLE_STATFS__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return information about selected file system.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *rq)
{
        GETARG(int *, seek);
        GETARG(struct mntent *, mntent);
        SETERRNO(_vfs_getmntentry(*seek, mntent));

        int ret;
        switch (GETERRNO()) {
        case ESUCC : ret =  0; break;
        case ENOENT: ret =  1; break;
        default    : ret = -1; break;
        }

        SETRETURN(int, ret);
}
#endif

#if __OS_ENABLE_MKNOD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create device node.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *rq)
{
        GETARG(const char *, pathname);
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        SETERRNO(_vfs_mknod(pathname, _dev_t__create(_module_get_ID(mod_name), *major, *minor)));
        SETRETURN(int, GETERRNO() ? 0 : -1);
}
#endif

#if __OS_ENABLE_MKDIR__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_mkdir(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_MKFIFO__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create FIFO pipe.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_mkfifo(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_DIRBROWSE__ == _YES_
//==============================================================================
/**
 * @brief  This syscall open selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_opendir(syscallrq_t *rq)
{
        GETARG(const char *, path);

        DIR *dir = NULL;
        int  err = _vfs_opendir(path, &dir);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, dir));
                if (err != ESUCC) {
                        _vfs_closedir(dir);
                        dir = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(DIR*, dir);
}

//==============================================================================
/**
 * @brief  This syscall close selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_closedir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, dir), RES_TYPE_DIR);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a dir! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

}

//==============================================================================
/**
 * @brief  This syscall read selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_readdir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);
        dirent_t *dirent = NULL;
        SETERRNO(_vfs_readdir(dir, &dirent));
        SETRETURN(dirent_t*, dirent);
}
#endif

#if __OS_ENABLE_REMOVE__ == _YES_
//==============================================================================
/**
 * @brief  This syscall remove selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_remove(syscallrq_t *rq)
{
        GETARG(const char *, path);
        SETERRNO(_vfs_remove(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_RENAME__ == _YES_
//==============================================================================
/**
 * @brief  This syscall rename selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_rename(syscallrq_t *rq)
{
        GETARG(const char *, oldname);
        GETARG(const char *, newname);
        SETERRNO(_vfs_rename(oldname, newname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_CHMOD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall change mode of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_chmod(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_CHOWN__ == _YES_
//==============================================================================
/**
 * @brief  This syscall change owner and group of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chown(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(uid_t *, owner);
        GETARG(gid_t *, group);
        SETERRNO(_vfs_chown(path, *owner, *group));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_FSTAT__ == _YES_
//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_stat(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_stat(path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by FILE object.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fstat(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_fstat(file, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_STATFS__ == _YES_
//==============================================================================
/**
 * @brief  This syscall read statistics of file system mounted in selected path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(struct statfs *, buf);
        SETERRNO(_vfs_statfs(path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

//==============================================================================
/**
 * @brief  This syscall open selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fopen(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(const char *, mode);

        FILE *file = NULL;
        int   err  = _vfs_fopen(path, mode, &file);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, file));
                if (err != ESUCC) {
                        _vfs_fclose(file, true);
                        file = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(FILE*, file);
}

//==============================================================================
/**
 * @brief  This syscall close selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fclose(syscallrq_t *rq)
{
        GETARG(FILE *, file);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, file), RES_TYPE_FILE);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a file! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

}

//==============================================================================
/**
 * @brief  This syscall write data to selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fwrite(syscallrq_t *rq)
{
        GETARG(const uint8_t *, buf);
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE*, file);

        size_t wrcnt = 0;
        SETERRNO(_vfs_fwrite(buf, (*count) * (*size), &wrcnt, file));
        SETRETURN(size_t, wrcnt / (*size));
}

//==============================================================================
/**
 * @brief  This syscall read data from selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================;
static void syscall_fread(syscallrq_t *rq)
{
        GETARG(uint8_t *, buf);
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE *, file);

        size_t rdcnt = 0;
        SETERRNO(_vfs_fread(buf, (*count) * (*size), &rdcnt, file));
        SETRETURN(size_t, rdcnt / (*size));
}

//==============================================================================
/**
 * @brief  This syscall move file pointer.
 *
 * @param  rq                   syscall request
 */
//==============================================================================;
static void syscall_fseek(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(i64_t *, lseek);
        GETARG(int *, orgin);
        SETERRNO(_vfs_fseek(file, *lseek, *orgin));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall perform not standard operation on selected file/device.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_ioctl(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(int *, request);
        GETARG(va_list *, arg);
        SETERRNO(_vfs_vfioctl(file, *request, *arg));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall flush buffers of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fflush(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_fflush(file));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall synchronize all buffers of filesystems.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_sync(syscallrq_t *rq)
{
        UNUSED_RQ();
        _vfs_sync();
}

#if __OS_ENABLE_TIMEMAN__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_gettime(syscallrq_t *rq)
{
        time_t time = -1;
        SETERRNO(_gettime(&time));
        SETRETURN(time_t, time);
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_settime(syscallrq_t *rq)
{
        GETARG(time_t *, time);
        SETERRNO(_settime(time));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

//==============================================================================
/**
 * @brief  This syscall initialize selected driver and create node.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_driverinit(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        GETARG(const char *, node_path);
        dev_t drvid = -1;
        SETERRNO(_driver_init(mod_name, *major, *minor,  node_path, &drvid));
        SETRETURN(dev_t, drvid);
}

//==============================================================================
/**
 * @brief  This syscall release selected driver.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_driverrelease(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        SETERRNO(_driver_release(_dev_t__create(_module_get_ID(mod_name), *major, *minor)));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_malloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kmalloc(_MM_PROG, *size, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*, mem ? &cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application and clear allocated block.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_zalloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kzalloc(_MM_PROG, *size, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*,  mem ? &cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall free allocated memory by application.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_free(syscallrq_t *rq)
{
        GETARG(void *, mem);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, mem) - 1, RES_TYPE_MEMORY);
        if (err != ESUCC) {
                const char *msg = "*** Error: double free or corruption ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall enable system log functionality in selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_syslogenable(syscallrq_t *rq)
{
        GETARG(const char *, path);
        SETERRNO(_printk_enable(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall disable system log functionality.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_syslogdisable(syscallrq_t *rq)
{
        SETERRNO(_printk_disable());
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_kernelpanicdetect(syscallrq_t *rq)
{
        GETARG(bool *, showmsg);
        SETRETURN(bool, _kernel_panic_detect(*showmsg));
}

//==============================================================================
/**
 * @brief  This syscall abort current process (caller) and set exit code as -1.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_abort(syscallrq_t *rq)
{
        UNUSED_RQ();
        SETERRNO(_process_abort(GETPROCESS()));
}

//==============================================================================
/**
 * @brief  This syscall close current process (caller).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_exit(syscallrq_t *rq)
{
        GETARG(int *, status);
        SETERRNO(_process_exit(GETPROCESS(), *status));
}

#if __OS_ENABLE_SYSTEMFUNC__ == _YES_
//==============================================================================
/**
 * @brief  This syscall start shell application to run command line.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_system(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);

//        GETARG(const char *, cmd);
//        GETARG(pid_t *, pid);
//        GETARG(sem_t **, exit_sem);

        //TODO system() syscall

        SETERRNO(ENOTSUP);
        SETRETURN(int, -1);
}
#endif

//==============================================================================
/**
 * @brief  This syscall create new process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processcreate(syscallrq_t *rq)
{
        GETARG(const char *, cmd);
        GETARG(process_attr_t *, attr);
        pid_t pid = 0;
        SETERRNO(_process_create(cmd, attr, &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall destroy existing process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processdestroy(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(int *, status);
        SETERRNO(_process_destroy(*pid, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return exit semaphore.
 *         then
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetexitsem(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(sem_t **, sem);
        SETERRNO(_process_get_exit_sem(*pid, sem));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by seek.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processstatseek(syscallrq_t *rq)
{
        GETARG(size_t *, seek);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_seek(*seek, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by pid.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processstatpid(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_pid(*pid, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return PID of caller process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetpid(syscallrq_t *rq)
{
        pid_t pid = -1;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall return PID's priority.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetprio(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        int prio = 0;
        SETERRNO(_process_get_priority(*pid, &prio));
        SETRETURN(int, prio);
}

#if __OS_ENABLE_GETCWD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return CWD of current process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getcwd(syscallrq_t *rq)
{
        GETARG(char *, buf);
        GETARG(size_t *, size);

        const char *cwd = NULL;
        if (buf && *size) {
                cwd = _process_get_CWD(GETPROCESS());
                strncpy(buf, cwd, *size);
        }

        SETRETURN(char*, cwd ? buf : NULL);
}

//==============================================================================
/**
 * @brief  This syscall set CWD of current process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_setcwd(syscallrq_t *rq)
{
        GETARG(const char *, cwd);
        SETERRNO(_process_set_CWD(GETPROCESS(), cwd));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

//==============================================================================
/**
 * @brief  This syscall create new thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadcreate(syscallrq_t *rq)
{
        GETARG(thread_func_t, func);
        GETARG(thread_attr_t *, attr);
        GETARG(void *, arg);

        tid_t tid = 0;
        SETERRNO(_process_thread_create(GETPROCESS(), func, attr, false, arg, &tid, NULL));
        SETRETURN(tid_t, tid);
}

//==============================================================================
/**
 * @brief  This syscall destroy thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threaddestroy(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);

        task_t *task = NULL;
        if (_process_thread_get_task(GETTHREAD(*tid), &task) == ESUCC) {
                syscallrq_t *syscall  = NULL;
                bool         detached = false;

                if (  (  (_process_get_detached_flag(task, &detached) == ESUCC)
                      && (detached == true)  )
                   ||
                      (  (_process_get_syscall_by_task(task, cast(void*, &syscall)) == ESUCC)
                      && (syscall == NULL)  )  ) {

                        SETERRNO(_process_release_resource(GETPROCESS(),
                                                           cast(res_header_t*, GETTHREAD(*tid)),
                                                           RES_TYPE_THREAD));

                        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

                } else {
                        SETRETURN(int, EAGAIN);
                }
        } else {
                SETERRNO(ESRCH);
                SETRETURN(int, -1);
        }
}

//==============================================================================
/**
 * @brief  This syscall destroy thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadexit(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        SETERRNO(_process_thread_exit(GETTHREAD(*tid)));
}

//==============================================================================
/**
 * @brief  This syscall join thread with parent (parent wait until thread finish).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadgetexitsem(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        GETARG(sem_t **, sem);
        SETERRNO(_process_thread_get_exit_sem(GETPROCESS(), *tid, sem));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create new semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphorecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, cnt_max);
        GETARG(const size_t *, cnt_init);

        sem_t *sem = NULL;
        int err    = _semaphore_create(*cnt_max, *cnt_init, &sem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, sem));
                if (err != ESUCC) {
                        _semaphore_destroy(sem);
                        sem = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(sem_t*, sem);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphoredestroy(syscallrq_t *rq)
{
        GETARG(sem_t *, sem);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, sem), RES_TYPE_SEMAPHORE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a semaphore! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new mutex.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexcreate(syscallrq_t *rq)
{
        GETARG(const enum mutex_type *, type);

        mutex_t *mtx = NULL;
        int err      = _mutex_create(*type, &mtx);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, mtx));
                if (err != ESUCC) {
                        _mutex_destroy(mtx);
                        mtx = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(mutex_t*, mtx);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected mutex.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexdestroy(syscallrq_t *rq)
{
        GETARG(mutex_t *, mtx);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, mtx), RES_TYPE_MUTEX);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a mutex! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, length);
        GETARG(const size_t *, item_size);

        queue_t *q = NULL;
        int err    = _queue_create(*length, *item_size, &q);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, q));
                if (err != ESUCC) {
                        _queue_destroy(q);
                        q = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(queue_t*, q);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuedestroy(syscallrq_t *rq)
{
        GETARG(queue_t *, q);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, q), RES_TYPE_QUEUE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a queue! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

#if __ENABLE_NETWORK__ == _YES_
//==============================================================================
/**
 * @brief  This syscall up network.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifup(syscallrq_t *rq)
{
        GETARG(NET_family_t *, family);
        GETARG(const NET_generic_config_t *, config);

        SETERRNO(_net_ifup(*family, config));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall down network.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifdown(syscallrq_t *rq)
{
        GETARG(NET_family_t *, family);

        SETERRNO(_net_ifdown(*family));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return network status.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifstatus(syscallrq_t *rq)
{
        GETARG(NET_family_t *, family);
        GETARG(NET_generic_status_t *, status);

        SETERRNO(_net_ifstatus(*family, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create new socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsocketcreate(syscallrq_t *rq)
{
        GETARG(NET_family_t*, family);
        GETARG(NET_protocol_t*, protocol);

        SOCKET *socket = NULL;
        int     err    = _net_socket_create(*family, *protocol, &socket);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socket));
                if (err != ESUCC) {
                        _net_socket_destroy(socket);
                        socket = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(SOCKET*, socket);
}

//==============================================================================
/**
 * @brief  This syscall destroy socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsocketdestroy(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, socket), RES_TYPE_SOCKET);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a socket! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall bind socket with address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netbind(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_bind(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall listen connection on selected socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netlisten(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_listen(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall accept incoming connection.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netaccept(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(SOCKET **, new_socket);

        SOCKET *socknew = NULL;
        int     err     = _net_socket_accept(socket, &socknew);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socknew));
                if (err) {
                        _net_socket_destroy(socknew);
                        socknew = NULL;
                }
        }

        *new_socket = socknew;

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netrecv(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t recved = 0;
        SETERRNO(_net_socket_recv(socket, buf, *len, *flags, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netrecvfrom(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        size_t recved = 0;
        SETERRNO(_net_socket_recvfrom(socket, buf, *len, *flags, sockaddr, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsend(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t sent = 0;
        SETERRNO(_net_socket_send(socket, buf, *len, *flags, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsendto(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(const NET_generic_sockaddr_t *, to_addr);

        size_t sent = 0;
        SETERRNO(_net_socket_sendto(socket, buf, *len, *flags, to_addr, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
}

//==============================================================================
/**
 * @brief  This syscall gets address of server by name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netgethostbyname(syscallrq_t *rq)
{
        GETARG(NET_family_t *, family);
        GETARG(const char *, name);
        GETARG(NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_gethostbyname(*family, name, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall set receive timeout of socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsetrecvtimeout(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_recv_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall set send timeout of socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsetsendtimeout(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_send_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall connect socket to address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netconnect(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_connect(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall disconnect socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netdisconnect(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_disconnect(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall shut down selected connection direction.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netshutdown(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(NET_shut_t *, how);

        SETERRNO(_net_socket_shutdown(socket, *how));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return socket address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netgetaddress(syscallrq_t *rq)
{
        GETARG(SOCKET *, socket);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        SETERRNO(_net_socket_getaddress(socket, sockaddr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/