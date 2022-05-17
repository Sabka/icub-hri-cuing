 // -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
  
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>


#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>

#include "side.h"

#include "constants.cpp"
#include "serialport.h"
#include "fcntl.h"
  
using namespace std;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


int clearBuffers()
{
     int orig_flags;
     int flags;
     if ((orig_flags = flags = fcntl(0, F_GETFL)) < -1)
     {
         perror("fcntl");
         exit(1);
     }
     flags |= O_NONBLOCK;
     if (fcntl(0, F_SETFL, flags) < 0)
     {
         perror("fcntl");
         exit(1);
     }    char buf[1024];    while (fread(buf, 1024, 1, stdin));
     if (errno != EAGAIN)
     {
         perror("fread");
         exit(1);
     }
     if (fcntl(0, F_SETFL, orig_flags) < 0)
     {
         perror("fcntl");
         exit(1);
     }
     return 0;

}


 bool checkMotionFinished(IPositionControl * pos)
{
     bool jntMotionDone = false;
     pos->checkMotionDone(&jntMotionDone);
     return jntMotionDone;
}

void checkMotionStartAndEnd(IPositionControl * pos)
{
    bool done=true;
    while(done)
    {
        done = checkMotionFinished(pos);
    }

    done=false;
    while(!done)
    {
        done = checkMotionFinished(pos);
    }
}

void gaze(IPositionControl *pos, Vector command, int step, int num_steps)
{

    for(int i=0; i<num_steps; i++)
    {
        command[4]+=step;
        command[3]-= 20;
        int head_step = -HEAD_STEP;
        if(step < 0)
        {
            head_step = HEAD_STEP;
        }
        command[1]-=head_step;
        command[2]+=head_step;
        pos->positionMove(command.data());
    }

    checkMotionStartAndEnd(pos);
}

void gaze_right(IPositionControl *pos, Vector command)
{

    gaze(pos, command, -EYE_STEP, 1);
    
}

void gaze_left(IPositionControl *pos, Vector command)
{

    gaze(pos, command, EYE_STEP, 1);
}

void set_to_init_position(IPositionControl *pos, Vector command)
{
     command[0]=-INIT_HEAD_POSITION; // look a little down
     command[1]=0;
     command[2]=0;
     command[3]=-10;
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
         /*if(i == 4)
         {
             command[i] = 3*ROBOT_SPEED;
         }
         if(i == 3)
         {
             command[i] = 3*ROBOT_SPEED;
         }*/
         pos->setRefSpeed(i, command[i]);
     }
}

void create_confs(vector<pair<side, side>>& confs)
{

    const int N = NUM_OF_ONE_CONF; // later 20
    for(int i=0; i<N; i++)
    {
        confs.push_back(make_pair(side::left, side::left));
    }
    for(int i=0; i<N; i++)
    {
        confs.push_back(make_pair(side::left, side::right));
    }
    for(int i=0; i<N; i++)
    {
        confs.push_back(make_pair(side::right, side::left));
    }
    for(int i=0; i<N; i++)
    {
        confs.push_back(make_pair(side::right, side::right));
    }
    std::random_shuffle(confs.begin(), confs.end());
}

 int main(int argc, char *argv[]) 
 {
     int PARTICIPANT_NUMBER;
     printf("%s\n", "Please enter PARTICIPANT NUMBER.");
     scanf("%d", &PARTICIPANT_NUMBER);

     struct termios tty;

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

     //checkMotionStartAndEnd(pos);

     vector<pair<side,side>> confs;
     create_confs(confs);


     int trial_number = 1;

     int conf_num = 0;
     while(conf_num < confs.size())
     {
         auto c = confs[conf_num];
         conf_num++;

         Time::delay(DELAY_BEFORE_CONF);
         printf("%d", conf_num);
         //auto start_tmp = std::chrono::system_clock::now();


         switch (c.first)
         {
             case side::left:
                 gaze_left(pos, command);
                 break;
             case side::right:
                 gaze_right(pos, command);
                 break;
         }

         //auto end_tmp = std::chrono::system_clock::now();
         //std::chrono::duration<double> elapsed_seconds_tmp = end_tmp-start_tmp;
         //printf("\n conf time  %f seconds\n", elapsed_seconds_tmp.count());


         // REACTION
         // Set terminal to raw mode
         printf("\n");
         system("stty raw");
         clearBuffers();

         Time::delay(DELAY_BEFORE_LIGHT);

         auto start = std::chrono::system_clock::now();

         // turn on led light
         switch (c.second)
         {
             case side::left:
                 printf("left light turned on \n");
                 break;

                 break;
             case side::right:
                 printf("right light turned on \n");
                 break;
         }

         // Wait for single character
         char response = getchar();
         auto end = std::chrono::system_clock::now();
         std::chrono::duration<double> elapsed_seconds = end-start;

         // Echo reaction mesurement
         printf("\n%d %c -> %f seconds\n", trial_number, response, elapsed_seconds.count());

         set_to_init_position(pos, command);


     
         // Reset terminal to normal "cooked" mode
         system("stty cooked");

         // WRITE RESULT TO OUTPUT FILES

         double reaction_time = elapsed_seconds.count() * 1000;

         ofstream outfile;
         outfile.open(OUT_FILE, ios::app);
         outfile<<PARTICIPANT_NUMBER<<","
                <<trial_number<<","
                <<(c.first == 0?"L":"R")<<","
                <<(c.second== 0?"L":"R")<<","
                <<(response == LEFT_KEY? "L":"")
                <<(response == RIGHT_KEY? "R":"")
                <<(((response != LEFT_KEY) && (response != RIGHT_KEY))?  WRONG_KEY_RESPONSE: "")<<","
                <<reaction_time<<"\n";

         outfile.close();

         ofstream participantfile;
         participantfile.open(to_string(PARTICIPANT_NUMBER) + ".csv", ios::app);
         participantfile<<PARTICIPANT_NUMBER<<","
                        <<trial_number<<","
                        <<(c.first == 0?"L":"R")<<","
                        <<(c.second== 0?"L":"R")<<","
                        <<(response == LEFT_KEY? "L":"")
                        <<(response == RIGHT_KEY? "R":"")
                        <<(((response != LEFT_KEY) && (response != RIGHT_KEY))?  WRONG_KEY_RESPONSE: "")<<","
                        <<reaction_time<<"\n";

         participantfile.close();
         trial_number++;

     }
     robotDevice.close();
     
     return 0; 
}
