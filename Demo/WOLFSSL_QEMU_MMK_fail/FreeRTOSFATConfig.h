
#ifndef FREERTOS_FAT_CONFIG_H
#define FREERTOS_FAT_CONFIG_H

#include <stdint.h>

#define ffconfigCWD_THREAD_LOCAL_INDEX              1

#define ffconfigBYTE_ORDER                          pdFREERTOS_LITTLE_ENDIAN

#define ffconfigMAX_PARTITIONS                      2

#define ffconfigMAX_FILENAME                        32

#define ffconfigFPRINTF_SUPPORT                     1

#define ffconfigPROTECT_FF_FOPEN_WITH_SEMAPHORE     0

#ifndef STDIN_FILENO
    #define STDIN_FILENO   0
#endif
#ifndef STDOUT_FILENO
    #define STDOUT_FILENO   1
#endif
#ifndef STDERR_FILENO
    #define STDERR_FILENO   2
#endif

#endif /* FREERTOS_FAT_CONFIG_H */
