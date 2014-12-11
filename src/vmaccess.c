#include "vmaccess.h"

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
}

int 
vmem_read(int address)
{
  if(vmem == NULL)
  {
    vm_init();
  } 
  int phy_adr = f(address);
}

void
vmem_write(int address, int data)
{
  if(vmem == NULL) 
  {
    vm_init();
  }
  int phy_adr = f(address);

}

// converts virtual address to physical
int 
f(int address)
{
  return 0;
}
