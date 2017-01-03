#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* put_user */
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Evgeniy Prudnikov" );
MODULE_DESCRIPTION( "My nice module" );
MODULE_SUPPORTED_DEVICE( "nulll" ); /* /dev/nulll */

#define SUCCESS         0
#define DEVICE_NAME     "nulll"

//static wait_queue_head_t read_queue;
static struct mutex lock;
static unsigned long bytes_counter = 0;
static long capacity = 8192;
module_param(capacity, ulong, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(capacity, "Device capacity");


static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open");
    return SUCCESS;
}

static int device_release( struct inode *inode, struct file *file ) {
    printk(KERN_DEBUG "close");
	return SUCCESS;
}

static ssize_t device_write( struct file *file, const char __user * in, size_t size, loff_t * off ) {

    if (mutex_lock_interruptible(&lock)) {
        printk(KERN_DEBUG "ne ok");
        return -ERESTARTSYS;
		goto out;
	}

    bytes_counter += size;

    if ( bytes_counter > capacity) {
        bytes_counter = capacity;
		return -ENOSPC;
		goto out;
	}

    mutex_unlock(&lock);

 out:
	return size;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long *ioctl_param)
{
    if (ioctl_num == BLKGETSIZE64) {
        *ioctl_param = bytes_counter;
    }
   	return SUCCESS;
}


static struct file_operations nulll_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
    .write = device_write,
    .open = device_open,
    .release = device_release
 };

static struct miscdevice nulll_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &nulll_fops
};


static int __init nulll_init( void ) {

    if (!capacity) {
        printk(KERN_ALERT "error: Wrong capacity");
        return -1;
    }

    misc_register(&nulll_misc_device);
    printk(KERN_ALERT "nulll device has been registered, capacity is %lu bytes\n", capacity);
    bytes_counter = 0;

    //init_waitqueue_head(&read_queue);
    mutex_init(&lock);
    return SUCCESS;
}

static void __exit nulll_exit( void ) {
    misc_deregister(&nulll_misc_device);
    mutex_destroy(&lock);
    printk( KERN_ALERT "nulll module is unloaded!\n" );
}


module_init( nulll_init );
module_exit( nulll_exit );
