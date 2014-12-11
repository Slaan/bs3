#include "vmaccess.h"

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
}

int 
vmem_read(int address)
{
	int phys_add = get_ad(address,0);
	return vmen->data[phys_ad];
}

void
vmem_write(int address, int data)
{
	int phys_add = get_ad(address,1);
	vmen->data[phys_ad] = data;
	return;
}

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
