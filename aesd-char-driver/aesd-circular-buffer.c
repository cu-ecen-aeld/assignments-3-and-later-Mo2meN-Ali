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
    unsigned int entry_size      = 0;
    unsigned int entry_counter   = buffer->out_offs;
    unsigned int total_char_size = 0;
    unsigned int prev_char_size  = 0;
    *entry_offset_byte_rtn       = -1;
    
    if (NULL == buffer) {
        return NULL;
    }
    entry_size = (true == buffer->full) ? 
        AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED + buffer->in_offs : buffer->in_offs;
    /*
        We need to read n of element from the circular buffer,
        but we want to read the elements into the right order that is why
        we need two counters: one for the number of elements and another for the right indices
    */
    while (entry_counter < entry_size) {
        prev_char_size   = total_char_size;
        total_char_size += 
            buffer->entry[entry_counter % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED].size;
        if (char_offset < total_char_size) {   // char lies into this element?
            *entry_offset_byte_rtn = char_offset - prev_char_size;        
            return &buffer->entry[entry_counter % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED];
        }
        ++entry_counter;
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
    if (buffer->full) 
        buffer->out_offs = (buffer->out_offs + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    if (AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED == (buffer->in_offs + 1)) {  // The next entry would be out-of-bounds?
        buffer->full     = true; // buffer is full
        buffer->in_offs  = 0;    // rewrite the oldest entry next time 
    } else { 
        ++buffer->in_offs;
    }
}

/**
 * Returns a pointer to the element being written next
 * This function is being used to help free older buffptr entries 
 *      when the circular buffer is full before assigning them to new pointers
 */
char *aesd_circular_buffer_ref_buff(struct aesd_circular_buffer *buffer)
{
    return (char *)buffer->entry[buffer->in_offs].buffptr;
}

/**
 * Returns a pointer to the entry containing the offset @param char_offset
 * Reads an entry from the buffer @param buffer starting from the offset @param char_offset
 * Any necessary locking must be handled by the caller
 * Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
 */
struct aesd_buffer_entry *aesd_circular_buffer_read_entry(struct aesd_circular_buffer *buffer, 
                                                          size_t offset_byte)
{
    size_t entry_offset_byte = -1;  // Set to an invalid value
    // struct aesd_buffer_entry *pbuffer_entry = NULL;

    return aesd_circular_buffer_find_entry_offset_for_fpos(
                        buffer, offset_byte, &entry_offset_byte);
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}
