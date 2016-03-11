#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

// When userspace writes a pointer to /proc/jump, jump to that address in
// kernel mode.
int jump_write(struct file *file, const char *buf,
               unsigned long len, void *data) {
    void (*fun)(void);

    if (len < sizeof(fun))
        return -EINVAL;

    if (copy_from_user(&fun, buf, sizeof(fun)))
        return -EFAULT;

    uint32_t* pcontent = (uint32_t*) fun;
    uint32_t content = *pcontent;
    printk("jump.ko: Jumping to %p\n", fun);
    printk("jump.ko: content %x\n", content);
    fun();

    return len;
}

static const struct file_operations proc_file_fops = {
 .write  = jump_write,
};

// Create a file /proc/jump, with writes handled by jump_write.
int init_jump(void) {
    struct proc_dir_entry *ent = proc_create("jump", 0666, NULL, &proc_file_fops);
    printk("jump.ko: Loaded incredibly insecure kernel module\n");
    return 0;
}

void exit_jump(void) {
    remove_proc_entry("jump", NULL);
}

module_init(init_jump);
module_exit(exit_jump);

MODULE_AUTHOR("Keegan McAllister");
MODULE_DESCRIPTION("Incredibly insecure kernel module for testing exploitation techniques");
MODULE_LICENSE("Dual BSD/GPL");
