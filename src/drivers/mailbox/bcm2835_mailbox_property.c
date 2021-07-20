/**
 * @file
 * @brief Mailbox Property interface for Raspberry Pi
 *
 * @date 2021.07.05
 * @author kpishere (ref: https://www.valvers.com/open-software/raspberry-pi/bare-metal-programming-in-c-part-5/)
 */

#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include <drivers/mailbox/bcm2835_mailbox.h>
#include "bcm2835_mailbox_property.h"

#define PROPERTY_BUFFER 8192

static int pt[PROPERTY_BUFFER] __attribute__((aligned(16)));
static int pt_index = 0;

void bcm2835_property_init( void )
{
    /* Fill in the size on-the-fly */
    pt[PT_OSIZE] = 12;

    /* Process request (All other values are reserved!) */
    pt[PT_OREQUEST_OR_RESPONSE] = 0;

    /* First available data slot */
    pt_index = 2;

    /* NULL tag to terminate tag list */
    pt[pt_index] = 0;
}

/**
    @brief Add a property tag to the current tag list. Data can be included. All data is uint32_t
    @param tag
*/
void bcm2835_property_add_tag( bcm2835_mailbox_tag_t tag, ... )
{
    va_list vl;
    va_start( vl, tag );

    pt[pt_index++] = tag;

    switch( tag )
    {
        case TAG_GET_FIRMWARE_VERSION:
        case TAG_GET_BOARD_MODEL:
        case TAG_GET_BOARD_REVISION:
        case TAG_GET_BOARD_MAC_ADDRESS:
        case TAG_GET_BOARD_SERIAL:
        case TAG_GET_ARM_MEMORY:
        case TAG_GET_VC_MEMORY:
        case TAG_GET_DMA_CHANNELS:
            /* Provide an 8-byte buffer for the response */
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */
            pt_index += 2;
            break;

        case TAG_GET_CLOCKS:
        case TAG_GET_COMMAND_LINE:
            /* Provide a 256-byte buffer */
            pt[pt_index++] = 256;
            pt[pt_index++] = 0; /* Request */
            pt_index += 256 >> 2;
            break;

        case TAG_GET_EDID_BLOCK:
            pt[pt_index++] = 136;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int );
            pt_index += (136 - sizeof(uint32_t));
            break;
            
        case TAG_ALLOCATE_BUFFER:
        case TAG_GET_MAX_CLOCK_RATE:
        case TAG_GET_MIN_CLOCK_RATE:
        case TAG_GET_CLOCK_RATE:
        case TAG_GET_CLOCK_STATE:
        case TAG_GET_POWER_STATE:
        case TAG_GET_TIMING:
        case TAG_GET_TURBO:
        case TAG_GET_VOLTAGE:
        case TAG_GET_MAX_VOLTAGE:
        case TAG_GET_MIN_VOLTAGE:
        case TAG_GET_TEMPERATURE:
        case TAG_GET_MAX_TEMPERATURE:
        case TAG_LOCK_MEMORY:
        case TAG_UNLOCK_MEMORY:
        case TAG_RELEASE_MEMORY:
        case TAG_GET_DISPMANX_MEM_HANDLE:
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int );
            pt[pt_index++] = 0;
            break;

        case TAG_SET_CLOCK_RATE:
        case TAG_ALLOCATE_MEMORY:
            pt[pt_index++] = 12;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int ); /* Clock ID / size */
            pt[pt_index++] = va_arg( vl, int ); /* Rate (in Hz) / alignment */
            pt[pt_index++] = va_arg( vl, int ); /* Skip turbo setting if == 1 / flags */
            break;

        case TAG_EXECUTE_CODE:
            pt[pt_index++] = 28;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int ); /* function pointer */
            pt[pt_index++] = va_arg( vl, int ); /* r0 */
            pt[pt_index++] = va_arg( vl, int ); /* r1 */
            pt[pt_index++] = va_arg( vl, int ); /* r2 */
            pt[pt_index++] = va_arg( vl, int ); /* r3 */
            pt[pt_index++] = va_arg( vl, int ); /* r4 */
            pt[pt_index++] = va_arg( vl, int ); /* r5 */
            break;

        case TAG_SET_CURSOR_INFO:
            pt[pt_index++] = 24;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int ); /* width */
            pt[pt_index++] = va_arg( vl, int ); /* height */
            pt[pt_index++] = va_arg( vl, int ); /* (unused) */
            pt[pt_index++] = va_arg( vl, int ); /* pinter to pixels */
            pt[pt_index++] = va_arg( vl, int ); /* hotspot X */
            pt[pt_index++] = va_arg( vl, int ); /* hotspot Y */
            break;     
    
        case TAG_SET_CURSOR_STATE:
            pt[pt_index++] = 16;
            pt[pt_index++] = 0; /* Request */
            pt[pt_index++] = va_arg( vl, int ); /* enable */
            pt[pt_index++] = va_arg( vl, int ); /* x */
            pt[pt_index++] = va_arg( vl, int ); /* y */
            pt[pt_index++] = va_arg( vl, int ); /* flags:  0=display coords, 1=framebuffer coords */
            break;

        case TAG_GET_PHYSICAL_SIZE:
        case TAG_SET_PHYSICAL_SIZE:
        case TAG_TEST_PHYSICAL_SIZE:
        case TAG_GET_VIRTUAL_SIZE:
        case TAG_SET_VIRTUAL_SIZE:
        case TAG_TEST_VIRTUAL_SIZE:
        case TAG_GET_VIRTUAL_OFFSET:
        case TAG_SET_VIRTUAL_OFFSET:
        case TAG_SET_POWER_STATE:
        case TAG_SET_CLOCK_STATE:
        case TAG_SET_TURBO:
        case TAG_SET_VOLTAGE:
            pt[pt_index++] = 8;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_PHYSICAL_SIZE ) ||
                ( tag == TAG_SET_VIRTUAL_SIZE ) ||
                ( tag == TAG_SET_VIRTUAL_OFFSET ) ||
                ( tag == TAG_TEST_PHYSICAL_SIZE ) ||
                ( tag == TAG_TEST_VIRTUAL_SIZE ) ||
                ( tag == TAG_SET_POWER_STATE ) ||
                ( tag == TAG_SET_CLOCK_STATE ) ||
                ( tag == TAG_SET_TURBO ) ||
                ( tag == TAG_SET_VOLTAGE ) )                
            {
                pt[pt_index++] = va_arg( vl, int ); /* Width / device id */
                pt[pt_index++] = va_arg( vl, int ); /* Height / on/off / level / value */
            }
            else
            {
                pt_index += 2;
            }
            break;

        case TAG_GET_ALPHA_MODE:
        case TAG_SET_ALPHA_MODE:
        case TAG_GET_DEPTH:
        case TAG_SET_DEPTH:
        case TAG_GET_PIXEL_ORDER:
        case TAG_SET_PIXEL_ORDER:
        case TAG_GET_PITCH:
        case TAG_BLANK_SCREEN:
            pt[pt_index++] = 4;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_DEPTH ) ||
                ( tag == TAG_SET_PIXEL_ORDER ) ||
                ( tag == TAG_SET_ALPHA_MODE ) ||
                ( tag == TAG_BLANK_SCREEN ) )
            {
                /* Colour Depth, bits-per-pixel \ Pixel Order State / on/off */
                pt[pt_index++] = va_arg( vl, int );
            }
            else
            {
                pt_index += 1;
            }
            break;

        case TAG_GET_OVERSCAN:
        case TAG_SET_OVERSCAN:
            pt[pt_index++] = 16;
            pt[pt_index++] = 0; /* Request */

            if( ( tag == TAG_SET_OVERSCAN ) )
            {
                pt[pt_index++] = va_arg( vl, int ); /* Top pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Bottom pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Left pixels */
                pt[pt_index++] = va_arg( vl, int ); /* Right pixels */
            }
            else
            {
                pt_index += 4;
            }
            break;

        case TAG_RELEASE_BUFFER:
            pt[pt_index++] = 0; /* no arguments */
            pt[pt_index++] = 0; /* Request */
            break;

        default:
            /* Unsupported tags, just remove the tag from the list */
            pt_index--;
            break;
    }

    /* Make sure the tags are 0 terminated to end the list and update the buffer size */
    pt[pt_index] = 0;

    va_end( vl );
}

