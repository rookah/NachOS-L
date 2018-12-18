#include "frameprovider.h"

FrameProvider::FrameProvider(int nb_frames)
{
	this->frame_bitmap = new BitMap(nb_frames);
}

FrameProvider::~FrameProvider()
{
	delete this->frame_bitmap;
}

int FrameProvider::GetEmptyFrame()
{
	return this->frame_bitmap->Find();
}

void FrameProvider::ReleaseFrame(int frame)
{
	this->frame_bitmap->Clear(frame);
}

int FrameProvider::NumAvailFrame()
{
	return this->frame_bitmap->NumClear();
}
