#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"
#include "copyright.h"

class FrameProvider
{
  public:
	FrameProvider(int nb_frames);
	~FrameProvider();

	int GetEmptyFrame();
	void ReleaseFrame(int frame);
	int NumAvailFrame();

  private:
	BitMap *frame_bitmap;
};

#endif // FRAMEPROVIDER_H
