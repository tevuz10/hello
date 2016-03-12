#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define THISMOD "testmod"
#define DEVNAME "nullzero"
#define THISMAJOR 240
#define THISMINOR 0
#define DEVID	MKDEV(THISMAJOR, THISMINOR)

struct cdev nullzero_cdev;
static int nullzero_read(struct file *filp, char __user *buf,
	size_t count, loff_t *offp) {
	char retval=0;
	printk(KERN_INFO "buf=0x%08lx,read count=%ld, offset=%ld\n",(ulong)buf, (ulong)count,(ulong)*offp);
	copy_to_user(buf+(*offp),&retval,1);
	(*offp)++;
	return (1);
}
static int nullzero_write(struct file *filp, char __user *buf,
	size_t count, loff_t *offp) {
	printk(KERN_INFO "buf=0x%08lx,write count=%ld, offset=%ld\n",(ulong)buf, (ulong)count,(ulong)*offp);
	(*offp)++;
	return (1);
}
struct file_operations nullzero_fopts = {
	.owner	=	THIS_MODULE,
	.read	=	nullzero_read,
	.write	=	nullzero_write,
};
int init_module(void){
	int ret;
	printk(KERN_INFO "loaded %s\n",THISMOD);
	ret = register_chrdev_region (DEVID, 1, DEVNAME);
	if (ret != 0) {
		printk(KERN_ERR "error registering %s\n",THISMOD);
		return (1);
	}
	cdev_init(&nullzero_cdev,&nullzero_fopts);
	nullzero_cdev.owner=THIS_MODULE;
	ret = cdev_add(&nullzero_cdev,DEVID,1 );
	if (ret != 0) {
		printk(KERN_ERR "error adding %s\n",THISMOD);
		cleanup_module();
		return (1);
	}
	printk(KERN_INFO "load completed %s\n",THISMOD);
	return (0);
}
void cleanup_module(void){
	cdev_del(&nullzero_cdev);
	unregister_chrdev_region(DEVID, 1);
	printk(KERN_INFO "unloaded %s\n",THISMOD);
}
