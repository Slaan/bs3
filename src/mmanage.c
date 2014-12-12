/* Description: Memory Manager BSP3*/
/* Prof. Dr. Wolfgang Fohl, HAW Hamburg */
/* Winter 2010/2011
 * 
 * This is the memory manager process that
 * works together with the vmaccess process to
 * mimic virtual memory management.
 *
 * The memory manager process will be invoked
 * via a SIGUSR1 signal. It maintains the page table
 * and provides the data pages in shared memory
 *
 * This process is initiating the shared memory, so
 * it has to be started prior to the vmaccess process
 *
 * */

#include "mmanage.h"
#include "stdbool.h"

struct vmem_struct *vmem = NULL;
FILE *pagefile = NULL;
FILE *logfile = NULL;
int signal_number = 0;          /* Received signal */

int
main(void)
{
  struct sigaction sigact;
  /* Init pagefile */
  init_pagefile(MMANAGE_PFNAME);
  if(!pagefile) {
    perror("Error creating pagefile\n");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else {
    fprintf(stderr, "pagefile successfully created\n");
  }
#endif /* DEBUG_MESSAGES */
  /* Open logfile */
  logfile = fopen(MMANAGE_LOGFNAME, "w");
  if(!logfile) {
    perror("Error creating logfile");
    exit(EXIT_FAILURE);
  }

  /* Create shared memory and init vmem structure */
  vmem_init();
  if(!vmem) {
    perror("Error initialising vmem");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else {
    fprintf(stderr, "vmem successfully created\n");
  }
#endif /* DEBUG_MESSAGES */
  /* Setup signal handler */
  /* Handler for USR1 */
  sigact.sa_handler = sighandler;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  if(sigaction(SIGUSR1, &sigact, NULL) == -1) {
    perror("Error installing signal handler for USR1");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else {
    fprintf(stderr, "USR1 handler successfully installed\n");
  }
#endif /* DEBUG_MESSAGES */
  if(sigaction(SIGUSR2, &sigact, NULL) == -1) {
    perror("Error installing signal handler for USR2");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else {
    fprintf(stderr, "USR2 handler successfully installed\n");
  }
#endif /* DEBUG_MESSAGES */
  if(sigaction(SIGINT, &sigact, NULL) == -1) {
    perror("Error installing signal handler for INT");
    exit(EXIT_FAILURE);
  }
#ifdef DEBUG_MESSAGES
  else {
    fprintf(stderr, "INT handler successfully installed\n");
  }
#endif /* DEBUG_MESSAGES */
  /* Signal processing loop */
  while(1) 
  {
    signal_number = 0;
    pause();
    if(signal_number == SIGUSR1) 
    {  /* Page fault */
#ifdef DEBUG_MESSAGES
      fprintf(stderr, "Processed SIGUSR1\n");
#endif /* DEBUG_MESSAGES */
      signal_number = 0;
    }
    else if(signal_number == SIGUSR2) 
    {     /* PT dump */
#ifdef DEBUG_MESSAGES
      fprintf(stderr, "Processed SIGUSR2\n");
#endif /* DEBUG_MESSAGES */
      signal_number = 0;
    }
    else if(signal_number == SIGINT) 
    {
#ifdef DEBUG_MESSAGES
      fprintf(stderr, "Processed SIGINT\n");
#endif /* DEBUG_MESSAGES */
    }
  }
  return 0;
}

/* Please DO keep this function unmodified! */
void
logger(struct logevent le)
{
    fprintf(logfile, "Page fault %10d, Global count %10d:\n"
            "Removed: %10d, Allocated: %10d, Frame: %10d\n",
            le.pf_count, le.g_count,
            le.replaced_page, le.req_pageno, le.alloc_frame);
    fflush(logfile);
}

void 
sighandler(int signo) 
{
  signal_number = signo;
  switch(signo)
  {
    case SIGUSR1: allocate_page();
                  break; 
    case SIGUSR2: dump_pt();
                  break;
     case SIGINT: cleanup();
                  exit(EXIT_SUCCESS);
         default: break;// nop
  }
}

void 
vmem_init(void)
{
    key_t shm_key = SHM_KEY ;
    int   shm_id  = -1;
    /* Create shared memory */
    //shm_key = ftok(SHMKEY, SHMPROCID);
    /* Set the IPC_CREAT flag */
    shm_id = shmget(shm_key, SHMSIZE, 0664 | IPC_CREAT);
    if(shm_id == -1)
    {
      printf("ERROR IN SHMGET");
    }
    vmem = shmat(shm_id, NULL, 0);
    if(vmem == (char *) -1)
    {
      printf("ERROR IN SHM INIT");
      exit(EXIT_FAILURE);
    }
    // Init pagetable 
    vmem->adm.size        = VMEM_NPAGES * VMEM_PAGESIZE;
    vmem->adm.mmanage_pid = getpid();
    vmem->adm.shm_id      = shm_id;
    // shared amoung processes 
    if(sem_init(&(vmem->adm.sema), 1, 0) == -1)
    {
      printf("ERROR IN SEM INIT");
      exit(EXIT_FAILURE);
    }
#ifdef DEBUG_MESSAGES
    printf("pid %d", vmem->adm.mmanage_pid);
#endif 
    return;
}

void 
allocate_page(void)
{
    int req_pageno        = vmem->adm.req_pageno;
    int frame             = VOID_IDX;
    int page_removed_idx  = VOID_IDX;
    if(vmem->pt.entries[req_pageno].flags & PTF_PRESENT)
            
    {
      // no page fault!
      return;
    } 
    /* find free page */
    frame = search_bitmap();
#ifdef DEBUG_MESSAGES
    printf("bitmap: frame %d\n", frame);
#endif // DEBUG_MESSAGES
    if(frame != VOID_IDX) 
    {
      fprintf(stderr, "Found free frame no %d, allocation page\n", frame);
      update_pt(frame);
      fetch_page(vmem->adm.req_pageno);
    } else {
      frame = find_remove_frame();
    }
    /* TODO: impl stuff*/
    page_removed_idx = vmem->pt.framepage[frame];
    /* TODO: impl stuff*/
    /* Store page to be removed and clear present-bit */
    if(vmem->pt.entries[page_removed_idx].flags & PTF_DIRTY) {
      store_page(page_removed_idx);
    }
    vmem->pt.entries[page_removed_idx].flags &= ~PTF_PRESENT;
    /* Load new page */
    update_pt(frame);
    fetch_page(vmem->adm.req_pageno);
    /* Update page fault counter */
    vmem->adm.pf_count++;
    // log allocation
    struct logevent le;
    le.req_pageno     = vmem->adm.req_pageno;
    le.replaced_page  = page_removed_idx;
    le.alloc_frame    = frame;
    le.pf_count       = vmem->adm.pf_count;
    le.g_count        = vmem->adm.g_count;
    logger(le);
    // unblock application
    sem_post(&(vmem->adm.sema));
}

void 
fetch_page(int pt_idx)
{
  int offset  = pt_idx * sizeof(int) * VMEM_PAGESIZE;
  int frame   = vmem->pt.entries[pt_idx].frame;
  int *pstart = &(vmem->data[frame * VMEM_PAGESIZE]);
  /* fseek: change file position indicator for the spcified stream */
  if(fseek(pagefile, offset, SEEK_SET) == -1) 
  {
    perror("couldnt find page");
    exit(EXIT_FAILURE);
  }
  fread(pstart, sizeof(int), VMEM_PAGESIZE, pagefile);
} 

void
store_page(int pt_idx)
{
  int offset  = pt_idx * sizeof(int) * VMEM_PAGESIZE;
  int frame   = vmem->pt.entries[pt_idx].frame;
  int *pstart = &(vmem->data[frame * VMEM_PAGESIZE]);
  fseek(pagefile, offset, SEEK_SET);
  /* TODO: impl stuff */
  fwrite(pstart, sizeof(int), VMEM_PAGESIZE, pagefile);
}

void
update_pt(int frame)
{
  int page_idx  = vmem->adm.req_pageno;
  int bm_idx    = frame / VMEM_BITS_PER_BMWORD;
  int bit       = frame % VMEM_BITS_PER_BMWORD;
  /* update bitmap */
  vmem->adm.bitmap[bm_idx] |= (1U << bit);
  /* increment of next_alloc_idx */
  vmem->adm.next_alloc_idx = (vmem->adm.next_alloc_idx +1) % VMEM_NFRAMES;
  /* update framepage */
  vmem->pt.framepage[frame] = page_idx;
  /* update pt_entry */
  vmem->pt.entries[page_idx].flags     |= PTF_USED | PTF_PRESENT;
  vmem->pt.entries[page_idx].flags     &= ~PTF_DIRTY;
  vmem->pt.entries[page_idx].frame      = frame;
  vmem->pt.entries[page_idx].count = vmem->adm.g_count;
}

int
find_remove_frame(void)
{
  int remove_frame = VOID_IDX;
  switch(VMEM_ALGO) 
  {
      case VMEM_ALGO_LRU: remove_frame = find_remove_lru();
                          break;
    case VMEM_ALGO_CLOCK: remove_frame = find_remove_clock();
                          break;
                 default: remove_frame = find_remove_fifo();
                          break;
  }
  return remove_frame;
}

int 
find_remove_fifo(void)
{

}

// least recently used (F6, S53)
int
find_remove_lru(void)
{

}

int 
find_remove_clock(void)
{
  int remove_frame  = vmem->adm.next_alloc_idx;
  int frame         = remove_frame;
  int page;
  while(true) 
  {
    page = vmem->pt.framepage[frame];
    if(vmem->pt.entries[page].flags & PTF_USED) 
    {
      /* frame used */
      vmem->pt.entries[page].flags &= ~PTF_USED;
      /* clear used flag */ 
      frame = (frame + 1) % VMEM_NFRAMES; 
    } 
    else
    {
      /* frame not marked as used*/
      remove_frame = frame;
      break;
    }
  }
  vmem->adm.next_alloc_idx = remove_frame;
  return remove_frame;
}

int
search_bitmap(void)
{
  int i;
  int free_bit = VOID_IDX;
  for(i = 0; i < VMEM_BMSIZE; i++) 
  {
    Bmword bitmap = vmem->adm.bitmap[i];
    Bmword mask   = (i == (VMEM_BMSIZE - 1) ? VMEM_LASTBMMASK : 0);
    free_bit = find_free_bit(bitmap, mask);
    if(free_bit != VOID_IDX) 
    {
      break;
    }
  }
  return free_bit;
}

int 
find_free_bit(Bmword bmword, Bmword mask)
{
  int     bit     = VOID_IDX;
  Bmword  bitmask = 1;
  bmword |= mask;
  for(bit = 0; bit < VMEM_BITS_PER_BMWORD; bit++)
  {
    if(!(bmword & bitmask))
    {
      break;
    }
    bitmask <<= 1;
  }
  return ((bit < VMEM_BITS_PER_BMWORD) ? bit : VOID_IDX);
}

void
init_pagefile(const char* pfname)
{
  // Create and open pagefile
  pagefile = fopen(pfname, "w");
}

// removes pagefile
void
cleanup(void)
{
  fclose(pagefile);
  remove(MMANAGE_PFNAME);
  fclose(logfile);
}

void dump_pt(void)
{
  printf("Pagetable\n");  
  printf("====================\n");
  int i;
  for(i = 0; i < VMEM_NPAGES; i++) {
    struct pt_entry entry = vmem->pt.entries[0]; 
    printf("Frame: %d, Flags: %d, Count:%d\n", 
                                        entry.frame, entry.flags, entry.count);

  }
}

