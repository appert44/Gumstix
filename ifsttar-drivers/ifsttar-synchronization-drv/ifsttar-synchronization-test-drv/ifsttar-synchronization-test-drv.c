//----------------------------------------------------------------------------------------------//
//                                                                                              //
//      FICHIER:	ifsttar-synchronization-test-drv											//
//      AUTEUR:		Jonathan AILLET																//
//      Decription :																			//
//      																						//
//																								//
//----------------------------------------------------------------------------------------------//

//----- Includes Files -------------------------------------------------------------------------//

// #include <asm/io.h>
// #include <asm/ioctls.h>
// #include <asm/uaccess.h>

// #include <linux/cdev.h>
// #include <linux/delay.h>
// #include <linux/device.h>
// #include <linux/errno.h>
// #include <linux/fs.h>
// #include <linux/gpio.h>
// #include <linux/init.h>
// #include <linux/interrupt.h>
// #include <linux/kernel.h>
// #include <linux/kthread.h>
#include <linux/module.h>
// #include <linux/poll.h>
// #include <linux/slab.h>
// #include <linux/string.h>
// #include <linux/types.h>
// #include <linux/version.h>

//----- Includes Applications ------------------------------------------------------------------//
#include "ifsttar-synchronization-test-drv.h"

// ----------- Définitions ---------------------------------------------------------------------//
// Noms utilisés lors de la déclaration du driver
#define DRV_NAME					"ifsttar-synchronization-test-drv > "				// Nom utilisé pour l'affichage du nom du driver lors d'un 'printk' (Typiquement, le même nom que le nom du driver aparaissant lors d'un 'lsmod')

//----- Divers informations --------------------------------------------------------------------//

MODULE_AUTHOR ("Jonathan Aillet <jonathan.aillet@gmail.com>");
MODULE_DESCRIPTION ("IFSTTAR Synchronization Test Driver");
MODULE_LICENSE ("GPL");
MODULE_VERSION ("0.1");

static char     sBanner[] __initdata = KERN_INFO "Synchronization Test Driver : v0.00 :p\n";

//----- Functions  ---------------------------------------------------------------------------- //

GeoDatationData EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER (void);

//----------------------------------------------------------------------------------------------//
//																								//
// Nom de la méthode	:	MyDriver_initialize													//
//																								//
// Description			:	Fonction appelée lors de l'insertion du driver (insmod). 			//
// 							Permet de renseigner des informations sur le driver et de			//
// 							linker les fonctions												//
//																								//
// Retour				:	0 si tout se passe bien, négatif s'il y a une erreur				//
// 							pour empêcher insmod.												//
//																								//
//----------------------------------------------------------------------------------------------//
int __init Test_initialize(void)
{
	GeoDatationData oActualTime;
	
	printk(KERN_NOTICE "%s", sBanner);
	
	oActualTime =EXPORT_GD_DATA_FROM_SYNCHONIZATION_DRIVER();
	
	printk(KERN_DEBUG "%sDate en seconde du driver de synchro : %d\n", DRV_NAME, (int) oActualTime.oData.oTimeTTimeInSeconds);
	
	printk(KERN_NOTICE "%sDriver loaded.\n", DRV_NAME);
	
	return 0;
}


//----------------------------------------------------------------------------------------------//
//																								//
// Nom de la méthode	:	MyDriver_cleanup													//
//																								//
// Description			:	Fonction de destruction du module									//
// 							Appelé lors de la commande rmmod									//
//																								//
// Retour				:               														//
//																								//
//----------------------------------------------------------------------------------------------//
void __exit Test_cleanup(void)
{
	printk(KERN_NOTICE "%sDriver unloaded.\n", DRV_NAME);
}

//----------------------------------------------------------------------------------------------//
//																								//
//	Mapping des fonctions module_init et module_exit											//
//																								//
//----------------------------------------------------------------------------------------------//

module_init(Test_initialize);
module_exit(Test_cleanup);
