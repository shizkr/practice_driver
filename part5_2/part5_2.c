#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>

dev_t dev = 0;
static struct class *dev_class;

/*
 * Module init function.
 */
static int __init hello_world_init(void) {
	/* Allocating Major number */
	if ((alloc_chrdev_region(&dev, 0, 1, "ext_dev")) < 0){
		printk(KERN_INFO "Cannot allocate major number for device\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/* Creating struct class */
	if ((dev_class = class_create(THIS_MODULE, "ext_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class for device \n");
		goto r_class;
	}

	/* Creating device */
	if ((device_create(dev_class, NULL, dev, NULL, "ext_device")) == NULL) {
		printk(KERN_INFO "Cannot create the device\n");
		goto r_device;
	}
	printk(KERN_INFO "Kernel Module inserted successfully...\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
}

/*
 * Module exit function.
 */
static void __exit hello_world_exit(void) {
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denny Yang <denny.ds.yang@gmail.com>");
MODULE_DESCRIPTION("Part 5 2 sample device driver");
MODULE_VERSION("1.0");
