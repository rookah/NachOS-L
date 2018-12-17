#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "copyright.h"
#include "bitmap.h"


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
