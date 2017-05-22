         the Free Software Foundation and modified by the dnx RTOS exception.
         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         Full license text is available on the following file: doc/license.txt.
#if __EEFS_LOG_ENABLE__ > 0
#define DBG(...) printk("EEFS: "__VA_ARGS__)
#else
#define DBG(...)
                                                DBG("enabled cache write-through");
                                                DBG("readonly mount");
                        DBG("init error %d", err);
                        sys_cache_drop(hdl->srcdev);
        DBG("mknod '%s' (%d)", path, err);
        DBG("mkdir '%s' (%d)", path, err);
        DBG("opendir '%s' (%d)", path, err);
        DBG("closedir (%d)", err);
                DBG("readdir, broken descriptor");
        DBG("readdir (%d)", err);
        DBG("remove '%s' (%d)", path, err);
        DBG("rename '%s' -> '%s' (%d)", old_name, new_name, err);
        DBG("chmod '%s' 0%o", path, mode);
        DBG("chown '%s' %d %d (%d)", path, owner, group, err);
        DBG("stat '%s' (%d)", path, err);
        DBG("fstat (%d)", err);
        DBG("statfs (%d)", err);
        DBG("file open '%s' (%d)", path, err);
        DBG("close file '%d' (%d)", cast(file_desc_t*, fhdl)->block_num, err);
        return sys_cache_read(hdl->srcdev, blk->num, sizeof(block_t), 1,
                return sys_cache_write(hdl->srcdev, blk->num, sizeof(block_t), 1,
                DBG("load_block() '%s'", path);
                        DBG("load_dir_block() invalid block");
                strlcpy(dirent->name, name, NAME_LEN);
                // remove slash at the end if exist
                char *lch = &LAST_CHARACTER(dirent->name);
                if (*lch == '/') {
                        *lch = '\0';
                }
        DBG("added file chain (%d)", err);