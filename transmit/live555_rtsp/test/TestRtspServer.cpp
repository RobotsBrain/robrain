#include "BasicUsageEnvironment.hh"

#include "H264VideoSource.h"
#include "H264VideoServerMediaSubsession.h"



int main(int argc, char** argv)
{  
    // Begin by setting up our usage environment:  
    TaskScheduler *scheduler = BasicTaskScheduler::createNew();  
    UsageEnvironment *env = BasicUsageEnvironment::createNew(*scheduler);  
  
    UserAuthenticationDatabase *authDB = NULL;

#ifdef ACCESS_CONTROL  
    // To implement client access control to the RTSP server, do the following:  
    authDB = new UserAuthenticationDatabase;  
    authDB->addUserRecord("username1", "password1"); // replace these with real strings  
    // Repeat the above with each <username>, <password> that you wish to allow  
    // access to the server.  
#endif  
  
    // Create the RTSP server:  
    RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);  
    if (rtspServer == NULL) {  
        *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";  
        exit(1);  
    }  
  
    // Add live stream
    H264VideoSource *videoSource = 0;  
  
    ServerMediaSession *sms = ServerMediaSession::createNew(*env, "live", 0, "live test");  
    sms->addSubsession(H264VideoServerMediaSubsession::createNew(*env, videoSource));  
    rtspServer->addServerMediaSession(sms);  
  
    char *url = rtspServer->rtspURL(sms);  
    *env << "using url \"" << url << "\"\n";  
    delete[] url;  
  
    // Run loop  
    env->taskScheduler().doEventLoop();  
  
    rtspServer->removeServerMediaSession(sms);  
  
    Medium::close(rtspServer);  
  
    env->reclaim();  
  
    delete scheduler;  
  
    return 1;  
}


