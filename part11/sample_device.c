#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

volatile int etx_value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
struct kobject *kobj_ref;

/*
 * Function prototypes
 */
static int	__init etx_driver_init(void);
static void	__exit etx_driver_exit(void);

/******************************** Driver Functions **************************/
static int	etx_open(struct inode *inode, struct file *file);
static int	etx_release(struct inode *inode, struct file *file);
static ssize_t	etx_read(struct file *filp,
			char __user *buf, size_t len, loff_t *off);
static ssize_t  etx_write(struct file *filp,
			const char __user *buf, size_t len, loff_t *off);

/******************** Sysfs Functions ***************************************/
static ssize_t	sysfs_show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf);
static ssize_t 	sysfs_store(struct kobject *kobj,
			struct kobj_attribute *attr, const char *buf, size_t count);

struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

/*
 * File operations structure
 */
static struct file_operations fops = {
	.owner		= THIS_MODULE,
	.open		= etx_open,
	.release	= etx_release,
	.read		= etx_read,
	.write		= etx_write,
};

/*
 * This function will be called when we read the sysfs file.
 */
static ssize_t sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf) {
	printk(KERN_INFO "Sysfs - Read!!!\n");
	return sprintf(buf, "%d", etx_value);
}

static ssize_t sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count) {
	printk(KERN_INFO "Sysfs - Write!!!\n");
	sscanf(buf, "%d", &etx_value);
	return count;
}

/*
 * This function will be called when we open the Device File
 */
static int etx_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Device File opened...!!!\n");
	return 0;
}

/*
 * This function will be called when we release the device file
 */
static int etx_release(struct inode *indoe, struct file *file) {
	printk(KERN_INFO "Device File Closed...!!!\n");
	return 0;
}

/*
 * This function will be called when we read the device file
 */
static ssize_t etx_read(struct file *filp,
			char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Read Function\n");
	return 0;
}

/*
 * This function will be called when we write the device file.
 */
static ssize_t etx_write(struct file *filp,
		const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Write Function.\n");
	return 0;
}

/*
 * Module init function.
 */
static int __init etx_driver_init(void) {

	if ((alloc_chrdev_region(&dev, 0, 1, "etx_dev")) <0) {
		printk(KERN_INFO "Cannot allocate major number\n");
		return -1;
	}

	printk(KERN_INFO "Major = %d Minor = %d\n", MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&etx_cdev, &fops);

	/* adding character device to the system */
	if ((cdev_add(&etx_cdev, dev, 1)) <0) {
		printk(KERN_INFO "Cannot add the device to the system \n");
		goto r_class;
	}

	/* creating struct class */
	if ((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/* Creating device */
	if ((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
		printk(KERN_INFO "Cannot create the device 1\n");
		goto r_device;
	}

	/* Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("etx_sysfs", kernel_kobj);

	/* Creating sysfs file for etx_value */
	if (sysfs_create_file(kobj_ref, &etx_attr.attr)) {
		printk(KERN_INFO "Cannot create sysfs file ... \n");
		goto r_sysfs;
	}

	printk(KERN_INFO "Device Driver Insert... Done!!!\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);

r_device:
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev, 1);
	cdev_del(&etx_cdev);
	return -1;
}

/*
 * Module exit function
 */
static void __exit etx_driver_exit(void) {
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove... Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("denny.ds.yang@gmail.com");
MODULE_DESCRIPTION("Sample sysfs driver");
MODULE_VERSION("1.0");
