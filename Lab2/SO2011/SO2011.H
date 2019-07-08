#ifndef __SO2011_H
#define __SO2011_H

struct so2011_msg_read {
  int   semid;
  int   value;
  int   waitcount;
};

#endif

