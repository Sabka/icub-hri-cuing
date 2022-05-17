
 #include <string>
 #include <stdio.h>
 #include <iostream>


 #include <yarp/os/Network.h>
 #include <yarp/dev/ControlBoardInterfaces.h>
 #include <yarp/dev/PolyDriver.h>
 #include <yarp/os/Time.h>
 #include <yarp/sig/Vector.h>


 #include "constants.cpp"
  
 using namespace std;
 using namespace yarp::dev;
 using namespace yarp::sig;
 using namespace yarp::os;

 bool moving = false;

void gaze(IPositionControl *pos, Vector command, int step, int num_steps)
{
    moving = true;

    int head_step = -HEAD_STEP;
    if(step < 0)
    {
        head_step = HEAD_STEP;
    }

    for(int i=0; i<num_steps; i++)
    {
        command[4]+=step;

        command[1]+=head_step;
        command[2]+=head_step;
        pos->positionMove(command.data());
    }

    bool done=false;
    while(!done)
    {
     pos->checkMotionDone(&done);
    }

    for(int i=0; i<num_steps; i++)
    {
        command[4]-=step;
        command[1]-=head_step;
        command[2]-=head_step;
        pos->positionMove(command.data());
    }

    done=false;
    while(!done)
    {
     pos->checkMotionDone(&done);
    }
    moving = false;
}

void gaze_right(IPositionControl *pos, Vector command)
{

    gaze(pos, command, -EYE_STEP, 10);
}

void set_to_init_position(IPositionControl *pos, Vector command)
{
     command[0]=INIT_HEAD_POSITION; // look a little up
     command[1]=0;
     command[2]=0;
     command[3]=0;
     command[4]=0;
     command[5]=0;
     pos->positionMove(command.data()); // set head joints positions
}

void set_acceleration(IPositionControl *pos, Vector command)
{
    int nj=0;
     pos->getAxes(&nj);
     for (int i = 0; i < nj; i++) 
     {
          command[i] = ROBOT_ACCELERATION;
     }
     pos->setRefAccelerations(command.data());

}

void set_speed(IPositionControl *pos, Vector command)
{
    int nj=0;
     pos->getAxes(&nj);
     for (int i = 0; i < nj; i++) 
     {
         command[i] = ROBOT_SPEED;
         pos->setRefSpeed(i, command[i]);
     }
}


 int main(int argc, char *argv[]) 
 {
     Network yarp;
     if (!yarp.checkNetwork())
     {
         fprintf(stdout,"Error: yarp server not available\n");
         return 1;
     }
  
     Property params;
     params.fromCommand(argc, argv);
  
     if (!params.check("robot"))
     {
         fprintf(stderr, "Please specify the name of the robot\n");
         fprintf(stderr, "--robot name (e.g. icub)\n");
         return 1;
     }

     string robotName=params.find("robot").asString();
     string remotePorts="/";
     remotePorts+=robotName;
     remotePorts+="/head";
     cout<<"remote : "<<remotePorts<<endl;
  
     string localPorts="/test/client";
  
     Property options;
     options.put("device", "remote_controlboard");
     options.put("local", localPorts);   //local port names
     options.put("remote", remotePorts); //where we connect to
  
     // create a device
     PolyDriver robotDevice(options);
     if (!robotDevice.isValid()) 
     {
         printf("Device not available.  Here are the known devices:\n");
         //printf("%s", Drivers::factory().toString().c_str());
         return 0;
     }
  
     IPositionControl *pos;
     IEncoders *encs;
  
     bool ok;
     ok = robotDevice.view(pos);
     ok = ok && robotDevice.view(encs);
  
     if (!ok) 
     {
         printf("Problems acquiring interfaces\n");
         return 0;
     }
  
     int nj=0;
     pos->getAxes(&nj);
     Vector encoders;
     Vector command;
     Vector tmp;
     encoders.resize(nj);
     tmp.resize(nj);
     command.resize(nj);
     command=encoders;
     
     //fisrst read all encoders
     //
     printf("waiting for encoders\n");
     while(!encs->getEncoders(encoders.data()))
     {
         Time::delay(1);
         printf(".");
     }
     printf("\n;\n");
  
     set_acceleration(pos, tmp);
     set_speed(pos, tmp);
     set_to_init_position(pos, command);
     
     bool done=false;
     while(!done)
     {
         pos->checkMotionDone(&done);
     }

     for(int i=0; i<3; i++)
     {
         gaze_right(pos, command);
         while(moving){}
         cout<<i<<endl;

     }

     robotDevice.close();
     
     return 0; 
}