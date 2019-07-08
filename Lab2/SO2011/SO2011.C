#include <so2011.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/delay.h>	/* udelay */
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/unistd.h>
#include <asm/semaphore.h>

MODULE_AUTHOR ("Sistemas 2011");
MODULE_LICENSE("Dual BSD/GPL");


static int major = 0;	/* dynamic by default */

module_param(major, int, 0);

typedef struct task_struct* tsk;

struct Proceso
{
	tsk PCB;
	struct Proceso* next;
};

typedef struct Proceso Proceso;

struct Cola_Procesos
{
	Proceso* head;
	Proceso* last;
};

typedef struct Cola_Procesos CP;

struct semaforo
{
    int valor;
    int cantProcBloq;
    CP procesosBloqueados[5];
};

typedef struct semaforo semaforo;

semaforo** Semaforos;

struct semaphore* farol;

/****************Operaciones de la Cola de Procesos*********************/

void Encolar(CP* PB, tsk PCB, int p)
{
    Proceso* nuevo_nodo = kmalloc(sizeof(Proceso), GFP_KERNEL);
    nuevo_nodo->PCB = PCB;
    if (PB[p].head == NULL)
        PB[p].head = PB[p].last = nuevo_nodo;
    else
    {
        PB[p].last->next = nuevo_nodo;
        PB[p].last = nuevo_nodo;
    }
    nuevo_nodo->next = NULL;
}
// Pre: existe por lo menos un Proceso en el array de Colas de Procesos
tsk Desencolar(CP* PB)
{
    int i = 0;
    Proceso* temp;
    tsk PCB;
    while(PB[i].head != NULL)
        i++;
    PCB = PB[i].head->PCB;
    temp = PB[i].head;
    if (PB[i].head->next == NULL) // es el ultimo proceso
        PB[i].head = PB[i].last = NULL;
    else
        PB[i].head = PB[i].head->next;
    kfree(temp);
    return PCB;
}

void DestruirArrayColas(CP* PB)
{
	int i;
	Proceso* p;
	for(i = 0; i < 5; i++)
	    for (p = PB[i].head; PB[i].head != NULL; p = PB[i].head)
		{
			PB[i].head = p->next;
			kfree(p);
		}
}

/********************************************************************/

int so2011_open (struct inode *inode, struct file *filp)
{
  unsigned int        class;

  class = MINOR(filp->f_dentry->d_inode->i_rdev);
  printk(KERN_INFO "so2011: OPEN %d, PID %d\n",class,current->pid);

  return 0;
}

int so2011_release (struct inode *inode, struct file *filp)
{
  unsigned int        class;

  class = MINOR(filp->f_dentry->d_inode->i_rdev);
  printk(KERN_INFO "so2011: RELEASE %d, PID %d\n",class,current->pid);

  return 0;
}

ssize_t so2011_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned int           class;
    int semid;

    class = MINOR(filp->f_dentry->d_inode->i_rdev);
    printk(KERN_INFO "so2011: READ %d, PID %d\n",class,current->pid);

    if(class == 32 && count == sizeof(struct so2011_msg_read))
    {
        semid = 1;         //*f_pos/count;
        if(semid < 32)
        {
            ((struct so2011_msg_read*)buf)->semid = semid;
            down_interruptible(farol);
            if(Semaforos[semid] == NULL) // semaforo no inicializado
            {
                up(farol);
                ((struct so2011_msg_read*)buf)->value = -1; // -1 indica no inicializado
                ((struct so2011_msg_read*)buf)->waitcount = 0;
            }
            else
            {
                ((struct so2011_msg_read*)buf)->value = Semaforos[semid]->valor;
                ((struct so2011_msg_read*)buf)->waitcount = Semaforos[semid]->cantProcBloq;
                up(farol);
            }
            return sizeof(struct so2011_msg_read);
        }
    }
    return 0;
}

