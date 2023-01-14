#include "RIFF.h"

using namespace SFZero;


void RIFFChunk::ReadFrom(InputStream* file)
{
	file->read(&id, sizeof(fourcc));
	size = (dword) file->readInt();
	start = file->getPosition();

	if (FourCCEquals(id, "RIFF")) {
		type = RIFF;
		file->read(&id, sizeof(fourcc));
		start += sizeof(fourcc);
		size -= sizeof(fourcc);
		}
	else if (FourCCEquals(id, "LIST")) {
		type = LIST;
		file->read(&id, sizeof(fourcc));
		start += sizeof(fourcc);
		size -= sizeof(fourcc);
		}
	else
		type = Custom;
}


void RIFFChunk::Seek(InputStream* file)
{
	file->setPosition(start);
}


void RIFFChunk::SeekAfter(InputStream* file)
{
	int64 next = start + size;
	if (next % 2 != 0)
		next += 1;
	file->setPosition(next);
}


String RIFFChunk::ReadString(InputStream* file)
{
	char *str = new char[size];
	file->read(str, (size_t)size);
	String s(str);
	delete[] str;
	return s;
}




