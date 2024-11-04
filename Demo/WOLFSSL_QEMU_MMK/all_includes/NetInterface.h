#ifndef _NET_INTERFACE_H_
#define _NET_INTERFACE_H_

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

#include "FreeRTOS.h"
#include "task.h"
#include "virtio.h"
#include "virtio-net.h"

    BaseType_t VirtioNetInitialize(void);

#endif