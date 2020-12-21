/***********************************************************************
 * driver.c
 * details simple linux driver (statistically allocating the major and minor number)
 * author: embedded linux device driver
 ***********************************************************************/
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>

dev_t dev = MKDEV(100,0);

// module init function.
static int __init hello_world_init(void) {
	register_chrdev_region(dev, 1, "embedded_dev");
	printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Kernel module inserted successfully... \n");
	return 0;
}

/*
 * module exit function.
 */
static void __exit hello_world_exit(void) {
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Kernel module removed successfully...\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denny Yang <denny.ds.yang@gmail.com>");
MODULE_DESCRIPTION("Simple Linux Device Driver (statically allocating the major and minor number)");
MODULE_VERSION("1.0");

