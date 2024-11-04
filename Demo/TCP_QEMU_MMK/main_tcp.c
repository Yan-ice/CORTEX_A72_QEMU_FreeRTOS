/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "aarch64.h"
#include "virtio.h"

#include "NetInterface.h"

/*-----------------------------------------------------------*/

#define tcpechoPORT_NUMBER 2333

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

static uint16_t usUsedStackSize = 0;

/*-----------------------------------------------------------*/

static void prvConnectionInstance(void* pvParam) {
    Socket_t xConnectedSocket = (Socket_t)pvParam;
    char buffer[51];
    buffer[50] = 0;
    int recv_len;
    while(1) {
        recv_len = FreeRTOS_recv(xConnectedSocket,buffer,50,0);
        if(recv_len > 0){
            buffer[recv_len] = '\0';
            printf("echo: %s",buffer);
            FreeRTOS_send(xConnectedSocket,buffer,recv_len,0);
        }
    }

}
/*-----------------------------------------------------------*/
static void prvConnectionListeningTask( void * pvParameters )
    {
        struct freertos_sockaddr xClient, xBindAddress;
        Socket_t xListeningSocket, xConnectedSocket;
        socklen_t xSize = sizeof( xClient );
        static const TickType_t xReceiveTimeOut = portMAX_DELAY;
        const BaseType_t xBacklog = 20;
/* Just to prevent compiler warnings. */
        ( void ) pvParameters;

        /* Attempt to open the socket. */
        vSendStringISR( "Opening listener socket\n" );
        xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET, FREERTOS_SOCK_STREAM, FREERTOS_IPPROTO_TCP );
        configASSERT( xListeningSocket != FREERTOS_INVALID_SOCKET );
        vSendStringISR( "Open success\n" );
        /* Set a time out so accept() will just wait for a connection. */
        FreeRTOS_setsockopt( xListeningSocket, 0, FREERTOS_SO_RCVTIMEO, &xReceiveTimeOut, sizeof( xReceiveTimeOut ) );

        /* Bind the socket to the port that the client task will send to, then
         * listen for incoming connections. */
        xBindAddress.sin_port = tcpechoPORT_NUMBER;
        xBindAddress.sin_port = FreeRTOS_htons( xBindAddress.sin_port );
        xBindAddress.sin_family = FREERTOS_AF_INET;
        FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) );
        FreeRTOS_listen( xListeningSocket, xBacklog );

        for( ; ; )
        {
            vSendStringISR( "Start listening\n" );
            /* Wait for a client to connect. */
            xConnectedSocket = FreeRTOS_accept( xListeningSocket, &xClient, &xSize );
            configASSERT( xConnectedSocket != FREERTOS_INVALID_SOCKET );
            /* Spawn a task to handle the connection. */
            xTaskCreate( prvConnectionInstance, "EchoServer", usUsedStackSize, ( void * ) xConnectedSocket, tskIDLE_PRIORITY, NULL );
            vSendStringISR( "Connected to client\n");
        }
    }
/*-----------------------------------------------------------*/

int main_tcp(void)
{   
    uint16_t usStackSize = 2048;
    UBaseType_t uxPriority = 2;
    vSendString( "Hello FreeRTOS!\n" );

    NetInterfaceInitialize();
    vSendString( "IP init success.\n" );
    /* Create the TCP echo server. */
    xTaskCreate( prvConnectionListeningTask, "lt", usStackSize, NULL, uxPriority + 2, NULL );
    vSendString( "server task create. \n" );

    /* Remember the requested stack size so it can be re-used by the server
         * listening task when it creates tasks to handle connections. */
    usUsedStackSize = usStackSize;
    vSendString( "Start Scheduler\n" );
    vTaskStartScheduler();
}

