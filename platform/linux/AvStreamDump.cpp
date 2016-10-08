#include "DumpFile.h"

#include "AvPacket.h"
#include "AvFrameRingBuffer.h"
#include "AvStreamDump.h"



CAvStreamDump::CAvStreamDump()
: Base::CThreadLoop("AvStreamDump")
{

}

CAvStreamDump::~CAvStreamDump()
{

}

bool CAvStreamDump::Start()
{
	return StartThread();
}

bool CAvStreamDump::Stop()
{
	return StopThread();
}
	
void CAvStreamDump::EventHandleLoop()
{
	char buffer[128 * 1024];
	AvPacket packet;
	Base::CDumpFile	 h264file;
	Base::CDumpFile	 pcmfile;

	h264file.Open("test.h264");
	pcmfile.Open("audio.pcm");

	while(1) {
		if(WaitForSleep(100) < 0) {
			break;
		}
		
		if(CAvFrameRingBuffer::Instance()->ReadFrame(packet, buffer, sizeof(buffer)) < 0) {
			continue;
		}

		switch(packet.type) {
		case AV_TYPE_AUDIO:
			pcmfile.Write(buffer, packet.len);
			break;

		case AV_TYPE_VIDEO:
			h264file.Write(buffer, packet.len);
			break;
		}
	}

	pcmfile.Close();
	h264file.Close();

	return;
}


