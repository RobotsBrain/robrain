#include "BasicUsageEnvironment.hh"
#include "liveMedia.hh"

#include "H264VideoServerMediaSubsession.h"
#include "G711AudioStreamServerMediaSubsession.h"



UsageEnvironment* env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = True;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;


int main(int argc, char** argv) 
{
  	// Begin by setting up our usage environment:
  	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  	env = BasicUsageEnvironment::createNew(*scheduler);

  	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  	// To implement client access control to the RTSP server, do the following:
  	authDB = new UserAuthenticationDatabase;
  	authDB->addUserRecord("username1", "password1"); // replace these with real strings
  	// Repeat the above with each <username>, <password> that you wish to allow
  	// access to the server.
#endif

  	int rtsp_audio_port = 8554;
  	// Create the RTSP server:
  	RTSPServer* rtspServer = RTSPServer::createNew(*env, rtsp_audio_port, authDB);
  	if (rtspServer == NULL) {
    	*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    	exit(1);
  	}

  	char const* descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

  	// Set up each of the possible streams that can be served by the
  	// RTSP server.  Each such stream is implemented using a
  	// "ServerMediaSession" object, plus one or more
  	// "ServerMediaSubsession" objects for each audio/video substream.

    char const* streamName = "stream0";
    ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName,
				      descriptionString);
	
    sms->addSubsession(G711AudioStreamServerMediaSubsession::createNew(*env, reuseFirstSource));
    sms->addSubsession(H264VideoServerMediaSubsession::createNew(*env));  
    rtspServer->addServerMediaSession(sms); 

    char* url = rtspServer->rtspURL(sms);
  	*env << "\n\"" << streamName << "\" stream, from the file \"" << streamName << "\"\n";
  	*env << "Play this stream using the URL \"" << url << "\"\n";
  	delete[] url;

  	env->taskScheduler().doEventLoop(); // does not return

  	return 0; // only to prevent compiler warning
}


