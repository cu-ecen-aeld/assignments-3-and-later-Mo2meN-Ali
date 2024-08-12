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

MODULE_AUTHOR("Your Name Here"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_circular_buffer buffer_dev; // A circular buffer
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
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    /** \c Nothing to do here */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buff, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    static bool is_new_write = true;  // A new write or append?
    struct aesd_buffer_entry *tmp_read_user_input_entry = NULL;

    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    if (mutex_lock_interruptible(&aesd_device.lock)) 
        return -ERESTARTSYS;   // Failed to acuire the lock, restart the system call!

    /* 
    * Allocate and cope user buffer in
    */
    tmp_read_user_input_entry = kmalloc(sizeof(struct aesd_buffer_entry), GFP_KERNEL);
    if (NULL == tmp_read_user_input_entry) {
        PDEBUG("aesd_write(): could not allocate tmp_read_user_input_entry");
        goto out;
    }
    tmp_read_user_input_entry->buffptr = kmalloc(sizeof(char) * count, GFP_KERNEL);
    if (NULL == tmp_read_user_input_entry->buffptr) {
        PDEBUG("aesd_write(): could not allocate tmp_read_user_input_entry->buffptr");
        goto out;     
    }
    if (copy_from_user((void *)tmp_read_user_input_entry->buffptr, buff, count)) {
        retval = -EFAULT;
        goto out;
    }
    tmp_read_user_input_entry->size = count;  // Set entry size
    PDEBUG("\n***Received from the user: %s\n", tmp_read_user_input_entry->buffptr);
    PDEBUG("Number of characters: %ld\n", tmp_read_user_input_entry->size);
    PDEBUG("Last character from the user buffer is: %d, %c\n", 
            tmp_read_user_input_entry->buffptr[count - 1], 
            tmp_read_user_input_entry->buffptr[count - 1]);
    /**
     * Check full and partial writes and call the appropriate aesd function interface
     */
    if (true == is_new_write) {
        PDEBUG("A new write buffer");
        if ('\n' == tmp_read_user_input_entry->buffptr[count - 1]) { // Check new line received or not?
            PDEBUG("Full write command\n");
            aesd_circular_buffer_add_entry(aesd_device.pbuffer, tmp_read_user_input_entry);
        } else {
            PDEBUG("A partial write buffer\n");
            is_new_write = false;
            aesd_circular_buffer_add_entry(aesd_device.pbuffer, tmp_read_user_input_entry);
        }
    } else { // Parital write
        PDEBUG("Continue a partial write\n");
        is_new_write = false;
        aesd_circular_buffer_append_entry(aesd_device.pbuffer, tmp_read_user_input_entry);
        if ('\n' == tmp_read_user_input_entry->buffptr[count - 1]) { // Final part of the write?
            aesd_circular_buffer_increment_buffer(aesd_device.pbuffer);
            is_new_write = true; // prepare for a new write
            PDEBUG("End of a partial write!\n");
        }
    }
    retval = count; // Return the number of bytes written in case of sucess

out:
    kfree(tmp_read_user_input_entry);
    mutex_unlock(&aesd_device.lock);
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
        return -ENOMEM;
    }
    aesd_circular_buffer_init(aesd_device.pbuffer);
    mutex_init(&aesd_device.lock);

    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
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
            kfree(buffer_dev.entry[i].buffptr); // free it then
        }
    }
    kfree(aesd_device.pbuffer); // Free the struct
    unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