ssize_t so2011_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned int        class,i;

    class = MINOR(filp->f_dentry->d_inode->i_rdev);
    printk(KERN_INFO "so2011: WRITE %d, PID %d\n",class,current->pid);

    if(class >= 0 && class < 32)
    {
        if(count == sizeof(char)*2)
        {
            if(buf[0] == 'i') // se esta intentando inicializar el semaforo numero class
            {
                if(buf[1] == '0' || buf[1] == '1')
                {
                    down_interruptible(farol);
                    if(Semaforos[class] != NULL)
                    {
                        up(farol);
                        printk(KERN_INFO "inicializando semaforo ya inicializado");
                        return -1; // error
                    }
                    else
                    {
                        Semaforos[class] = kmalloc(sizeof(semaforo),GFP_KERNEL);
                        Semaforos[class]->valor = (buf[1] == '0' ? 0 : 1);
                        Semaforos[class]->cantProcBloq = 0;
                        for (i = 0; i<5; i++)
                        {
                            Semaforos[class]->procesosBloqueados[i].head = NULL;
                            Semaforos[class]->procesosBloqueados[i].last = NULL;
                        }
                        up(farol);
                        return sizeof(char)*2;
                    }
                }
            }
            else if (buf[0] == 'p')
            {
                if (buf[1] > '0' && buf[1] < '6')
                {
                    down_interruptible(farol);
                    if(Semaforos[class] != NULL)
                    {
                        if(Semaforos[class]->valor == 1)
                        {
                            Semaforos[class]->valor = 0;
                            up(farol);
                            return sizeof(char)*2;
                        }
                        else
                        {
                            Encolar(Semaforos[class]->procesosBloqueados,current,((int)buf[1] - '0'));
                            Semaforos[class]->cantProcBloq++;
                            up(farol);
                            set_current_state(TASK_INTERRUPTIBLE);
                            schedule();
                            return sizeof(char)*2;
                        }
                    }
                    else
                    {
                        up(farol);
                        printk(KERN_INFO "p de semaforo no inicializado");
                        return -1;
                    }
                }
            }
        }
        else if (count == sizeof(char))
        {
            if(buf[0] == 'v')
            {
                down_interruptible(farol);
                if(Semaforos[class] != NULL)
                {
                    if(Semaforos[class]->valor == 0) // si el valor es 1 v no produce efecto
                    {
                       if((Semaforos[class])->cantProcBloq == 0)
                       {
							Semaforos[class]->valor = 1;
                            up(farol);
                            return sizeof(char);
                       }
                       else // v de semaforo en 0 con procesos bloqueados
                       {
							tsk PCB = Desencolar(Semaforos[class]->procesosBloqueados);
                            Semaforos[class]->cantProcBloq = Semaforos[class]->cantProcBloq - 1;
                            up(farol);
                            wake_up_process(PCB);
                            return sizeof(char);
                       }
                    }
                    else
                    {
                        up(farol);
                        return sizeof(char);
                    }
                }
                else
                {
                    up(farol);
                    printk(KERN_INFO "v de semaforo no inicializado");
                    return -1;
                }
            }
            else if(buf[0] == 'd') // se esta intentando destruir el semaforo numero class
            {
                down_interruptible(farol);
                if(Semaforos[class] != NULL)
                {
                    if (Semaforos[class]->cantProcBloq == 0)
                    {
                        DestruirArrayColas(Semaforos[class]->procesosBloqueados);
                        kfree(Semaforos[class]);
                        Semaforos[class] = NULL;
                        up(farol);
                        return sizeof(char);
                    }
                    else // se intenta eliminar un semaforo en el cual hay procesos bloqueados
                    {
                        up(farol);
                        printk(KERN_INFO "destruyendo un semaforo con procesos bloqueados");
                        return -1;
                    }
                }
                else
                    up(farol);
            }
        }
    }
    return 0;
}

struct file_operations so2011_fops = {
  .owner   = THIS_MODULE,
  .read    = so2011_read,
  .write   = so2011_write,
  .open    = so2011_open,
  .release = so2011_release,
};

int so2011_init(void)
{
    int result,i;
    result = register_chrdev(major, "so2011", &so2011_fops);
    if (result < 0){
        printk(KERN_INFO "so2011: can't get major number\n");
        return result;
    }
    if (major == 0) major = result; /* dynamic */
	
    sema_init(farol,1);
	
    Semaforos = kmalloc(sizeof(semaforo*)*32,GFP_KERNEL);
	for(i = 0; i < 32; i++)
        Semaforos[i] = NULL;

    return 0;
}

void so2011_cleanup(void)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		if(Semaforos[i] != NULL)
		{
            DestruirArrayColas(Semaforos[i]->procesosBloqueados);
            kfree(Semaforos[i]);
            Semaforos[i] = NULL;
		}
	}
	kfree(Semaforos);
	unregister_chrdev(major, "so2011");
}

module_init(so2011_init);
module_exit(so2011_cleanup);
