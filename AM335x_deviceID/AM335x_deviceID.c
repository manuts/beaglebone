#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdlib.h>

/* SPRUH73P – October 2011 – Revised March 2017
 * table 2-2 page 180 */
#define CONTROL_MODULE_BASE             0x44E10000
#define CONTROL_MODULE_SIZE             0x1000
#define CONTROL_REG_DEVICE_ID_OFFSET    0x600

#define ERROR_MESSAGE_LEN       256

#define DEBUG(x)    // x

/* Map a memory range */
void* Osal_mmap (void *ptr, uint32_t size) {
    DEBUG(printf("Osal_mmap(): ptr=%p, size=%u\n", ptr, size));

    int32_t     mmap_fd;
    int32_t     pageSize;
    void*       mmapBaseAddress;

    /* Get the page size. */
    pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize <= 0) {
        perror("Osal_mmap(): sysconf(_SC_PAGE_SIZE) failed");
        return NULL;
    }

    /* Ensure block size and physical addresses are aligned to page size. */
    if ((size % pageSize) || ((uintptr_t)ptr % pageSize)) {
        printf("Osal_mmap(): Alignment error! (size mod page=%u, ptr mod page=%u\n", 
               (size % pageSize), (uint32_t)((uintptr_t)ptr % pageSize));
        return NULL;
    }

    /* Open the /dev/mem file - for non-cachable memroy */
    if ((mmap_fd = open("/dev/mem", (O_RDWR | O_SYNC))) < 0) {
        perror("Osal_mmap(): Could not open device /dev/mem");
        return NULL;
    }

    /* Create a mapping of the physical address. */
    mmapBaseAddress = mmap(0, size, (PROT_READ|PROT_WRITE|PROT_EXEC), MAP_SHARED|MAP_LOCKED, mmap_fd, (off_t)ptr);
    if(mmapBaseAddress == MAP_FAILED) {
        perror("Osal_mmap() mmap failure");
        close(mmap_fd);
        return NULL;
    }

    close(mmap_fd);

    DEBUG(printf("Osal_mmap(): physical addr=%p,size=%d is mapped to %p\n", ptr, size, mmapBaseAddress));

    /* Return the mapped virtual address. */
    return (mmapBaseAddress);
}

/* Unmap a memory range */
int Osal_munmap (void * ptr, uint32_t size) {
    DEBUG(printf("Osal_munmap(): ptr=%p, size=%u\n",ptr, size));

    char errMsg[ERROR_MESSAGE_LEN] = {0};
    if (munmap(ptr, size) == 0) {
        return 0;
    } else {
        sprintf(errMsg, "ERROR: munmap(%p, %u) failed", ptr, size);
        perror(errMsg);
        return errno;
    }
}

void AM335x_dumpDeviceID(uint32_t deviceID) {
    uint8_t devRev;
    printf("Device_ID:          0x%0x\n", deviceID);

    devRev = ((deviceID & 0xf0000000) >> 28);
    switch(devRev)
    {
        case 0b0000:
            printf("Silicon Revision:   1.0\n");
            break;
        case 0b0001:
            printf("Silicon Revision:   2.0\n");
            break;
        case 0b0010:
            printf("Silicon Revision:   2.1\n");
            break;
        default:
            printf("Unidenfied Silicon Revision\n");
            break;
    }
}

int main(int argc, char **argv) {
    printf("Hello world!\n");
    printf("This program prints the AM335x SoC Silicon Revision on your beaglebone black\n");
    printf("****************************************************************************\n");

    void* cmVirtAddr = NULL;
    int ret = 0;
    uint32_t deviceID;

    cmVirtAddr = Osal_mmap((void*)CONTROL_MODULE_BASE, CONTROL_MODULE_SIZE);
    if(cmVirtAddr == NULL) {
        printf("Osal_mmap() failed! exiting the program\n");
        exit(-1);
    }

    deviceID = *(uint32_t*)(cmVirtAddr + CONTROL_REG_DEVICE_ID_OFFSET);

    AM335x_dumpDeviceID(deviceID);

    ret = Osal_munmap(cmVirtAddr, CONTROL_MODULE_SIZE);
    if(ret != 0){
        printf("Osal_munmap() failed! returnn value is %d\n", ret);
    }

    printf("****************************************************************************\n");
    return 0;
}
