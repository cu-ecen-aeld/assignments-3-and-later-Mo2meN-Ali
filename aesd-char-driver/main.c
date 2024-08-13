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
    //aesd_circular_buffer_show(dev->pbuffer);
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
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
        PDEBUG("Offset found in buffer: %s\n", entry->buffptr);
        PDEBUG("String to be returned to the user: %s, size: %ld\n", 
            read_offset_buffer, read_offset_buffer_size);
        *f_pos += read_offset_buffer_size;    // Increment the file postion by the amount of character read
        if (count > read_offset_buffer_size) // Partial read?
            retval = read_offset_buffer_size; // Return the amount of character read
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
    static bool is_new_write = true;  // A new write or append?
    struct aesd_buffer_entry *buffer_read_user_input_entry      = NULL; // This buffer reads from the user space
    static struct aesd_buffer_entry *full_read_user_input_entry = NULL; // This buffer contains a full read
    char *tmp_read_user_input_buff = NULL; // This buffer acts a temp buffer for partial reads

    PDEBUG("write %zu bytes with offset %lld", count, *f_pos);
    /**
     * TODO: handle write
     */
    if (mutex_lock_interruptible(&dev->lock)) 
        return -ERESTARTSYS;   // Failed to acuire the lock, restart the system call!
    /* 
    * Allocate and cope user buffer in
    */
    buffer_read_user_input_entry = (struct aesd_buffer_entry *)kmalloc(sizeof(struct aesd_buffer_entry), GFP_KERNEL);
    if (NULL == buffer_read_user_input_entry) {
        goto out;
    }
    buffer_read_user_input_entry->buffptr = (const char *)kmalloc(sizeof(char) * count, GFP_KERNEL);
    if (NULL == buffer_read_user_input_entry->buffptr) {
        goto out;     
    }
    if (copy_from_user((char *)buffer_read_user_input_entry->buffptr, buff, count)) {
        retval = -EFAULT;
        goto out;
    }
    buffer_read_user_input_entry->size = count;  // Set entry size

    PDEBUG("\n\n***Received from the user: %s\n", buffer_read_user_input_entry->buffptr);
    PDEBUG("Number of characters: %ld\n", buffer_read_user_input_entry->size);
    PDEBUG("Last character from the user buffer is: %d, %c\n", 
            buffer_read_user_input_entry->buffptr[count - 1], 
            buffer_read_user_input_entry->buffptr[count - 1]);
    /**
     * Check full and partial writes and call the appropriate aesd function interface
     * Addresses that are eventually going into the circular buffer are not freed here,
     *  but into the release function of the driver, only temp mallocs are being freed here.
     */
    if (true == is_new_write) {
        PDEBUG("A new write into the buffer");
        if ('\n' == buffer_read_user_input_entry->buffptr[count - 1]) { // Check new line received or not?
            PDEBUG("Full write command, buffer size is %ld, buffer is %s\n", 
                buffer_read_user_input_entry->size, buffer_read_user_input_entry->buffptr);
            // Delete older entry's buffer if full
            if (dev->pbuffer->full) {
                tmp_read_user_input_buff = aesd_circular_buffer_ref_buff(dev->pbuffer);
                kfree(tmp_read_user_input_buff);
            }
            aesd_circular_buffer_add_entry(aesd_device.pbuffer, buffer_read_user_input_entry);
        } else {
            PDEBUG("A partial write command\n");
            full_read_user_input_entry = (struct aesd_buffer_entry *)kmalloc(sizeof(struct aesd_buffer_entry), GFP_KERNEL);
            if (NULL == full_read_user_input_entry) {
                goto out;
            }
            full_read_user_input_entry->buffptr = (const char *)kmalloc(sizeof(char) * count, GFP_KERNEL);
            if (NULL == full_read_user_input_entry->buffptr) {
                goto out;     
            }
            memcpy((char *)full_read_user_input_entry->buffptr, 
                buffer_read_user_input_entry->buffptr, 
                buffer_read_user_input_entry->size);
            full_read_user_input_entry->size = buffer_read_user_input_entry->size;
            kfree(buffer_read_user_input_entry);
            is_new_write = false;
        }
    } else { // Parital write continue
        tmp_read_user_input_buff = (char *)full_read_user_input_entry->buffptr;
        full_read_user_input_entry->buffptr = 
            (const char *)kmalloc(sizeof(char) * (full_read_user_input_entry->size + count), GFP_KERNEL);
        if (NULL == full_read_user_input_entry->buffptr) {
            goto out;     
        }
        memcpy((char *)full_read_user_input_entry->buffptr, 
            tmp_read_user_input_buff, 
            full_read_user_input_entry->size);
        memcpy((char *)full_read_user_input_entry->buffptr + full_read_user_input_entry->size, 
            buffer_read_user_input_entry->buffptr, count);
        full_read_user_input_entry->size += count;
        kfree(tmp_read_user_input_buff); // Release the old memory used for partial reads
        PDEBUG("Continue a partial write, buffer size is %ld, buffer is %s\n", 
            full_read_user_input_entry->size, full_read_user_input_entry->buffptr);
        if ('\n' == buffer_read_user_input_entry->buffptr[count - 1]) { // Final part of the write?
            // Delete older entry's buffer if full
            if (dev->pbuffer->full) {
                tmp_read_user_input_buff = aesd_circular_buffer_ref_buff(dev->pbuffer);
                kfree(tmp_read_user_input_buff);
            }
            aesd_circular_buffer_add_entry(aesd_device.pbuffer, full_read_user_input_entry);
            is_new_write = true; // prepare for the next new write
            PDEBUG("End of a partial write!\n");
        }
    }
    retval = count; // Return the number of bytes written in case of sucess

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
    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific portions here as necessary
     */
    // Free the allocated circular buffer
    for (i = 0; i < AESDCHAR_MAX_WRITE_OPERATIONS_SUPPORTED; ++i) {
        if (NULL != aesd_device.pbuffer->entry[i].buffptr) {  // Is it allocated?
            kfree(aesd_device.pbuffer->entry[i].buffptr); // free it then
        }
    }
    kfree(aesd_device.pbuffer); // Free the struct
    unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
