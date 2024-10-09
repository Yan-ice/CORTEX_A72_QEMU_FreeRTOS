#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOSIPConfig.h"

// 创建和初始化网络接口
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent) {
    switch (eNetworkEvent) {
        case eNetworkUp:
            // 网络接口已经启动并获取了 IP 地址
            break;
        case eNetworkDown:
            // 网络接口失效
            break;
        default:
            break;
    }
}

uint32_t ulApplicationGetNextSequenceNumber(
    uint32_t ulSourceAddress,
    uint16_t usSourcePort,
    uint32_t ulDestinationAddress,
    uint16_t usDestinationPort
)
{
    uint32_t ulSequenceNumber;

    // 简单的组合源 IP、目的 IP、端口号和系统时间来生成序列号
    ulSequenceNumber = ulSourceAddress ^ ulDestinationAddress;
    ulSequenceNumber ^= (usSourcePort << 16) | usDestinationPort;
    ulSequenceNumber ^= xTaskGetTickCount();

    return ulSequenceNumber;
}

void vTCPServerTask(void *pvParameters) {
    Socket_t xListeningSocket, xClientSocket;
    struct freertos_sockaddr xClientAddress;
    struct freertos_sockaddr xBindAddress;
    BaseType_t xReceivedBytes;

    // 创建监听套接字
    xListeningSocket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP);
    if (xListeningSocket != FREERTOS_INVALID_SOCKET) {
        // 绑定套接字到指定端口
        xBindAddress.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr("0.0.0.0");
        xBindAddress.sin_port = FreeRTOS_htons(80);
        FreeRTOS_bind(xListeningSocket, &xBindAddress, sizeof(xBindAddress));
        
        // 监听连接
        FreeRTOS_listen(xListeningSocket, 5);

        while (1) {
            // 接受客户端连接
            xClientSocket = FreeRTOS_accept(xListeningSocket, &xClientAddress, NULL);
            if (xClientSocket != FREERTOS_INVALID_SOCKET) {
                char buffer[100];
                // 接收数据
                xReceivedBytes = FreeRTOS_recv(xClientSocket, buffer, sizeof(buffer), 0);
                if (xReceivedBytes > 0) {
                    // 处理数据
                    buffer[xReceivedBytes] = '\0'; // 添加字符串终止符
                    printf("Received data: %s\n", buffer);
                }
                // 关闭连接
                FreeRTOS_shutdown(xClientSocket, 0);
            }
        }
    }
}


NetworkInterface_t xZynqDescriptor = {
        .pcName					= "virtio-net",
        .pvArgument				= ( void * )1,
        .pfInitialise           = VirtIONetInit,
        .pfOutput               = xZynqGEMOutput,
        .pfGetPhyLinkStatus     = xZynqGEMGetPhyLinkStatus,
};

struct virtio_device virtio_net0_dev = {
    .mmio_base          = (uint32_t*) VIRTIO_NET_ADDR,
	.features           = 0
};

struct virtio_net * net0;

BaseType_t VirtIONetInit(BaseType_t p) {
    net0 = virtionet_open(&virtio_net0_dev);
    return pdTRUE;
}
BaseType_t VirtIONetOutput(NetworkInterfact_t* interface, 
        NetworkBufferDescriptor_t* pxNetworkBuffer, BaseType_t xReleaseAfterSend) {
    size_t len = pxNetworkBuffer->xDataLength;
    char* buffer = 
    return pdTRUE;
}

int main_network(void) {
    printf("network");
    FreeRTOS_AddNetworkInterface( NetworkInterface_t * pxInterface )
    // 启动 FreeRTOS+TCP 协议栈
    FreeRTOS_IPInit(
        /* IP 地址 */
        "192.168.0.100",
        /* 子网掩码 */
        "255.255.255.0",
        /* 默认网关 */
        "192.168.0.1",
        /* DNS 服务器 */
        "8.8.8.8",
        /* 网络接口驱动程序 */
        NULL
    );
    printf("network init finish");

    xTaskCreate( vTCPServerTask, "tcps", configMINIMAL_STACK_SIZE * 5U, NULL,
                     tskIDLE_PRIORITY + 1, NULL );
    
    // 初始化 FreeRTOS
    printf("task start");
    vTaskStartScheduler();

}

