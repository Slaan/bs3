#include "vmaccess.h"

<<<<<<< HEAD
struct vmem_struct *vmem = NULL;

void 
vmem_init(void)
{
    key_t shm_key = 1337;
    int   shm_id  = -1;
	
    /* Set the IPC_CREAT flag */
    shm_id = shmget(shm_key, SHMSIZE, 0664 | IPC_CREAT);
    if(shm_id == -1) {
		perror("shm_id: Error initializing shmget");
		exit(EXIT_FAILURE);
	}
		
	/*TODO: implement me*/
    vmem = shmat(shm_id, NULL, 0);
    if(vmen == NULL) {
		perror("vmen: Error initalizing shmat");
		exit(EXIT_FALIURE);
	}
=======
struct vmem_struct* vmem = NULL;

// accessing shared memory
void
vm_init(void)
{
  key_t shm_key = SHM_KEY;
  int   shm_id  = -1;
  shm_id = shmget(shm_key, SHMSIZE, 0664 | IPC_CREAT);
  if(shm_id != 0)
  {
    perror("Error accessing shared memory\n");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else
  {
    printf("Successfully accessing shared memory");
  }
#endif // DEBUG_MESSAGES
  vmem = shmat(shm_id, NULL, 0);
  // TODO: add debug output
  return;
>>>>>>> f41bf93d64b9a33e5e0b91b882deeb78238f0bee
}

int 
vmem_read(int address)
{
<<<<<<< HEAD
	int phys_add = get_ad(address,0);
	return vmen->data[phys_ad];
=======
  if(vmem == NULL)
  {
    vm_init();
  } 
  int phy_adr = f(address);
>>>>>>> f41bf93d64b9a33e5e0b91b882deeb78238f0bee
}

void
vmem_write(int address, int data)
{
<<<<<<< HEAD
	int phys_add = get_ad(address,1);
	vmen->data[phys_ad] = data;
	return;
}
=======
  if(vmem == NULL) 
  {
    vm_init();
  }
  int phy_adr = f(address);
>>>>>>> f41bf93d64b9a33e5e0b91b882deeb78238f0bee

int get_ad(int address, int save) {
	if (vmen == NULL) {
		vmen_init();
	}
	
	int page = address / VMEM_PAGESIZE;
	int offset = address % VMEM_PAGESIZE;
	int flags = vmen-> pt.entries[page].flags;
	int page_loaded = ((flags & PTF_PRESENT) == PTF_PRESENT);
	
	if(!page_loaded) {
		vmen->adm.req_pageno = page;
		kill(vmen->adm.mmanage_pid, SIGUSR1);
		sem_wait(&vmem->adm.sema);
	}
	
	vmen->pt.entries[page].flags |= PTF_USED;
	if(save){
		vmem->pt.entries[page].flags |= PTF:DIRTY;
	}
	vnem->adm.g_count++;
	vnem->pt.entries[page].count = vmem->adm.g_count;
	return (vmem->pt.entries[page].frame * VMEM_PAGESIZE) + offset;
}

// converts virtual address to physical
int 
f(int address)
{
  return 0;
}
