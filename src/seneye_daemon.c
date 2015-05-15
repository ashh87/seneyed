#include "seneye_daemon.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <hidapi/hidapi.h>
#include <libusb-1.0/libusb.h>
hid_device *handle;

static void *TaskCode(void *argument)
{
    int res, i;
    //hid_device *handle;
    unsigned char buf[64];

//     res = hid_init();
//     if( res == -1 )
//     {
//         return (void*)1;
//     }
//
//     handle = hid_open(0x0911, 0x251c, NULL);
//     if( handle == NULL )
//     {
//         return (void*)2;
//     }

    printf( "while 2\n");

    while( 1 )
    {
        memset( buf, 0, 64 );
        res = hid_read(handle, buf, 64);
        if( res == -1 )
        {
            return (void*)3;
        }

        printf( "received %d bytes\n", res);

        for (i = 0; i < res; i++)
            printf("Byte %d: %02x ", i+1, buf[i]);
        //printf( "%02x ", buf[0]);
        fflush(stdout);
    }

    return (void*)0;
}


int openhid(void)
{
    int res;
    //hid_device *handle;
    unsigned char buf[65];

    res = hid_init();
    if( res == -1 )
    {
        return 1;
    }
    
	// Enumerate and print the HID devices on the system
	struct hid_device_info *devs, *cur_dev;
	
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
			cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

    handle = hid_open(0x0911, 0x251c, NULL);
    if( handle == NULL )
    {
        return 2;
    }

    hid_set_nonblocking( handle, 0 );

    pthread_t thread;
    int rc = pthread_create(&thread, NULL, TaskCode, NULL);

    printf( "while 1\n");

    while(1)
    {
        int a = getchar();
        if( a == 'a')
        {
            // Get Device Type (cmd 0x82). The first byte is the report number (0x0).
            buf[0] = 0x0;
            buf[1] = 0x82;
            res = hid_write(handle, buf, 65);
            if( res != -1 )
                printf( "write ok, transferred %d bytes\n", res );
            else
            {
                printf( "write error\n" );
                return 1;
            }
        }
        else if( a== 'b')
            break;
    }

    void* trc;
    rc = pthread_join(thread, &trc);

    printf( "rc code: %ld\n", (long)trc );

    // Finalize the hidapi library
    res = hid_exit();

    return 0;
}
