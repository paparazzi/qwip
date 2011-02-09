#ifndef GS_SHM_H
#define GS_SHM_H

#include "ground_segment.h"

extern struct GroundSegment* gs_shm_init(void);
extern void gs_shm_close(struct GroundSegment*);

#endif /* GS_SHM_H */

