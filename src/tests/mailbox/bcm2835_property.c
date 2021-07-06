/**
 * @file
 * @brief
 *
 * @author  kpishere
 * @date    2021.06.06
 */

#include <cmd/cmdline.h>
#include <embox/test.h>
#include <kernel/printk.h>

#include <drivers/mailbox/bcm2835_mailbox_property.h>

EMBOX_TEST_SUITE("driver/mailbox/bcm2835_mailbox_property test");

TEST_CASE("Subset of Mailbox Property interface calls") {
    bcm2835_mailbox_property_t *resp;
    uint32_t tags[] = {TAG_GET_FIRMWARE_VERSION, TAG_GET_BOARD_MODEL, TAG_GET_BOARD_REVISION \
        , TAG_GET_BOARD_MAC_ADDRESS, TAG_GET_BOARD_SERIAL, TAG_GET_ARM_MEMORY, TAG_GET_VC_MEMORY \
        , TAG_GET_DMA_CHANNELS};

    printk("\n");

    bcm2835_property_init();
    for(int i = 0 ; i < sizeof(tags)/sizeof(uint32_t) ; i++ ) {
        bcm2835_property_add_tag(tags[i]);
    }
    bcm2835_property_process();
    for(int i = 0 ; i < sizeof(tags)/sizeof(uint32_t) ; i++ ) {
        resp = bcm2835_property_get( tags[i] );
        printk("%d- %x: %x %x\n", i, tags[i], resp->data.buffer_32[0],resp->data.buffer_32[1]);
    }

    // TAG_GET_CLOCKS
    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_CLOCKS );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_CLOCKS );
    printk("Clocks available: %d\n", resp->byte_length / ( 2 * sizeof(uint32_t) ) );
    for(int i = 0 ; i < resp->byte_length/sizeof(uint32_t) ; i+=2 ) {
        printk("clock %d parent id x id: %x %x\n", i/2, resp->data.buffer_32[i], resp->data.buffer_32[i+1]);
    }

    // TAG_GET_COMMAND_LINE
    bcm2835_property_init();
    bcm2835_property_add_tag(TAG_GET_COMMAND_LINE);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_COMMAND_LINE );
    // Add null termination to ASCII string
    #define MAX_RESP_BUFFER 256
    resp->data.buffer_8[ ( resp->byte_length==MAX_RESP_BUFFER ? MAX_RESP_BUFFER-1 : resp->byte_length) ] = 0;
    printk("Command line: '%s'\n",resp->data.buffer_8);
    
    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_PHYSICAL_SIZE );
    bcm2835_property_add_tag( TAG_GET_DEPTH );
    bcm2835_property_process();

    resp = bcm2835_property_get( TAG_GET_PHYSICAL_SIZE );
    uint32_t ht = resp->data.buffer_32[0];
    uint32_t wd = resp->data.buffer_32[1];

    resp = bcm2835_property_get( TAG_GET_DEPTH );
    uint32_t dp = resp->data.value_32;

    // Allocate and get frame buffer
    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_ALLOCATE_BUFFER );
    bcm2835_property_add_tag( TAG_SET_PHYSICAL_SIZE, ht, wd );
    bcm2835_property_add_tag( TAG_SET_VIRTUAL_SIZE, ht, wd );
    bcm2835_property_add_tag( TAG_SET_DEPTH, dp );
    bcm2835_property_add_tag( TAG_GET_PITCH );
    bcm2835_property_add_tag( TAG_GET_PHYSICAL_SIZE );
    bcm2835_property_add_tag( TAG_GET_DEPTH );
    bcm2835_property_process();

    resp = bcm2835_property_get( TAG_GET_PITCH );
    printk("FB pitch: %d\n",resp->data.value_32);

    resp = bcm2835_property_get( TAG_GET_PHYSICAL_SIZE );
    printk("FB with x height: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    resp = bcm2835_property_get( TAG_GET_DEPTH );
    printk("FB depth: %d\n",resp->data.value_32);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_POWER_STATE,  TAG_POWER_UART0);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_POWER_STATE );
    printk("PWR USB0: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_TIMING,  TAG_POWER_UART0);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_TIMING );
    printk("USB0 PWR Wait time: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_VOLTAGE,  TAG_VOLTAGE_CORE);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_VOLTAGE );
    printk("Core voltage: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_MAX_VOLTAGE,  TAG_VOLTAGE_CORE);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_MAX_VOLTAGE );
    printk("Core max voltage: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_TEMPERATURE,  TAG_VOLTAGE_CORE);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_TEMPERATURE );
    printk("Core temp: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_GET_MAX_TEMPERATURE,  TAG_VOLTAGE_CORE);
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_GET_MAX_TEMPERATURE );
    printk("Core max temp: %d %d\n",resp->data.buffer_32[0], resp->data.buffer_32[1]);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_ALLOCATE_MEMORY,  1024*64, 32, BCM2835_MEM_FLAG_NORMAL  );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_ALLOCATE_MEMORY );
    void *handle = (void *)resp->data.value_32;
    printk("Alloc handle: %x\n",resp->data.value_32);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_LOCK_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_LOCK_MEMORY );
    printk("Locked (bus address): %x\n",resp->data.value_32);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_UNLOCK_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_UNLOCK_MEMORY );
    printk("Unlocked (handle: %x) status: %x\n",(uint32_t)handle,resp->data.value_32);

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_RELEASE_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_RELEASE_MEMORY );
    printk("Release (handle: %x) status: %x\n",(uint32_t)handle,resp->data.value_32);
}
