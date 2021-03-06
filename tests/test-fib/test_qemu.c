#include "udynlink.h"
#include "mod_fib_module_data.h"
#include "test_utils.h"
#include <stdio.h>

int test_qemu(void) {
    const char *exported_syms[] = {"test", NULL};
    udynlink_module_t *p_mod;
    int res = 0;

    for (int i = (int)_UDYNLINK_LOAD_MODE_FIRST; i <= (int)_UDYNLINK_LOAD_MODE_LAST; i ++) {
        if ((p_mod = udynlink_load_module(mod_fib_module_data, NULL, 0, (udynlink_load_mode_t)i, NULL)) == NULL)
            return 0;
        CHECK_RAM_SIZE(p_mod, 0);
        // This module should also have a total RAM size of 0 in XIP mode
        if(((udynlink_load_mode_t)i == UDYNLINK_LOAD_MODE_XIP) && (udynlink_get_ram_size(p_mod) != 0))
            goto exit;
        if (!check_exported_symbols(p_mod, exported_syms))
            goto exit;
        if (!run_test_func(p_mod))
            goto exit;
        udynlink_unload_module(p_mod);
    }
    res = 1;
    p_mod = NULL;
exit:
    if (p_mod)
        udynlink_unload_module(p_mod);
    return res;
}
