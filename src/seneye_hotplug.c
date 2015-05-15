#include "seneye_hotplug.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <hidapi/hidapi.h>

static int count = 0;
static int init_done = 0;
static libusb_hotplug_callback_handle usb_handle;
static libusb_context *hotplug_context;

int start_hotplug()
{
    int rc;
    libusb_init(&hotplug_context);
    rc = hid_init();
    //return error
    rc = libusb_hotplug_register_callback(hotplug_context, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                                        LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, 0x24f7, 0x2201,
                                        LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL,
                                        &usb_handle);
    if (LIBUSB_SUCCESS != rc) {
        syslog(LOG_ERR, "Error creating a hotplug callback: %d\n", rc);
        hid_exit();
        return EXIT_FAILURE;
    }
    syslog(LOG_INFO, "starting hotplug");
    init_done = 1;
    return 0;
}

int stop_hotplug()
{
    if (init_done) {
        libusb_hotplug_deregister_callback(hotplug_context, usb_handle);
        libusb_exit(hotplug_context);
        hid_exit();
        syslog(LOG_INFO, "ending hotplug");
    }
    return 0;
}

int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data) {
    static libusb_device_handle *handle = NULL;
    struct libusb_device_descriptor desc;
    int rc;
    (void)libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
        syslog(LOG_INFO, "device connected");
    } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
        syslog(LOG_INFO, "device disconnected");
    } else {
        syslog(LOG_WARNING, "Unhandled event %d\n", event);
    }
    count++;
    return 0;
}
