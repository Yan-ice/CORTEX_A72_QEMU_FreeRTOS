#include "FreeRTOS_POSIX/fcntl.h"
#include "ff_headers.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP.h"

// 定义 RAMDisk 的参数
#define RAMDISK_SECTOR_SIZE     512    // 每个扇区的大小（字节）
#define RAMDISK_NUM_SECTORS     2048   // 扇区数量
#define RAMDISK_NAME            "/ram" // RAM 磁盘挂载点名称

extern void printf(char* x, ...);

typedef struct _IO_interface {
    size_t (*vwrite)(void*, const void*, size_t);
    size_t (*vread)(void*, const void*, size_t);
    size_t (*vclose)(void*);
    
} IO_interface;

typedef struct _fd_node {
    int fd;
    void *stream_obj;
    IO_interface* interface;
    struct _fd_node* next;
} FdNode;

static size_t Socket_read(void *obj, const void *buf, size_t nbytes) {
    return FreeRTOS_recv((Socket_t)obj, buf, nbytes, 0);
}
static size_t Socket_write(void *obj, const void *buf, size_t nbytes) {
    return FreeRTOS_send((Socket_t)obj, buf, nbytes, 0);
}
static size_t Socket_close(void *obj) {
    return FreeRTOS_closesocket((Socket_t)obj);
}

static size_t File_read(void *obj, const void *buf, size_t nbytes) {
    return ff_fread(buf,nbytes,1,(FF_FILE*)obj);
}
static size_t File_write(void *obj, const void *buf, size_t nbytes) {
    return ff_fwrite(buf,nbytes,1,(FF_FILE*)obj);
}
static size_t File_close(void *obj) {
    return ff_fclose((FF_FILE*)obj);
}

static size_t StdIO_read(void *obj, const void *buf, size_t nbytes) {
    return 0;
}
static size_t StdIO_write(void *obj, const void *buf, size_t nbytes) {
    char* cbuf = buf;
    while(nbytes > 0){
        _std_putchar(*cbuf);
        cbuf++;
    }
    return 0;
}

IO_interface socket_interface = {
    .vread = Socket_read,
    .vwrite = Socket_write,
    .vclose = Socket_close
};
IO_interface file_interface = {
    .vread = File_read,
    .vwrite = File_write,
    .vclose = File_close
};
IO_interface stdio_interface = {
    .vread = StdIO_read,
    .vwrite = StdIO_write,
    .vclose = NULL
};
static int fd_seq = 0;
static int fd_alloc(){
    fd_seq++;
    return fd_seq;
}

void fs_init(void) {
    
    // 定义指向 RAM 磁盘的指针
    FF_Disk_t *pxDisk;

    // 创建 RAM 磁盘
    pxDisk = FF_RAMDiskInit(
        RAMDISK_NAME,               // 挂载点名称
        RAMDISK_NUM_SECTORS,        // 扇区数量
        RAMDISK_SECTOR_SIZE,        // 每扇区字节数
        NULL );                     // 用于跟踪错误的指针（可以为 NULL）

    if (pxDisk != NULL)
    {
        // 初始化成功，磁盘已创建并挂载
        printf("RAMDisk created and mounted at %s\n", RAMDISK_NAME);
    }
    else
    {
        // 初始化失败
        printf("Failed to create RAMDisk\n");
    }
} 

static FdNode node0;

int append_fd(void* fd_instance, IO_interface* interface){
    FdNode* new_node = (FdNode*)pvPortMalloc(sizeof(FdNode));
    new_node->fd = fd_alloc();
    new_node->stream_obj = fd_instance;
    new_node->next = node0.next;
    new_node->interface = interface;
    node0.next = new_node;
}

void free_fd(int fd) {
    FdNode* last_node;
    FdNode* current_node;
    while(current_node->fd != fd) {
        if(current_node == NULL) return;
        last_node = current_node;
        current_node = current_node->next;
    }
    last_node->next = current_node->next;
    vPortFree(current_node); 
}

FdNode* find_fd(int fd) {
    FdNode* current_node;
    while(current_node->fd != fd) {
        if(current_node == NULL) return NULL;
        current_node = current_node->next;
    }
    return current_node->stream_obj;
}

int std_init(void) {
    return append_fd(NULL, &stdio_interface);
}

int open (const char *fname, int flag) {
    FF_FILE* file =ff_fopen(fname,"rw");
    if(file != NULL) {
        return append_fd(file, &file_interface);
    }
    return -1;
}

int socket(int domain, int type, int protocol) {
    Socket_t sockfd = FreeRTOS_socket(domain, type, protocol);
    if(sockfd != NULL) {
        return append_fd(sockfd, &socket_interface);
    }
    return -1;
}

int write(int fd, const void *buf, size_t nbytes) {
    FdNode* nd = find_fd(fd);
    if(nd != NULL) {
        return (nd->interface->vwrite)(nd->stream_obj, buf, nbytes);
    }
    return -1;
}

int read(int fd, void *buf, size_t nbytes) {
    FdNode* nd = find_fd(fd);
    if(nd != NULL) {
        return (nd->interface->vread)(nd->stream_obj, buf, nbytes);
    }
    return -1;
}

int close(int fd) {
    FdNode* nd = find_fd(fd);
    if(nd != NULL) {
        int ret = (nd->interface->vclose)(nd->stream_obj);
        free_fd(fd);
        return ret;
    }
    return -1;
}