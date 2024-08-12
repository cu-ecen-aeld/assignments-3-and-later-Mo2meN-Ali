/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#include <stdio.h>  
#endif

#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(
            struct aesd_circular_buffer *buffer,
            size_t char_offset, 
            size_t *entry_offset_byte_rtn)
{
    /**
    * TODO: implement per description
    */
    int buffer_size        = 0;
    int buffer_counter     = buffer->out_offs;
    int total_char_size    = 0;
    int i                  = 0;
    int offset             = 0;
    *entry_offset_byte_rtn = 0;
    
    if (NULL == buffer) {
        return NULL;
    }

    buffer_size = (true == buffer->full) ? 
        AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED + buffer->out_offs : buffer->in_offs;
    /*
        We need to read n of element from the circular buffer,
        but we want to read the elements into the right order that is why
        we need two counters: one for the number of elements and another for the right indices
    */
    while (buffer_counter < buffer_size) {
        total_char_size += 
            buffer->entry[buffer_counter % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED].size;
        ++buffer_counter;
        if (total_char_size >= char_offset) {
            i      = buffer->out_offs; 
            offset = char_offset;
            /*
                We need to read n of element from the circular buffer,
                but we want to read the elements into the right order that is why
                we need two counters: one for the number of elements and another for the right indices
            */
            while (i < buffer_counter) {
                if (offset < 
                    buffer->entry[i % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED].size) {
                    *entry_offset_byte_rtn = offset;
                    return &buffer->entry[i % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
                }
                offset = 
                    offset - buffer->entry[i % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED].size;
                ++i;
            }
            
        }
    }
    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, 
                                    const struct aesd_buffer_entry *add_entry)
{
    /**
    * TODO: implement per description
    */
    buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;
    buffer->entry[buffer->in_offs].size    = add_entry->size;    
    if (AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == (buffer->in_offs + 1)) {  // The next entry would be out-of-bounds?
        buffer->full     = true; // buffer is full
        buffer->in_offs  = 0;    // rewrite the oldest entry next time 
    } else {
        ++buffer->in_offs;
    }
    if (true == buffer->full) {
        buffer->out_offs = buffer->in_offs;
    }
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
