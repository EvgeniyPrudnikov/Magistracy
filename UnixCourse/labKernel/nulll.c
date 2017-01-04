#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/ioctl.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Evgeniy Prudnikov");
MODULE_DESCRIPTION("device /dev/null Limited Edition");
MODULE_SUPPORTED_DEVICE("nulll"); /* /dev/nulll */

#define SUCCESS         0
#define DEVICE_NAME     "nulll"

static struct mutex lock;
static unsigned long bytes_counter = 0;
static unsigned long capacity = 0;
module_param(capacity, ulong, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(capacity, " Device capacity");

static int device_open(struct inode *inode, struct file *file) {
    return SUCCESS;
}

static int device_release( struct inode *inode, struct file *file ) {
	return SUCCESS;
}

static ssize_t device_write( struct file *file, const char __user * in, size_t size, loff_t * off ) {
    if (mutex_lock_interruptible(&lock)) {
        return -ERESTARTSYS;
		goto out;
	}
    bytes_counter += size;
    if ( capacity > 0 && bytes_counter > capacity) {
        bytes_counter = capacity;
        goto out_err;
	}
    mutex_unlock(&lock);
 out:
    return size;
out_err:
    mutex_unlock(&lock);
    return -ENOSPC;

}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
    if (ioctl_num == BLKGETSIZE64) {
        copy_to_user((void __user *)ioctl_param, (const void *)&bytes_counter, sizeof(unsigned long));
        return SUCCESS;
    }
   	return -ENOTTY;
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
    int rec;
    rec = misc_register(&nulll_misc_device);
    if (rec == 0) {
        if (capacity > 0) {
            printk(KERN_INFO "nulll device has been loaded, capacity is %lu bytes\n", capacity);
        } else {
            printk(KERN_INFO "nulll device has been loaded");
        }
    } else {
        return -1;
    }
    bytes_counter = 0;
    mutex_init(&lock);
    return SUCCESS;
}

static void __exit nulll_exit( void ) {
    misc_deregister(&nulll_misc_device);
    mutex_destroy(&lock);
    printk( KERN_INFO "nulll module is unloaded!\n" );
}

module_init( nulll_init );
module_exit( nulll_exit );
