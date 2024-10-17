
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "aarch64.h"

#include <wolfssl/ssl.h>

#include <FreeRTOS_IP.h>
#include <FreeRTOS_Sockets.h>

#define SERVER_PORT 2333



int main_ssl() {
    
    socklen_t addr_size;
    
    // 初始化 wolfSSL
    wolfSSL_Init();
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    
    // 创建 SSL 对象
    WOLFSSL* ssl = wolfSSL_new(ctx);

    // 创建 TCP socket
    Socket_t sockfd = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP); //TCP

    // construct server address
    struct freertos_sockaddr server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_address.ulIP_IPv4 = INADDR_ANY;

    // 绑定和监听
    FreeRTOS_bind(sockfd, &server_addr, sizeof(server_addr));
    FreeRTOS_listen(sockfd, 5);

    // 接受连接
    struct freertos_sockaddr client_addr;
    addr_size = sizeof(client_addr);
    Socket_t session_sock = FreeRTOS_accept(sockfd, &client_addr, &addr_size);

    // 将 socket 关联到 SSL

    wolfSSL_set_fd(ssl, session_sock);

    // 接收数据
    char buffer[256];
    int bytes = wolfSSL_read(ssl, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    printf("Received: %s\n", buffer);

    // 发送数据
    const char* response = "Hello from server!";
    wolfSSL_write(ssl, response, strlen(response));

    // 清理
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    FreeRTOS_closesocket(session_sock);
    FreeRTOS_closesocket(sockfd);
    wolfSSL_Cleanup();

    return 0;
}