#include "vmaccess.h"
#include <stdbool.h>
#include <sys/signal.h>

struct vmem_struct *vmem = NULL;

// converts virtual address to physical
// address  - virtual address
// save     - sets dirty flag
int 
f(int address, bool save)
{
  int page = address / VMEM_PAGESIZE; 
  int offset = address % VMEM_PAGESIZE;
  int flags = vmem-> pt.entries[page].flags;

  // do we need to load the frame?
  int page_loaded = (flags & PTF_PRESENT);
  if(!page_loaded) {
  	vmem->adm.req_pageno = page;
    printf("pid: %d\n", vmem->adm.mmanage_pid);
  	kill(vmem->adm.mmanage_pid, SIGUSR1);
  	sem_wait(&vmem->adm.sema);
  }
 
  vmem->pt.entries[page].flags |= PTF_USED;
  if(save){
  	vmem->pt.entries[page].flags |= PTF_DIRTY;
  }
  vmem->adm.g_count++;
  vmem->pt.entries[page].count = vmem->adm.g_count;
  return (vmem->pt.entries[page].frame * VMEM_PAGESIZE) + offset;
}

void vm_init(void){
  key_t shm_key = SHM_KEY;
  int shm_id = -1;
  shm_id = shmget(shm_key, SHMSIZE, 0664 | IPC_CREAT);
  if(shm_id == -1){
    perror("Error initialising shmget");
    exit(EXIT_FAILURE);
  }
  vmem = shmat(shm_id, NULL, 0);
  if(vmem == (char *)-1){
    perror("Error initialising shmat");
    exit(EXIT_FAILURE);
  }
}

// address - virtual to read from
// returns value from address
int 
vmem_read(int address)
{
  if(vmem == NULL)
  {
    vm_init();
  } 
  int phy_adr = f(address, false);
  return vmem->data[phy_adr];
}

// address  - virtual address
// data     - to store
void
vmem_write(int address, int data)
{
  if(vmem == NULL) 
  {
    vm_init();
  }
  int phy_adr = f(address, true);
  vmem->data[phy_adr] = data;
}

