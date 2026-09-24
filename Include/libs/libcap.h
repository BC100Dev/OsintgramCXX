#ifndef CAP_DYNAMIC_H
#define CAP_DYNAMIC_H

#ifndef __linux__
#error "This header cannot be used outside Linux-capable systems"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <dlfcn.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>

#ifndef __user
#define __user
#endif
#include <linux/capability.h>

#define LIBCAP_MAJOR 2
#define LIBCAP_MINOR 77

typedef struct _cap_struct* cap_t;
typedef struct cap_iab_s* cap_iab_t;
typedef struct cap_launch_s* cap_launch_t;
typedef int cap_value_t;

typedef enum {
    CAP_EFFECTIVE = 0,
    CAP_PERMITTED = 1,
    CAP_INHERITABLE = 2
} cap_flag_t;

typedef enum {
    CAP_IAB_INH = 2,
    CAP_IAB_AMB = 3,
    CAP_IAB_BOUND = 4
} cap_iab_vector_t;

typedef enum {
    CAP_CLEAR = 0,
    CAP_SET = 1
} cap_flag_value_t;

typedef unsigned cap_mode_t;
#define CAP_MODE_UNCERTAIN   ((cap_mode_t) 0)
#define CAP_MODE_NOPRIV      ((cap_mode_t) 1)
#define CAP_MODE_PURE1E_INIT ((cap_mode_t) 2)
#define CAP_MODE_PURE1E      ((cap_mode_t) 3)
#define CAP_MODE_HYBRID      ((cap_mode_t) 4)

#define CAP_DIFFERS(result, flag)       (((result) & (1 << (flag))) != 0)
#define CAP_IAB_DIFFERS(result, vector) (((result) & (1 << (vector))) != 0)
#define CAP_IS_SUPPORTED(cap)           (cap_get_bound(cap) >= 0)
#define CAP_AMBIENT_SUPPORTED()         (cap_get_ambient(CAP_CHOWN) >= 0)

