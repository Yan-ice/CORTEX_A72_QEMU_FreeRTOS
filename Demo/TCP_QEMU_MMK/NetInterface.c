
#include "NetInterface.h"


/*--------------------------------------------------------*/

static struct virtio_net *netd;
static struct virtio_device* vd;

/* <interface of the descriptor> Initialise the interface. */
    static BaseType_t QemuNetworkInterfaceInitialise( struct xNetworkInterface * pxDescriptor ){
        //pxDescriptor->bits.bInterfaceUp = 1;
        printf("Network Interface Initialized.\n");
        return pdPASS; //return pdPASS(=1) would let this interface up.
    }

/* <interface of the descriptor> Send out an Ethernet packet. */
    static BaseType_t QemuNetworkInterfaceOutput( struct xNetworkInterface * pxDescriptor,
            NetworkBufferDescriptor_t * const pxNetworkBuffer,
            BaseType_t xReleaseAfterSend ) {
            dprintf("Network Interface send a packet with length %d.\n",pxNetworkBuffer->xDataLength);
            virtionet_write(netd, (char*)(pxNetworkBuffer->pucEthernetBuffer),
                    pxNetworkBuffer->xDataLength);
            return pdPASS;
    }

/* <interface of the descriptor> Return true as long as the LinkStatus on the PHY is present. */
    static BaseType_t QemuGetPhyLinkStatus ( struct xNetworkInterface * pxDescriptor ) {
        //int status = 0;
        //virtio_get_status(vd,&status);
        //return status;
        printf("Network Interface checking status.\n");
        return pdPASS;
    }


    NetworkInterface_t xQemuVirtioDescriptor = {
        .pcName					= "Qemu-virtio",
        .pvArgument				= ( void * )1,
        .pfInitialise           = QemuNetworkInterfaceInitialise,
        .pfOutput               = QemuNetworkInterfaceOutput,
        .pfGetPhyLinkStatus     = QemuGetPhyLinkStatus,
    };
    NetworkEndPoint_t xEndpoint;

/*--------------------------------------------------------*/

#include <FreeRTOS_IP.h>
#include <FreeRTOS_Sockets.h>

static void prvPassEthMessages( NetworkBufferDescriptor_t * pxDescriptor )
{
    IPStackEvent_t xRxEvent;

    xRxEvent.eEventType = eNetworkRxEvent;
    xRxEvent.pvData = ( void * ) pxDescriptor;

    if( xSendEventStructToIPTask( &xRxEvent, ( TickType_t ) 1000 ) != pdPASS )
    {
        /* The buffer could not be sent to the stack so must be released again.
         * This is a deferred handler task, not a real interrupt, so it is ok to
         * use the task level function here. */
        #if ( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
        {
            do
            {
                NetworkBufferDescriptor_t * pxNext = pxDescriptor->pxNextBuffer;
                vReleaseNetworkBufferAndDescriptor( pxDescriptor );
                pxDescriptor = pxNext;
            } while( pxDescriptor != NULL );
        }
        #else
        {
            vReleaseNetworkBufferAndDescriptor( pxDescriptor );
        }
        #endif /* ipconfigUSE_LINKED_RX_MESSAGES */

        FreeRTOS_printf( ( "prvPassEthMessages: Can not queue return packet!\n" ) );
    }
}
			
static BaseType_t recvNetworkInterfaceInput(struct virtio_net *vnet){
    const static int ETHERNET_BUF_SIZ = 1000;

	char buf[ETHERNET_BUF_SIZ];
    int len = virtionet_read(vnet,buf,ETHERNET_BUF_SIZ);
	BaseType_t xAccepted = pdTRUE;

	if (len > 0) {
        if (len > 990) {
            printf("WARN: ethernet buf size is full. This may cause problems.\n");
        }
        NetworkBufferDescriptor_t * pxNewDescriptor = NULL;
		pxNewDescriptor = pxGetNetworkBufferWithDescriptor( len, pdMS_TO_TICKS(1000) );
		
        if( pxNewDescriptor == NULL )
        {
            /* A new descriptor can not be allocated now. This packet will be dropped. */
            xAccepted = pdFALSE;
			return;
        }

		pxNewDescriptor->pxInterface = FreeRTOS_FirstNetworkInterface();
		pxNewDescriptor->pxEndPoint = pxNewDescriptor->pxInterface->pxEndPoint;

        
		memcpy( ( void * ) pxNewDescriptor->pucEthernetBuffer, buf, len);
		prvPassEthMessages(pxNewDescriptor);
	}else{
		xAccepted = pdFALSE;
	}
	return xAccepted;
}

//The virtionet_handle_interrupt() in virtio-net.h is no longer used.
//We use this method instead.
void virtionet_check_interrupt(struct virtio_net *vnet)
{
	uint32_t int_status = 0;

	virtio_get_interrupt_status(&(vnet->vdev), &int_status);
	if(int_status == 1){
		recvNetworkInterfaceInput(vnet);
		virtio_interrupt_ack(&(vnet->vdev), int_status);
	}
}

    // This task check interrupt for every 10ms.
    // it is 
    static void prvVirtioINTRTask( void * pvParameters )
    {
        uint64_t xNextWakeTime = xTaskGetTickCount();
        while(1)
        {
            vTaskDelayUntil( &xNextWakeTime, pdMS_TO_TICKS(10));
            virtionet_check_interrupt(netd);
        }
        
    }
    
/*--------------------------------------------------------*/


/* Initialise the interface. */
    BaseType_t NetInterfaceInitialize(void) {
        printf("Initializing virtio dev.\n");
        vd = virtio_setup_vd((void *)virtio_net_address);
        printf("Initializing virtio net.\n");
        netd = virtionet_open(vd);

        if (netd != NULL) {
            printf("virtio initialize success\n");
        }else {
            printf("virtio initialize failed\n");
        }

        printf("Initializing FreeRTOS Network Interface based on virtio-net.\n");
        FreeRTOS_AddNetworkInterface(&xQemuVirtioDescriptor);

        FreeRTOS_FillEndPoint( &xQemuVirtioDescriptor,
                                &xEndpoint,
                                ip_arr,
                                netmask_arr,
                                gateway_arr,
                                0,
                                0);

        FreeRTOS_IPInit_Multi();
        printf("Create virtio INTR task.\n");
        xTaskCreate(prvVirtioINTRTask,"vr",0x100,NULL,6,NULL);
        printf("Initialize finished.\n");
        return pdPASS;
    }
