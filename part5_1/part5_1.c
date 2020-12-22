#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

dev_t dev = 0;

static int __init hello_world_init(void) {
	/* Allocating Major number */
	if ((alloc_chrdev_region(&dev, 1, 100, "Embedded_Dev")) < 0) {
		printk(KERN_INFO "Cannot allocate major number for device\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
	return 0;
}

static void __exit hello_world_exit(void) {
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denny Yang <denny.ds.yang@gmail.com>");
MODULE_DESCRIPTION("Sample Character Device Driver");
MODULE_VERSION("1.0");
