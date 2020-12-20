#define DEBUG
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    pr_debug("Hello world!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    pr_debug("Goodbye world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("test <test@domain.com>");
MODULE_DESCRIPTION("Simple \"Hello world\" kernel module");
