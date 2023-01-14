#ifndef AudioControlSGTL5000Vindor_H
#define AudioControlSGTL5000Vindor_H

class AudioControlSGTL5000Vindor: public AudioControlSGTL5000 {
public:
  bool enable(void);
  bool disable();
  int isShort();
  int getStatus();
  void setRate(int rate);
  // bool hpVolume(unsigned int n);
  bool powerHeadphone(int state);
  bool powerLineout(int state);
  int getId();
};

#endif
