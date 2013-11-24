#include "system/thread.h"
#include "system/thread.h"
                union {
                        struct vfs_drv_interface *drv;
                        queue_t                  *pipe;
                        void                     *generic;
                } nif;
                tfile_t                   type;
        struct devfs_chain *next;
        int                 number_of_opened_files;
static stdret_t            closedir                     (void *fs_handle, DIR *dir);
static dirent_t            readdir                      (void *fs_handle, DIR *dir);
static struct devfs_chain *chain_new                    (void);
static void                chain_delete                 (struct devfs_chain *chain);
static struct devnode     *chain_get_node_by_path       (struct devfs_chain *chain, const char *path);
static struct devnode     *chain_get_empty_node         (struct devfs_chain *chain);
static struct devnode     *chain_get_n_node             (struct devfs_chain *chain, int n);
static int                 create_new_chain_if_necessary(struct devfs *devfs);
        mutex_t            *mtx   = mutex_new(MUTEX_NORMAL);
        struct devfs_chain *chain = chain_new();
                devfs->number_of_opened_files   = 0;
        } else {
                if (devfs) {
                        free(devfs);
                }
                if (mtx) {
                        mutex_delete(mtx);
                }
                if (chain) {
                        chain_delete(chain);
                }
        if (mutex_lock(devfs->mutex, 100)) {
                if (devfs->number_of_opened_files != 0) {
                        mutex_unlock(devfs->mutex);
                        errno = EBUSY;
                critical_section_begin();
                mutex_unlock(devfs->mutex);
                chain_delete(devfs->root_chain);
                mutex_delete(devfs->mutex);
                critical_section_end();
        } else {
                errno = EBUSY;
                return STD_RET_ERROR;
        }
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                        stdret_t open = STD_RET_ERROR;
                        if (node->type == FILE_TYPE_DRV) {
                                open = node->nif.drv->drv_open(node->nif.drv->handle, O_DEV_FLAGS(flags));
                        } else if (node->type == FILE_TYPE_PIPE) {
                                open = STD_RET_OK;
                        }
                        if (open == STD_RET_OK) {
                                devfs->number_of_opened_files++;
                mutex_unlock(devfs->mutex);
        } else {
                errno = EBUSY;
        stdret_t close = STD_RET_ERROR;
        if (node->type == FILE_TYPE_DRV) {
                close = node->nif.drv->drv_close(node->nif.drv->handle, force, file_owner);
        } else if (node->type == FILE_TYPE_PIPE) {
                close = STD_RET_OK;
        }

        if (close == STD_RET_OK) {
                if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                        devfs->number_of_opened_files--;
                        mutex_unlock(devfs->mutex);
        } else {
                return STD_RET_ERROR;
 * @return number of written bytes, -1 if error
        if (node->type == FILE_TYPE_DRV) {
                return node->nif.drv->drv_write(node->nif.drv->handle, src, count, fpos);
        } else if (node->type == FILE_TYPE_PIPE) {
                for (size_t i = 0; i < count; i++) {
                        if (queue_send(node->nif.pipe, src + i, MAX_DELAY) == false) {
                                i--;
                        }
                }

                return count;
        } else {
                return -1;
        }
 * @return number of read bytes, -1 if error
        if (node->type == FILE_TYPE_DRV) {
                return node->nif.drv->drv_read(node->nif.drv->handle, dst, count, fpos);
        } else if (node->type == FILE_TYPE_PIPE) {
                for (size_t i = 0; i < count; i++) {
                        if (queue_receive(node->nif.pipe, dst + i, MAX_DELAY) == false) {
                                i--;
                        }
                }

                return count;
        } else {
                return -1;
        }
        if (node->type == FILE_TYPE_DRV) {
                return node->nif.drv->drv_ioctl(node->nif.drv->handle, request, arg);
        } else {
                return -1;
        }
        if (node->type == FILE_TYPE_DRV) {
                return node->nif.drv->drv_flush(node->nif.drv->handle);
        } else {
                return -1;
        }
API_FS_FSTAT(devfs, void *fs_handle, void *extra, fd_t fd, struct stat *stat)
        int                 pipelen;
        stdret_t            getfstat = STD_RET_ERROR;

        if (node->type == FILE_TYPE_DRV) {
                getfstat = node->nif.drv->drv_stat(node->nif.drv->handle, &devstat);
                if (getfstat == STD_RET_OK) {
                        stat->st_dev  = devstat.st_major << 8 | devstat.st_minor;
                        stat->st_size = devstat.st_size;
                        stat->st_type = FILE_TYPE_DRV;
                }
        } else if (node->type == FILE_TYPE_PIPE) {
                pipelen = queue_get_number_of_items(node->nif.pipe);
                if (pipelen >= 0) {
                        stat->st_size = pipelen;
                        stat->st_type = FILE_TYPE_PIPE;
                        stat->st_dev  = 0;

                        getfstat = STD_RET_OK;
                } else {
                        getfstat = STD_RET_ERROR;
                }
        }

        if (getfstat == STD_RET_OK) {
        } else {
                return STD_RET_ERROR;
 * @param[in ]           mode                   dir mode
API_FS_MKDIR(devfs, void *fs_handle, const char *path, mode_t mode)
        UNUSED_ARG(mode);

        errno = EPERM;
//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_FS_MKFIFO(devfs, void *fs_handle, const char *path, mode_t mode)
{
        STOP_IF(!fs_handle);
        STOP_IF(!path);

        struct devfs *devfs  = fs_handle;
        stdret_t      status = STD_RET_ERROR;

        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {

                if (create_new_chain_if_necessary(devfs) >= 0) {

                        struct devnode *node = chain_get_empty_node(devfs->root_chain);
                        if (node) {
                                node->nif.pipe = queue_new(CONFIG_STREAM_BUFFER_LENGTH, sizeof(char));
                                node->path     = malloc(strlen(path + 1) + 1);

                                if (node->nif.pipe && node->path) {
                                        strcpy(node->path, path + 1);
                                        node->gid  = 0;
                                        node->uid  = 0;
                                        node->mode = mode;
                                        node->type = FILE_TYPE_PIPE;

                                        devfs->number_of_used_nodes++;

                                        status = STD_RET_OK;
                                } else {
                                        if (node->nif.pipe) {
                                                queue_delete(node->nif.pipe);
                                                node->nif.pipe = NULL;
                                        }

                                        if (node->path) {
                                                free(node->path);
                                                node->path = NULL;
                                        }

                                        errno = ENOSPC;
                                }
                        }
                }

                mutex_unlock(devfs->mutex);
        }

        return status;
}

        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                if (create_new_chain_if_necessary(devfs) >= 0) {
                        struct devnode *node = chain_get_empty_node(devfs->root_chain);
                        if (node) {
                                node->nif.drv = malloc(sizeof(struct vfs_drv_interface));
                                node->path    = malloc(strlen(path + 1) + 1);
                                if (node->nif.drv && node->path) {
                                        *node->nif.drv = *drv_if;
                                        strcpy(node->path, path + 1);
                                        node->gid  = 0;
                                        node->uid  = 0;
                                        node->mode = S_IRUSR | S_IWUSR | S_IRGRO | S_IWGRO | S_IROTH | S_IWOTH;
                                        node->type = FILE_TYPE_DRV;
                                        devfs->number_of_used_nodes++;
                                        status = STD_RET_OK;
                                } else {
                                        if (node->nif.drv) {
                                                free(node->nif.drv);
                                                node->nif.drv = NULL;
                                        }
                                        if (node->path) {
                                                free(node->path);
                                                node->path = NULL;
                                        }
                                        errno = ENOSPC;
                                }
                mutex_unlock(devfs->mutex);
        } else {
                errno = ENOENT;
                return STD_RET_ERROR;
        dirent.filetype = FILE_TYPE_REGULAR;
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_n_node(devfs->root_chain, dir->f_seek);
                        if (node->type == FILE_TYPE_DRV) {
                                struct vfs_dev_stat devstat;
                                if (node->nif.drv->drv_stat(node->nif.drv->handle, &devstat) == STD_RET_OK) {
                                        dirent.size     = devstat.st_size;
                                        dirent.filetype = FILE_TYPE_DRV;
                                } else {
                                        dirent.size = 0;
                                }
                        } else if (node->type == FILE_TYPE_PIPE) {
                                int n = queue_get_number_of_items(node->nif.pipe);
                                if (n >= 0) {
                                        dirent.size     = n;
                                        dirent.filetype = FILE_TYPE_PIPE;
                                } else {
                                        dirent.size = 0;
                                }
                        }
                mutex_unlock(devfs->mutex);
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                                if (node->type == FILE_TYPE_DRV) {
                                        free(node->nif.drv);
                                } else if (node->type == FILE_TYPE_PIPE) {
                                        queue_delete(node->nif.pipe);
                                }
                                node->nif.generic = NULL;
                                free(node->path);
                        } else {
                                errno = EBUSY;
                mutex_unlock(devfs->mutex);
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, old_name);
                        } else {
                                errno = ENOMEM;
                mutex_unlock(devfs->mutex);
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                mutex_unlock(devfs->mutex);
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                mutex_unlock(devfs->mutex);
API_FS_STAT(devfs, void *fs_handle, const char *path, struct stat *stat)
        if (mutex_lock(devfs->mutex, TIMEOUT_MS)) {
                struct devnode *node = chain_get_node_by_path(devfs->root_chain, path);
                        status = _devfs_fstat(devfs, node, 0, stat);
                mutex_unlock(devfs->mutex);
 *        Errno: ENOENT
 * @param[in] chain             chain
 * @param[in] path              node's path
static struct devnode *chain_get_node_by_path(struct devfs_chain *chain, const char *path)
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                        if (nchain->devnode[i].nif.generic == NULL)
                        if (strcmp(nchain->devnode[i].path, path + 1) == 0)
                                return &nchain->devnode[i];
        errno = ENOENT;

 *        Errno: ENOENT
 * @param[in] chain             chain
static struct devnode *chain_get_empty_node(struct devfs_chain *chain)
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                        if (nchain->devnode[i].nif.generic == NULL)
                                return &nchain->devnode[i];
        errno = ENOENT;

 *        Errno: ENOENT
static struct devnode *chain_get_n_node(struct devfs_chain *chain, int n)
        for (struct devfs_chain *nchain = chain; nchain != NULL; nchain = nchain->next) {
                        if (nchain->devnode[i].nif.generic != NULL) {
                                        return &nchain->devnode[i];
        errno = ENOENT;

static struct devfs_chain *chain_new(void)
static void chain_delete(struct devfs_chain *chain)
        if (chain->next) {
                chain_delete(chain->next);
                if (chain->devnode[i].nif.generic) {
                        free(chain->devnode[i].nif.generic);
//==============================================================================
/**
 * @brief Function create new chain if no empty nodes exist
 * ERRNO: ENOSPC
 *
 * @param devfs         file system object
 *
 * @retval 1            new chain created
 * @retval 0            number of nodes is enough
 * @retval -1           error occurred
 */
//==============================================================================
static int create_new_chain_if_necessary(struct devfs *devfs)
{
        if (devfs->number_of_chains * CHAIN_NUMBER_OF_NODES == devfs->number_of_used_nodes) {
                struct devfs_chain *chain = devfs->root_chain;
                while (chain->next != NULL) {
                        chain = chain->next;
                }

                chain->next = chain_new();
                if (!chain->next) {
                        errno = ENOSPC;
                        return -1;
                } else {
                        devfs->number_of_chains++;
                        return 1;
                }
        }
        return 0;
}