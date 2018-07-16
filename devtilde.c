/* Shamelessly based on the lkmpg's char.c */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define SUCCESS 0
#define TILDE_NAME "tilde"
#define CLASS_NAME "tilde"

const char msg = '~';
#define MSG_LEN 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("fauxm");
MODULE_DESCRIPTION("~");
MODULE_VERSION("~.1.1");

static int Major;
static struct class* tildeC = NULL;
static struct device* tildeD = NULL;

static int tilde_open(struct inode*, struct file*);
static int tilde_release(struct inode*, struct file*);
static ssize_t tilde_read(struct file*, char*, size_t, loff_t*);
static ssize_t tilde_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations tilde_fops = {
  .read = tilde_read,
  .write = tilde_write,
  .open = tilde_open,
  .release = tilde_release
};

static int __init tilde_init(void)
{
  Major = register_chrdev(0, TILDE_NAME, &tilde_fops);

  if (Major < 0 ) {
    printk(KERN_ALERT "~: Registering char device failed with %d\n", Major);
    return Major;
  }

  tildeC = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(tildeC)) {
    unregister_chrdev(Major, TILDE_NAME);
    printk(KERN_ALERT "~: Failed to register device class\n");
    return PTR_ERR(tildeC);
  }

  tildeD = device_create(tildeC, NULL, MKDEV(Major, 0), NULL, TILDE_NAME);
  if(IS_ERR(tildeD)) {
    class_destroy(tildeC);
    unregister_chrdev(Major, TILDE_NAME);
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
  unregister_chrdev(Major, TILDE_NAME);
  printk(KERN_INFO "~: Module cleanup successful~!\n");
}

static int tilde_open(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

static int tilde_release(struct inode *inode, struct file *file)
{
  return SUCCESS;
}

static ssize_t tilde_read(struct file *filep,
                           char *buf,
                           size_t len,
                           loff_t *off)
{
  int errno = 0;
  errno = copy_to_user(buf, &msg, MSG_LEN);

  if (errno != 0){
    printk(KERN_INFO "~: lol something happened: %d\n", errno);
    return -EFAULT;
  }
  return MSG_LEN;
}

static ssize_t tilde_write(struct file *filep,
                            const char *buff,
                            size_t len,
                            loff_t *off)
{
  printk(KERN_INFO "~: Writing is not supported, silly~!\n");
  return -EINVAL;
}

module_init(tilde_init);
module_exit(tilde_exit);
