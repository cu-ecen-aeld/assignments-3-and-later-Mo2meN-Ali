/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("Momen"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

void aesd_buffer_print(const char* prefix, const char* buff, size_t size) 
{
    char* tmp = kmalloc(size + 1, GFP_KERNEL);
    if(tmp) {
        memcpy(tmp, buff, size);
        tmp[size] = '\0';
        PDEBUG("%s: buffer = %s, size = %ld\n", prefix, tmp, size);
        kfree(tmp);
    }
}

/** Print the content of the circular buffer @param buffer 
 * Any necessary locking must be handled by the caller
 * Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_show_circular_buffer(struct aesd_circular_buffer *buffer)
{
    int i, buffer_iterator = buffer->out_offs;
    printk(KERN_DEBUG "Circular buffer (oldest to newest):\n");

    for (i = 0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; ++i) {
        if (NULL != buffer->entry[buffer_iterator].buffptr) {
            printk(KERN_DEBUG "elem: %d, ", i);
            aesd_buffer_print("",
                buffer->entry[buffer_iterator].buffptr, 
                buffer->entry[buffer_iterator].size);                
        } else {            
           printk(KERN_DEBUG "elem: %d, Empty", i); 
        }
        buffer_iterator = 
            (buffer_iterator + 1) % AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED;
    }
}

int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev *devPtr;
    PDEBUG("open");
    /**
     * TODO: handle open
     */
    devPtr = container_of(inode->i_cdev, struct aesd_dev, cdev);
    filp->private_data = devPtr;
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    struct aesd_dev *dev = filp->private_data;
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    /** Print the circular buffer fo debugging purposes */
    aesd_show_circular_buffer(dev->pbuffer);
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buff, size_t count,
                  loff_t *f_pos)
{
    ssize_t retval = 0;
    size_t entry_offset = -1;
    struct aesd_dev *dev = filp->private_data;
    char* read_offset_buffer = NULL;
    size_t read_offset_buffer_size = 0; 
    struct aesd_buffer_entry *entry = NULL;

    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */
    if (mutex_lock_interruptible(&dev->lock))
        return -ERESTARTSYS;

    /** @note all strings are terminated with '\n' instead of '\0', 
     *      which makes printing them is a bit tricky and unreliable.
     *  for reliable output one can add the null character manually but would need to allocate a character for it. 
     * */

    // We will examine one entry per read for simplicity
    entry = aesd_circular_buffer_find_entry_offset_for_fpos(dev->pbuffer, *f_pos, &entry_offset);
    if (NULL != entry) {  // If this offset exists in the circular buffer
        read_offset_buffer_size = entry->size - entry_offset; // Calculate the new buffer size after deducting the offset
        read_offset_buffer = (char *)kmalloc(sizeof(char) * read_offset_buffer_size, GFP_KERNEL);
        if (NULL == read_offset_buffer) {
            retval = -EFAULT;
            goto out;
        }
        memcpy(read_offset_buffer, entry->buffptr + entry_offset, read_offset_buffer_size);
        aesd_buffer_print("Offset found in buffer", entry->buffptr, entry->size);
        aesd_buffer_print("String to be returned to the user",
            read_offset_buffer, read_offset_buffer_size);
        if (copy_to_user(buff, read_offset_buffer, read_offset_buffer_size)) {
            retval = -EFAULT;
            goto out;
        }
        *f_pos += read_offset_buffer_size;    // Increment the file postion by the amount of character read
        retval = (count > read_offset_buffer_size)? read_offset_buffer_size : count;
    } else {
        PDEBUG("Offset was not found resulted into null entry\n");
    }

out:
    mutex_unlock(&dev->lock);
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buff, size_t count,
                   loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    struct aesd_dev *dev = filp->private_data;
    char *tmp_user_buffer = NULL; // This buffer reads from the user space
    static struct aesd_buffer_entry full_user_entry = { NULL, 0 }; // This buffer contains a full read buffer

    PDEBUG("write %zu bytes with offset %lld", count, *f_pos);
    /**
     * TODO: handle write
     */
    if (mutex_lock_interruptible(&dev->lock)) 
        return -ERESTARTSYS;   // Failed to acuire the lock, restart the system call!    

    /**
    * Allocate and cope user buffer in tmp buffer
    * full_user_entry contains the full buffer 
    */
    tmp_user_buffer = 
        (char *)kmalloc(sizeof(char) * (count + full_user_entry.size), GFP_KERNEL);
    if (NULL == tmp_user_buffer) {
        goto out;     
    }
    // Copy the previous parts in case of partial write or NULL in case of full writes in the front
    memcpy(tmp_user_buffer, full_user_entry.buffptr, full_user_entry.size); 
    // copy the user new input right after the latest input
    if (copy_from_user(tmp_user_buffer + full_user_entry.size, 
                       buff, count)) {
        retval = -EFAULT;
        kfree(tmp_user_buffer);
        goto out;
    }
    aesd_buffer_print("\n\n***Received from the user", 
        tmp_user_buffer + full_user_entry.size, count);
    PDEBUG("Last character from the user input is: %d, %c\n", 
            tmp_user_buffer[full_user_entry.size + count - 1], 
            tmp_user_buffer[full_user_entry.size + count - 1]);

    // We need to free the old allocated memory before assigning the new one from the temp buffer
    kfree(full_user_entry.buffptr); // Free the previous buffer 
    full_user_entry.buffptr = tmp_user_buffer; // Copy the new buffer address
    full_user_entry.size   += count;

    if ('\n' == full_user_entry.buffptr[full_user_entry.size - 1]) {
        if (dev->pbuffer->full)  // If buffer is full we need to delete old buffptr before we overwrite it
            kfree(aesd_circular_buffer_ref_buff(dev->pbuffer)); // Read the old read and discard it

        aesd_buffer_print("Completed write command", 
            full_user_entry.buffptr, full_user_entry.size);
        aesd_circular_buffer_add_entry(dev->pbuffer, &full_user_entry); // Add the new item
        // Reset the full_read_entry without free(), gets freed in the cleanup function
        full_user_entry.buffptr = NULL;
        full_user_entry.size    = 0;
    }
    *f_pos = count; // We write all user bytes every call so no need to increment, just assign.
    retval = count; // Number of bytes written during this call.


out:
    mutex_unlock(&dev->lock);
    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}

int __init aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */
    aesd_device.pbuffer = kmalloc(1 * sizeof(struct aesd_circular_buffer), GFP_KERNEL);
    if (NULL == aesd_device.pbuffer) {
        result = -ENOMEM;
        goto fail;
    }
    aesd_circular_buffer_init(aesd_device.pbuffer);
    mutex_init(&aesd_device.lock);

    result = aesd_setup_cdev(&aesd_device);
    if (result) 
        goto fail;

    return 0;

fail:    
    unregister_chrdev_region(dev, 1);
    return result;
}

void __exit aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);
    unsigned char i;
    struct aesd_buffer_entry *entry = NULL;

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific portions here as necessary
     */
    // Free the allocated circular buffer
    AESD_CIRCULAR_BUFFER_FOREACH(entry, aesd_device.pbuffer, i) {
        if (NULL != entry->buffptr) {  // Is it allocated?
            kfree(entry->buffptr); // free it then
        }
    }
    kfree(aesd_device.pbuffer); // Free the struct
    unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
