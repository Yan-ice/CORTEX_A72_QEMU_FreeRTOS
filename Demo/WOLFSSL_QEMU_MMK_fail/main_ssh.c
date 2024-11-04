
// #include <FreeRTOS.h>
// #include <task.h>
// #include <queue.h>

// #include "aarch64.h"

// #include <wolfssl/ssl.h>

// #include <FreeRTOS_IP.h>
// #include <FreeRTOS_Sockets.h>
// #include <wolfssh/ssh.h>


// #define SERVER_PORT 2333

// static const char* username = "test";
// static const char* password = "password";

// static int server_auth_callback(struct WOLFSSH* ssh, void* ctx) {
//     vSendString( "Server Auth. \n" );
//     const char* user = wolfSSH_GetUsername(ssh);
//     if (strcmp(user, username) == 0) {
//         wolfSSH_SetAuthenticated(ssh, 1);
//         return WOLFSSH_USERAUTH_SUCCESS;
//     }
//     return WOLFSSH_USERAUTH_FAILURE;
// }

// int main_ssh( void )
// {
//     vSendString( "Hello FreeRTOS SSH! \n" );


//     //FreeRTOS_IPInit();

//     WOLFSSH_CTX* ctx;
//     WOLFSSH* ssh;
//     int clientfd;
    
//     /* Construct a socket */
//     Socket_t sockfd = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP); //TCP
//     //Socket_t socket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP); //UDP

//     /* Create WOLFSSH_CTX and set options */
//     ctx = wolfSSH_CTX_new(WOLFSSH_ENDPOINT_SERVER, NULL);
//     wolfSSH_CTX_SetBanner(ctx, "MySSHServer");
//     wolfSSH_SetUserAuth(ctx, server_auth_callback);

//     /* Construct the socket address. */
//     struct freertos_sockaddr server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(SERVER_PORT);
//     server_addr.sin_address.ulIP_IPv4 = INADDR_ANY;

//     /* Bind address to socket */
//     FreeRTOS_bind(sockfd, &server_addr, sizeof(server_addr));

//     vTaskStartScheduler();
    
//     return 0;
// }
