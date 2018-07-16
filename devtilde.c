/* Shamelessly based on the lkmpg's char.c */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define SUCCESS 0
#define DEVICE_NAME "tilde"
#define CLASS_NAME "tilde"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("fauxm");
MODULE_DESCRIPTION("~");
MODULE_VERSION("~.1");

static int Major;
static struct class* tildeC = NULL;
static struct device* tildeD = NULL;

static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

static int __init tilde_init(void)
{
  Major = register_chrdev(0, DEVICE_NAME, &fops);

  if (Major < 0 ) {
    printk(KERN_ALERT "~: Registering char device failed with %d\n", Major);
    return Major;
  }

  tildeC = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(tildeC)) {
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_ALERT "~: Failed to register device class\n");
    return PTR_ERR(tildeC);
  }

  tildeD = device_create(tildeC, NULL, MKDEV(Major, 0), NULL, DEVICE_NAME);
  if(IS_ERR(tildeD)) {
    class_destroy(tildeC);
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_ALERT "~: Failed to create device.\n");
    return PTR_ERR(tildeD);
  }

  printk(KERN_INFO "~: Module loaded successfully~!\n");
  return SUCCESS;
}

static void __exit tilde_exit(void)
{
  device_destroy(tildeC, MKDEV(Major, 0));
  class_unregister(tildeC);
  class_destroy(tildeC);
  unregister_chrdev(Major, DEVICE_NAME);
  printk(KERN_INFO "~: Module cleanup successful~!\n");
}

static int device_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "~: device opened");
  return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

static ssize_t device_read(struct file *filep,
                           char *buffer,
                           size_t length,
                           loff_t *offset)
{
  int errno = 0;
  errno = copy_to_user(buffer, "~", 2);

  if (errno != 0){
    printk(KERN_INFO "~: lol something happened: %d\n", errno);
    return -EFAULT;
  }
  return 2;
}

static ssize_t device_write(struct file *filep,
                            const char *buff,
                            size_t len,
                            loff_t *off)
{
  printk(KERN_ALERT "~: Writing is not supported. Why would you even want to?\n");
  return -EINVAL;
}

module_init(tilde_init);
module_exit(tilde_exit);
