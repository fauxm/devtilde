/* Shamelessly based on the lkmpg's char.c */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("fauxm");
MODULE_DESCRIPTION("~");
MODULE_VERSION("~.2.0");

#define SUCCESS 0
#define FAILURE 1

#define CLASS_NAME "tilde"
static struct class* tildeC = NULL;

// Normal tilde
#define TILDEVICE_NAME "tilde"
#define MSG_LEN 1
const static char msg = '~';
static int TildeMajor;
static struct device* tildeD = NULL;

static int tilde_open(struct inode*, struct file*);
static int tilde_release(struct inode*, struct file*);
static ssize_t tilde_read(struct file*, char*, size_t, loff_t*);
static ssize_t tilde_write(struct file*, const char*, size_t, loff_t*);

// Wide tilde
#define WTILDEVICE_NAME "widetilde"
#define WIDEMSG_LEN 3
const static int widemsg = 0x9EBDEF;
static int WTildeMajor;
static struct device* wtildeD = NULL;

#define wtilde_open tilde_open
#define wtilde_release tilde_release
static ssize_t wtilde_read(struct file*, char*, size_t, loff_t*);
#define wtilde_write tilde_write

static struct file_operations tilde_fops = {
  .read = tilde_read,
  .write = tilde_write,
  .open = tilde_open,
  .release = tilde_release
};

static struct file_operations wtilde_fops = {
  .read = wtilde_read,
  .write = wtilde_write,
  .open = wtilde_open,
  .release = wtilde_release
};

// Helper function for dev_init() to call; registers and creates the devices.
static int reg_and_create_devices(void)
{
  TildeMajor = register_chrdev(0, TILDEVICE_NAME, &tilde_fops);
  WTildeMajor = register_chrdev(0, WTILDEVICE_NAME, &wtilde_fops);

  tildeC = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(tildeC)) {
    unregister_chrdev(TildeMajor, TILDEVICE_NAME);
    unregister_chrdev(WTildeMajor, WTILDEVICE_NAME);
    printk(KERN_ALERT "~: Failed to register device class\n");
    return PTR_ERR(tildeC);
  }

  if(TildeMajor < 0) {
    printk(KERN_ALERT "~: Registering %s failed with %d\n", TILDEVICE_NAME, TildeMajor);
  } else {
    tildeD = device_create(tildeC, NULL, MKDEV(TildeMajor, 0), NULL, TILDEVICE_NAME);
    if(IS_ERR(tildeD)) {
      class_destroy(tildeC);
      unregister_chrdev(TildeMajor, TILDEVICE_NAME);
      printk(KERN_ALERT "~: Failed to create %s. Aborting.\n", TILDEVICE_NAME);
      return PTR_ERR(tildeD);
    }
  }

  if(WTildeMajor < 0) {
    printk(KERN_ALERT "~: Registering %s failed with %d\n", WTILDEVICE_NAME, WTildeMajor);
  } else {
    wtildeD = device_create(tildeC, NULL, MKDEV(WTildeMajor, 0), NULL, WTILDEVICE_NAME);
    if(IS_ERR(wtildeD)) {
      class_destroy(tildeC);
      unregister_chrdev(WTildeMajor, WTILDEVICE_NAME);
      printk(KERN_ALERT "~: Failed to create %s. Aborting.\n", WTILDEVICE_NAME);
      return PTR_ERR(wtildeD);
    }
  }

  return SUCCESS;
}

static int __init dev_init(void)
{
  if(reg_and_create_devices() == 0) {
    printk(KERN_INFO "~: Module initiallized successfully~!\n");
    return SUCCESS;
  } else {
    printk(KERN_ALERT "~: Oopsie daisy, we made a fucky-wucky~!\n");
    return FAILURE;
  }
}

static void __exit dev_exit(void)
{
  device_destroy(tildeC, MKDEV(TildeMajor, 0));
  device_destroy(tildeC, MKDEV(WTildeMajor, 0));

  class_unregister(tildeC);
  class_destroy(tildeC);

  unregister_chrdev(TildeMajor, TILDEVICE_NAME);
  unregister_chrdev(WTildeMajor, WTILDEVICE_NAME);

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

static ssize_t wtilde_read(struct file *filep,
                          char *buf,
                          size_t len,
                          loff_t *off)
{
  int errno = 0;
  errno = copy_to_user(buf, &widemsg, WIDEMSG_LEN);

  if (errno != 0){
    printk(KERN_INFO "~: lol something happened: %d\n", errno);
    return -EFAULT;
  }
  return WIDEMSG_LEN;
}
static ssize_t tilde_write(struct file *filep,
                            const char *buf,
                            size_t len,
                            loff_t *off)
{
  printk(KERN_INFO "~: Writing is not supported, silly~!\n");
  return -EINVAL;
}

module_init(dev_init);
module_exit(dev_exit);
