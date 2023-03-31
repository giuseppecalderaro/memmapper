#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mman.h>
#include <linux/device.h>
#include <linux/kprobes.h>
#include <memmapper-main.h>

static int memmapper_major_number;
static struct class *memmapper_class;
#ifdef CONFIG_KRETPROBES
static int kprobe = 0;
module_param(kprobe, int, S_IRUGO);
#endif

static int memmapper_mmap(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;

	/* Remap-pfn-range will mark the range VM_IO and VM_RESERVED */
	if (remap_pfn_range(vma,
			    vma->vm_start,
			    vma->vm_pgoff,
			    size,
			    vma->vm_page_prot)) {
		return -EAGAIN;
	}

	return 0;
}

static struct file_operations memmapper_fops = {
	.mmap = memmapper_mmap,
};

#ifdef CONFIG_KRETPROBES
static int memmapper_kretprobe_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
	regs->ax = 0;
	return 0;
}

static struct kretprobe memmapper_probe = {
	.handler = memmapper_kretprobe_handler,
	.kp.symbol_name = "reserve_pfn_range",
};
#endif

int __init memmapper_load(void)
{
	int __ret;

#ifdef CONFIG_KRETPROBES
	if(kprobe) {
		__ret = register_kretprobe(&memmapper_probe);
		if(__ret < 0) {
			printk(KERN_WARNING "kretprobe NOT registered. Reverting to chardev...\n");
			kprobe = 0;
		}
	}

	if(!kprobe) {
#endif
		memmapper_major_number = register_chrdev(MEMMAPPER_MAJOR, MEMMAPPER_NAME, &memmapper_fops);
		if(memmapper_major_number < 0)
		{
			printk(KERN_ERR "Unable to register char device.\n");
			return -ENODEV;
		}

		memmapper_class = class_create(THIS_MODULE, MEMMAPPER_NAME);
		device_create(memmapper_class, NULL, MKDEV(memmapper_major_number, 0), NULL, MEMMAPPER_NAME);
#ifdef CONFIG_KRETPROBES
	}
#endif

	return 0;
}

void __exit memmapper_unload(void)
{
#ifdef CONFIG_KRETPROBES
	if(kprobe) {
		unregister_kretprobe(&memmapper_probe);
	} else {
#endif
		device_destroy(memmapper_class, MKDEV(memmapper_major_number, 0));
		class_destroy(memmapper_class);
		unregister_chrdev(memmapper_major_number, MEMMAPPER_NAME);
#ifdef CONFIG_KRETPROBES
	}
#endif
}

module_init(memmapper_load);
module_exit(memmapper_unload);

MODULE_LICENSE(MEMMAPPER_LICENSE);
MODULE_AUTHOR(MEMMAPPER_AUTHOR);
