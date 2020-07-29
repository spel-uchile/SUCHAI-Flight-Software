#ifndef _OBC_H
#define _OBC_H

class SuchaiFS {
public:
      SuchaiFS();
      void Initialize();
      void MainRoutine(int tick_ms);
      void SetTime(time_t time);

protected:
      void PreTickHook();
      void PosTickHook();
};


#endif //_OBC_H