typedef cap_value_t (*cap_max_bits_t)(void);
typedef char* (*cap_proc_root_t)(const char*);
typedef cap_t (*cap_dup_t)(cap_t);
typedef int (*cap_free_t)(void*);
typedef cap_t (*cap_init_t)(void);
typedef cap_iab_t (*cap_iab_dup_t)(cap_iab_t);
typedef cap_iab_t (*cap_iab_init_t)(void);
typedef int (*cap_get_flag_t)(cap_t, cap_value_t, cap_flag_t, cap_flag_value_t*);
typedef int (*cap_set_flag_t)(cap_t, cap_flag_t, int, const cap_value_t*, cap_flag_value_t);
typedef int (*cap_clear_t)(cap_t);
typedef int (*cap_clear_flag_t)(cap_t, cap_flag_t);
typedef int (*cap_fill_flag_t)(cap_t, cap_flag_t, cap_t, cap_flag_t);
typedef int (*cap_fill_t)(cap_t, cap_flag_t, cap_flag_t);
typedef int (*cap_compare_t)(cap_t, cap_t);
typedef int (*cap_iab_compare_t)(cap_iab_t, cap_iab_t);
typedef cap_flag_value_t (*cap_iab_get_vector_t)(cap_iab_t, cap_iab_vector_t, cap_value_t);
typedef int (*cap_iab_set_vector_t)(cap_iab_t, cap_iab_vector_t, cap_value_t, cap_flag_value_t);
typedef int (*cap_iab_fill_t)(cap_iab_t, cap_iab_vector_t, cap_t, cap_flag_t);
typedef cap_t (*cap_get_fd_t)(int);
typedef cap_t (*cap_get_file_t)(const char*);
typedef uid_t (*cap_get_nsowner_t)(cap_t);
typedef int (*cap_set_fd_t)(int, cap_t);
typedef int (*cap_set_file_t)(const char*, cap_t);
typedef int (*cap_set_nsowner_t)(cap_t, uid_t);
typedef cap_t (*cap_get_proc_t)(void);
typedef cap_t (*cap_get_pid_t)(pid_t);
typedef int (*cap_set_proc_t)(cap_t);
typedef int (*cap_get_bound_t)(cap_value_t);
typedef int (*cap_drop_bound_t)(cap_value_t);
typedef int (*cap_get_ambient_t)(cap_value_t);
typedef int (*cap_set_ambient_t)(cap_value_t, cap_flag_value_t);
typedef int (*cap_reset_ambient_t)(void);
typedef ssize_t (*cap_size_t)(cap_t);
typedef ssize_t (*cap_copy_ext_t)(void*, cap_t, ssize_t);
typedef cap_t (*cap_copy_int_t)(const void*);
typedef cap_t (*cap_copy_int_check_t)(const void*, ssize_t);
typedef cap_t (*cap_from_text_t)(const char*);
typedef char* (*cap_to_text_t)(cap_t, ssize_t*);
typedef int (*cap_from_name_t)(const char*, cap_value_t*);
typedef char* (*cap_to_name_t)(cap_value_t);
typedef char* (*cap_iab_to_text_t)(cap_iab_t);
typedef cap_iab_t (*cap_iab_from_text_t)(const char*);
typedef void (*cap_set_syscall_t)(
    long int (*)(long int, long int, long int, long int),
    long int (*)(long int, long int, long int, long int, long int, long int, long int)
);
typedef int (*cap_set_mode_t)(cap_mode_t);
typedef cap_mode_t (*cap_get_mode_t)(void);
typedef const char* (*cap_mode_name_t)(cap_mode_t);
typedef unsigned (*cap_get_secbits_t)(void);
typedef int (*cap_set_secbits_t)(unsigned);
typedef int (*cap_prctl_t)(long int, long int, long int, long int, long int, long int);
typedef int (*cap_prctlw_t)(long int, long int, long int, long int, long int, long int);
typedef int (*cap_setuid_t)(uid_t);
typedef int (*cap_setgroups_t)(gid_t, size_t, const gid_t*);
typedef cap_iab_t (*cap_iab_get_proc_t)(void);
typedef cap_iab_t (*cap_iab_get_pid_t)(pid_t);
typedef int (*cap_iab_set_proc_t)(cap_iab_t);
typedef cap_launch_t (*cap_new_launcher_t)(const char*, const char* const *, const char* const *);
typedef cap_launch_t (*cap_func_launcher_t)(int (*)(void*));
typedef int (*cap_launcher_callback_t)(cap_launch_t, int (*)(void*));
typedef int (*cap_launcher_setuid_t)(cap_launch_t, uid_t);
typedef int (*cap_launcher_setgroups_t)(cap_launch_t, gid_t, int, const gid_t*);
typedef int (*cap_launcher_set_mode_t)(cap_launch_t, cap_mode_t);
typedef cap_iab_t (*cap_launcher_set_iab_t)(cap_launch_t, cap_iab_t);
typedef int (*cap_launcher_set_chroot_t)(cap_launch_t, const char*);
typedef pid_t (*cap_launch_t_fn)(cap_launch_t, void*);
typedef int (*capget_t)(cap_user_header_t, cap_user_data_t);
typedef int (*capset_t)(cap_user_header_t, const cap_user_data_t);
typedef int (*capgetp_t)(pid_t, cap_t);
typedef int (*capsetp_t)(pid_t, cap_t);

__attribute__((weak)) void* _libcap_handle = NULL;

