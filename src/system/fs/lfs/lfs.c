        uid_t            uid;                   /* user ID of owner          */
        gid_t            gid;                   /* group ID of owner         */
        fpos_t           size;                  /* file size                 */
API_FS_CHMOD(lfs, void *fs_handle, const char *path, mode_t mode)
API_FS_CHOWN(lfs, void *fs_handle, const char *path, uid_t owner, gid_t group)
 * @param[in]            flags                  file open flags
API_FS_OPEN(lfs, void *fs_handle, void **extra, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags)
                if (!(flags & O_CREATE)) {
                if ((flags & O_CREATE) && !(flags & O_APPEND)) {
                if (driver_open((dev_t)node->data, flags) == STD_RET_OK) {
API_FS_WRITE(lfs, void *fs_handle, void *extra, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
API_FS_READ(lfs, void *fs_handle, void *extra, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
                if (request != IOCTL_PIPE__CLOSE) {
//==============================================================================
/**
 * @brief Synchronize all buffers to a medium
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return None
 */
//==============================================================================
API_FS_SYNC(lfs, void *fs_handle)
{
        UNUSED_ARG(fs_handle);
}
