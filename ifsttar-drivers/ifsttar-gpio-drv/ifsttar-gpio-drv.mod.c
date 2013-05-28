#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x961c1a02, "module_layout" },
	{ 0x15692c87, "param_ops_int" },
	{ 0xdbd06b7c, "class_destroy" },
	{ 0x91b2b73e, "class_unregister" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x73559946, "device_destroy" },
	{ 0x9d669763, "memcpy" },
	{ 0x91715312, "sprintf" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x723995b6, "device_create" },
	{ 0xc15f60b4, "cdev_del" },
	{ 0xe7516dad, "__class_create" },
	{ 0xb1512a77, "cdev_add" },
	{ 0x8cfe1e99, "cdev_init" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xd5e521b, "EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER" },
	{ 0xb9e52429, "__wake_up" },
	{ 0x47229b5c, "gpio_request" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x65d6d0f0, "gpio_direction_input" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x562ad513, "interruptible_sleep_on" },
	{ 0x6c8d5ae8, "__gpio_get_value" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0xc2165d85, "__arm_iounmap" },
	{ 0x27e1a049, "printk" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x11f447ce, "__gpio_to_irq" },
	{ 0xfe990052, "gpio_free" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ifsttar-synchronization-drv";


MODULE_INFO(srcversion, "BA16A19D6B257BF930776AA");