int bcm2835_property_process( void )
{
    int result;

    /* Fill in the size of the buffer */
    pt[PT_OSIZE] = ( pt_index + 1 ) << 2;
    pt[PT_OREQUEST_OR_RESPONSE] = 0;

    bcm2835_mailbox_write((unsigned int)pt, BCM2835_TAGS_ARM_TO_VC);
    
    result = bcm2835_mailbox_read( BCM2835_TAGS_ARM_TO_VC );

    return result;
}

bcm2835_mailbox_property_t* bcm2835_property_get( bcm2835_mailbox_tag_t tag )
{
    static bcm2835_mailbox_property_t property;
    int* tag_buffer = 0;

    property.tag = tag;

    /* Get the tag from the buffer. Start at the first tag position  */
    int index = 2;

    while( index < ( pt[PT_OSIZE] >> 2 ) )
    {
        /* printf( "Test Tag: [%d] %8.8X\r\n", index, pt[index] ); */
        if( pt[index] == tag )
        {
            tag_buffer = &pt[index];
            break;
        }

        /* Progress to the next tag if we haven't yet discovered the tag */
        index += ( pt[index + 1] >> 2 ) + 3;
    }

    /* Return NULL of the property tag cannot be found in the buffer */
    if( tag_buffer == 0 )
        return 0;

    /* Return the required data */
    property.byte_length = tag_buffer[T_ORESPONSE] & 0xFFFF;
    memcpy( property.data.buffer_8, &tag_buffer[T_OVALUE], property.byte_length );

    return &property;
}

//
// A null terminated list of properties to retrieve
// 
void bcm2835_property_value32(bcm2835_mailbox_tag_t *props, uint32_t *value32) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    for(int i = 0; props[i] != 0x0; i++) {
        bcm2835_property_add_tag( props[i] );

    }
    bcm2835_property_process();
    for(int i = 0; props[i] != 0x0; i++) {
        resp = bcm2835_property_get( props[i] );
        value32[i] = resp->data.value_32;
    }
}