__attribute__((weak)) cap_max_bits_t cap_max_bits = NULL;
__attribute__((weak)) cap_proc_root_t cap_proc_root = NULL;
__attribute__((weak)) cap_dup_t cap_dup = NULL;
__attribute__((weak)) cap_free_t cap_free = NULL;
__attribute__((weak)) cap_init_t cap_init = NULL;
__attribute__((weak)) cap_iab_dup_t cap_iab_dup = NULL;
__attribute__((weak)) cap_iab_init_t cap_iab_init = NULL;
__attribute__((weak)) cap_get_flag_t cap_get_flag = NULL;
__attribute__((weak)) cap_set_flag_t cap_set_flag = NULL;
__attribute__((weak)) cap_clear_t cap_clear = NULL;
__attribute__((weak)) cap_clear_flag_t cap_clear_flag = NULL;
__attribute__((weak)) cap_fill_flag_t cap_fill_flag = NULL;
__attribute__((weak)) cap_fill_t cap_fill = NULL;
__attribute__((weak)) cap_compare_t cap_compare = NULL;
__attribute__((weak)) cap_iab_compare_t cap_iab_compare = NULL;
__attribute__((weak)) cap_iab_get_vector_t cap_iab_get_vector = NULL;
__attribute__((weak)) cap_iab_set_vector_t cap_iab_set_vector = NULL;
__attribute__((weak)) cap_iab_fill_t cap_iab_fill = NULL;
__attribute__((weak)) cap_get_fd_t cap_get_fd = NULL;
__attribute__((weak)) cap_get_file_t cap_get_file = NULL;
__attribute__((weak)) cap_get_nsowner_t cap_get_nsowner = NULL;
__attribute__((weak)) cap_set_fd_t cap_set_fd = NULL;
__attribute__((weak)) cap_set_file_t cap_set_file = NULL;
__attribute__((weak)) cap_set_nsowner_t cap_set_nsowner = NULL;
__attribute__((weak)) cap_get_proc_t cap_get_proc = NULL;
__attribute__((weak)) cap_get_pid_t cap_get_pid = NULL;
__attribute__((weak)) cap_set_proc_t cap_set_proc = NULL;
__attribute__((weak)) cap_get_bound_t cap_get_bound = NULL;
__attribute__((weak)) cap_drop_bound_t cap_drop_bound = NULL;
__attribute__((weak)) cap_get_ambient_t cap_get_ambient = NULL;
__attribute__((weak)) cap_set_ambient_t cap_set_ambient = NULL;
__attribute__((weak)) cap_reset_ambient_t cap_reset_ambient = NULL;
__attribute__((weak)) cap_size_t cap_size = NULL;
__attribute__((weak)) cap_copy_ext_t cap_copy_ext = NULL;
__attribute__((weak)) cap_copy_int_t cap_copy_int = NULL;
__attribute__((weak)) cap_copy_int_check_t cap_copy_int_check = NULL;
__attribute__((weak)) cap_from_text_t cap_from_text = NULL;
__attribute__((weak)) cap_to_text_t cap_to_text = NULL;
__attribute__((weak)) cap_from_name_t cap_from_name = NULL;
__attribute__((weak)) cap_to_name_t cap_to_name = NULL;
__attribute__((weak)) cap_iab_to_text_t cap_iab_to_text = NULL;
__attribute__((weak)) cap_iab_from_text_t cap_iab_from_text = NULL;
__attribute__((weak)) cap_set_syscall_t cap_set_syscall = NULL;
__attribute__((weak)) cap_set_mode_t cap_set_mode = NULL;
__attribute__((weak)) cap_get_mode_t cap_get_mode = NULL;
__attribute__((weak)) cap_mode_name_t cap_mode_name = NULL;
__attribute__((weak)) cap_get_secbits_t cap_get_secbits = NULL;
__attribute__((weak)) cap_set_secbits_t cap_set_secbits = NULL;
__attribute__((weak)) cap_prctl_t cap_prctl = NULL;
__attribute__((weak)) cap_prctlw_t cap_prctlw = NULL;
__attribute__((weak)) cap_setuid_t cap_setuid = NULL;
__attribute__((weak)) cap_setgroups_t cap_setgroups = NULL;
__attribute__((weak)) cap_iab_get_proc_t cap_iab_get_proc = NULL;
__attribute__((weak)) cap_iab_get_pid_t cap_iab_get_pid = NULL;
__attribute__((weak)) cap_iab_set_proc_t cap_iab_set_proc = NULL;
__attribute__((weak)) cap_new_launcher_t cap_new_launcher = NULL;
__attribute__((weak)) cap_func_launcher_t cap_func_launcher = NULL;
__attribute__((weak)) cap_launcher_callback_t cap_launcher_callback = NULL;
__attribute__((weak)) cap_launcher_setuid_t cap_launcher_setuid = NULL;
__attribute__((weak)) cap_launcher_setgroups_t cap_launcher_setgroups = NULL;
__attribute__((weak)) cap_launcher_set_mode_t cap_launcher_set_mode = NULL;
__attribute__((weak)) cap_launcher_set_iab_t cap_launcher_set_iab = NULL;
__attribute__((weak)) cap_launcher_set_chroot_t cap_launcher_set_chroot = NULL;
__attribute__((weak)) cap_launch_t_fn cap_launch = NULL;
__attribute__((weak)) capget_t capget = NULL;
__attribute__((weak)) capset_t capset = NULL;
__attribute__((weak)) capgetp_t capgetp = NULL;
__attribute__((weak)) capsetp_t capsetp = NULL;

