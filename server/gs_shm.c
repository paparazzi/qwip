#include "gs_shm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct GroundSegment* gs_shm_init(void) {
  int  shmid;
  /* test for an existing shm */
  //shmctl((key_t)GS_SHM_KEY, IPC_RMID, NULL);
  shmid = shmget((key_t)GS_SHM_KEY, sizeof(struct GroundSegment), 0777+IPC_CREAT+IPC_EXCL);
  if (shmid ==-1) { 
    perror ("shmget");
    return NULL; 
  }

  struct GroundSegment* gs;
  gs = shmat(shmid,NULL,0);
  if (gs == (struct GroundSegment*)-1) {
    perror ("shmat");
    return NULL; 
  }

  struct GS_Vehicle* v = &gs->local_vehicles[0];
  v->cpu_time = 0.;

  return gs;
}



void gs_shm_close(struct GroundSegment* gs) {

  if (shmdt(gs)) {
    perror("shmdt");
  }
  if (shmctl((key_t)GS_SHM_KEY, IPC_RMID, NULL)) {
    perror("shmctl:IPC_RMID");
  }
}


