        size_t           size;                  /* file size                 */
        time_t           mtime;                 /* time of last modification */
                                dir->d_items    = sys_llist_size(parent->data);
                                dir->d_readdir  = ramfs_readdir;
                                dir->d_closedir = ramfs_closedir;
                                dir->d_seek     = 0;
                                dir->d_dd       = parent;
                node_t *parent = dir->d_dd;
                node_t *child  = sys_llist_at(parent->data, dir->d_seek++);
                        char *newname;
                        result = sys_zalloc(strsize(basename), cast(void**, &newname));
                                strcpy(newname, basename);

                                target->name = newname;
                                        stat->st_dev = 0;
 * @param[in ]          *fhdl                   file handle
API_FS_FSTAT(ramfs, void *fs_handle, void *extra, struct stat *stat)
                                stat->st_dev = 0;
 * @param[out]          *fhdl                   file handle
API_FS_OPEN(ramfs, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
                *fhdl = sys_llist_back(hdl->opended_files);
 * @param[in ]          *fhdl                   file handle
API_FS_CLOSE(ramfs, void *fs_handle, void *fhdl, bool force)
                struct opened_file_info *opened_file = fhdl;
 * @param[in ]          *fhdl                   file handle
             void            *fhdl,
                struct opened_file_info *opened_file = fhdl;
                        sys_gettime(&target->mtime);

 * @param[in ]          *fhdl                   file handle
            void            *fhdl,
                struct opened_file_info *opened_file = fhdl;
 * @param[in ]          *fhdl                   file handle
API_FS_IOCTL(ramfs, void *fs_handle, void *fhdl, int request, void *arg)
                struct opened_file_info *opened_file = fhdl;
 * @param[in ]          *fhdl                   file handle
API_FS_FLUSH(ramfs, void *fs_handle, void *fhdl)
                struct opened_file_info *opened_file = fhdl;