////////////////////////////////////////////////
//
// The NetInterface is a bridge between Virtio-net(Ethernet layer) and FreeRTOS-TCP(TCP/IP layer).
//
////////////////////////////////////////////////

#ifndef _NET_INTERFACE_H_
#define _NET_INTERFACE_H_

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "FreeRTOS.h"
#include "task.h"
#include "virtio.h"
#include "virtio-net.h"

//parameters.

static uint8_t ip_arr[4] = {192,168,31,15};
// Qemu auto assign 192.168.31.15 in my environment.
// If in the future the assigned IP changed, find it in the first received ARP package from the host.
// (Ref: FreeRTOS_ARP.c:153 in eARPProcessPacket(), read pxARPHeader->ulTargetProtocolAddress)

static uint8_t netmask_arr[4] = {255,255,255,0};
static uint8_t gateway_arr[4] = {192,168,31,1};

//the virtio net MMIO address
#define virtio_net_address  0xa003e00

//Initialize virtio-net
//Initialize FreeRTOS-TCP
//Initialize the Network Interface of FreeRTOS-TCP
//Create Interrupt listening task for virtio-net
BaseType_t NetInterfaceInitialize(void);


#endif