/**
 * Tiny loadable kernel module that reads or writes from a single address. 
 * 
 *
 * Based on LKM code from http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction
*/

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include <asm/io.h>	// For iomem functions

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Josh Levine");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("devmemkm tiny but powerfull address peek and poke tool");
MODULE_VERSION("0.1");              ///< The version of the module

static unsigned dummy_addr_target;   // A special address no one would want to write to

#define DUMMY_TARGET_ADDR (resource_size_t) ((&dummy_addr_target))

static resource_size_t addr = DUMMY_TARGET_ADDR ;        
module_param(addr, uint, 0); // 0=Dont make a file in sys to access this param
MODULE_PARM_DESC(addr, "The address to write to");  ///< parameter description

static unsigned val = 0x00;        
module_param(val, uint, 0); // 0=Dont make a file in sys to access this param
MODULE_PARM_DESC(value, "The value  to write");  


/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init helloDMK_init(void){

   void  * mem_addr;

   if (addr==DUMMY_TARGET_ADDR )  {  // User did not provide an address
      printk(KERN_INFO "DMK: No target address provided in param `addr`. Nothing done.\n" );
      return -EINVAL;   // Prints "Invalid argument"
   }

   mem_addr = ioremap_nocache( addr ,  4 );

   printk(KERN_INFO "DMK: Address 0x%x changed from 0x%x to 0x%x\n", addr , ioread32( mem_addr ) , val );

   iowrite32( val , mem_addr );		

   iounmap(mem_addr);

   return -ENOTTY;   // We need to return some error so the module is unloaded and "I am not a bear" is not available in all kernel versions. 
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit helloDMK_exit(void){
   // We really should never get here since we always abort install.
   printk(KERN_INFO "DMK: Goodbye from the DMK LKM! Tell me you saw this! http://josh.com/contact.html\n");
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(helloDMK_init);
module_exit(helloDMK_exit);