#define CAP_LOAD_SYM(handle, name) \
    name = (name##_t) dlsym(handle, #name)

__attribute__((weak)) int cap_dynamic_init() {
    _libcap_handle = dlopen("libcap.so.2", RTLD_LAZY | RTLD_GLOBAL);
    if (!_libcap_handle) return -1;

    CAP_LOAD_SYM(_libcap_handle, cap_max_bits);
    CAP_LOAD_SYM(_libcap_handle, cap_proc_root);
    CAP_LOAD_SYM(_libcap_handle, cap_dup);
    CAP_LOAD_SYM(_libcap_handle, cap_free);
    CAP_LOAD_SYM(_libcap_handle, cap_init);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_dup);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_init);
    CAP_LOAD_SYM(_libcap_handle, cap_get_flag);
    CAP_LOAD_SYM(_libcap_handle, cap_set_flag);
    CAP_LOAD_SYM(_libcap_handle, cap_clear);
    CAP_LOAD_SYM(_libcap_handle, cap_clear_flag);
    CAP_LOAD_SYM(_libcap_handle, cap_fill_flag);
    CAP_LOAD_SYM(_libcap_handle, cap_fill);
    CAP_LOAD_SYM(_libcap_handle, cap_compare);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_compare);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_get_vector);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_set_vector);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_fill);
    CAP_LOAD_SYM(_libcap_handle, cap_get_fd);
    CAP_LOAD_SYM(_libcap_handle, cap_get_file);
    CAP_LOAD_SYM(_libcap_handle, cap_get_nsowner);
    CAP_LOAD_SYM(_libcap_handle, cap_set_fd);
    CAP_LOAD_SYM(_libcap_handle, cap_set_file);
    CAP_LOAD_SYM(_libcap_handle, cap_set_nsowner);
    CAP_LOAD_SYM(_libcap_handle, cap_get_proc);
    CAP_LOAD_SYM(_libcap_handle, cap_get_pid);
    CAP_LOAD_SYM(_libcap_handle, cap_set_proc);
    CAP_LOAD_SYM(_libcap_handle, cap_get_bound);
    CAP_LOAD_SYM(_libcap_handle, cap_drop_bound);
    CAP_LOAD_SYM(_libcap_handle, cap_get_ambient);
    CAP_LOAD_SYM(_libcap_handle, cap_set_ambient);
    CAP_LOAD_SYM(_libcap_handle, cap_reset_ambient);
    CAP_LOAD_SYM(_libcap_handle, cap_size);
    CAP_LOAD_SYM(_libcap_handle, cap_copy_ext);
    CAP_LOAD_SYM(_libcap_handle, cap_copy_int);
    CAP_LOAD_SYM(_libcap_handle, cap_copy_int_check);
    CAP_LOAD_SYM(_libcap_handle, cap_from_text);
    CAP_LOAD_SYM(_libcap_handle, cap_to_text);
    CAP_LOAD_SYM(_libcap_handle, cap_from_name);
    CAP_LOAD_SYM(_libcap_handle, cap_to_name);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_to_text);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_from_text);
    CAP_LOAD_SYM(_libcap_handle, cap_set_syscall);
    CAP_LOAD_SYM(_libcap_handle, cap_set_mode);
    CAP_LOAD_SYM(_libcap_handle, cap_get_mode);
    CAP_LOAD_SYM(_libcap_handle, cap_mode_name);
    CAP_LOAD_SYM(_libcap_handle, cap_get_secbits);
    CAP_LOAD_SYM(_libcap_handle, cap_set_secbits);
    CAP_LOAD_SYM(_libcap_handle, cap_prctl);
    CAP_LOAD_SYM(_libcap_handle, cap_prctlw);
    CAP_LOAD_SYM(_libcap_handle, cap_setuid);
    CAP_LOAD_SYM(_libcap_handle, cap_setgroups);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_get_proc);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_get_pid);
    CAP_LOAD_SYM(_libcap_handle, cap_iab_set_proc);
    CAP_LOAD_SYM(_libcap_handle, cap_new_launcher);
    CAP_LOAD_SYM(_libcap_handle, cap_func_launcher);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_callback);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_setuid);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_setgroups);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_set_mode);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_set_iab);
    CAP_LOAD_SYM(_libcap_handle, cap_launcher_set_chroot);
    cap_launch = (cap_launch_t_fn)dlsym(_libcap_handle, "cap_launch");
    CAP_LOAD_SYM(_libcap_handle, capget);
    CAP_LOAD_SYM(_libcap_handle, capset);
    CAP_LOAD_SYM(_libcap_handle, capgetp);
    CAP_LOAD_SYM(_libcap_handle, capsetp);

    return 0;
}

__attribute__((weak)) void cap_dynamic_close(void) {
    if (_libcap_handle) {
        dlclose(_libcap_handle);
        _libcap_handle = NULL;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* CAP_DYNAMIC_H */
