#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#define WR_VALUE _IOW('a', 'a', int32_t*)
#define RD_VALUE _IOR('a', 'b', int32_t*)

int32_t value = 0;
char etx_array[20] = "try_proc_array\n";
static int len = 1;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

/*
 * Function Prototypes
 */
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/******************** Driver Functions ************************/
static int	etx_open(struct inode *inode, struct file *file);
static int 	etx_release(struct inode *inode, struct file *file);
static ssize_t 	etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t *off);
static long	etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/******************* procfs functions *************************/
static int	open_proc(struct inode *inode, struct file *file);
static int	release_proc(struct inode *inode, struct file *file);
static ssize_t	read_proc(struct file *filp, char __user *buff, size_t length, loff_t *off);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t *off);

/*
 * File operation structure
 *
 */
static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.read		= etx_read,
	.write		= etx_write,
	.open		= etx_open,
	.unlocked_ioctl	= etx_ioctl,
	.release	= etx_release,
};

/*
 * procfs operation structure
 */
static struct file_operations proc_fops = {
	.open	= open_proc,
	.read	= read_proc,
	.write	= write_proc,
	.release = release_proc,
};

/*
 * This function will be called when we open teh procfs file
 */
static int open_proc(struct inode *inode, struct file *file) {
	printk(KERN_INFO "proc file opened....\t");
	return 0;
}

/*
 * This function will be called when we close the procfs file.
 */
static int release_proc(struct inode *inode, struct file *file) {
	printk(KERN_INFO "proc file relase... \n");
	return 0;
}

/*
 * This function will be called when we read the procfs file.
 */
static ssize_t read_proc(struct file *filp, char __user *buff, size_t length, loff_t *offset) {
	printk(KERN_INFO "proc file read.... \n");
	if(len)
		len = 0;	
	else {
		len = 1;
		return 0;
	}
	copy_to_user(buff, etx_array,20);

	return length;
}

static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t *off) {
	printk(KERN_INFO "proc file wrote .... \n");
	copy_from_user(etx_array, buff, len);
	return len;
}

/*
 * This function will be called when we open device fiel
 */
static int etx_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Device File opened...~~~\n");
	return 0;
}

/*
 * This function will be called when we close the device file.
 */
static int etx_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Device File Closed...!!!\n");
	return 0;
}

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "REad Function\n");
	return 0;
}

/*
 * This function wil be called when we write the device file.
 */
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Write Function \n");
	return 0;
}

/*
 * This function will be called when we write IOCTL on teh device file.
 */
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	switch(cmd) {
		case WR_VALUE:
			copy_from_user(&value, (int32_t *)arg, sizeof(value));
			printk(KERN_INFO "Value = %d\n", value);
			break;
		case RD_VALUE:
			copy_to_user((int32_t *)arg, &value, sizeof(value));
			break;
	}
	return 0;
}

/*
 * This function will be called when we init the function.
 */
static int __init etx_driver_init(void) {
	/* Allocating Major Number */
	if (alloc_chrdev_region(&dev, 0, 1, "etx_dev") < 0) {
		printk(KERN_INFO "Cannot allocate major number.\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&etx_cdev, &fops);

	/* Adding character device to the system */
	if (cdev_add(&etx_cdev, dev, 1) < 0) {
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_class;
	}

	/* Creating struct class */
	if ((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/* creating device */
	if ((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
		printk(KERN_INFO "Cannot create the device 1 \n");
		goto r_device;
	}

	/* Creating proc entry */
	proc_create("etx_proc", 0666, NULL, &proc_fops);

	printk(KERN_INFO "Device Driver Insert... DOne!!! \n");
	return 0;
r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

/* Module exit function. */
static void __exit etx_driver_exit(void) {
	remove_proc_entry("etx_proc", NULL);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove... Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denny Yang <denny.ds.yang@gmail.com>");
MODULE_DESCRIPTION("Sample Device Driver");
MODULE_VERSION("1.00");

























