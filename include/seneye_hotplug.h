#ifndef SENEYE_HOTPLUG_H
#define SENEYE_HOTPLUG_H

#include <libusb-1.0/libusb.h>

int start_hotplug();
int hotplug_callback(struct libusb_context *ctx, struct libusb_device *dev,
                     libusb_hotplug_event event, void *user_data);
int stop_hotplug();
                     
#endif
