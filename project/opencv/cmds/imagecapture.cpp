

#include <vector>
#include <unistd.h>

#include <iostream>
#include "opencv2/core/utility.hpp"

#include "opencv2/imgproc.hpp"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <vector>
#include <drivers/video/fb.h>





using namespace cv;

using namespace std;

int main()

{



	//waitKey(1);


//VideoCapture capture("rtsp://admin:admin@123@192.168.0.100/cam/realmotion?channel=1&subtype=1"); //to capture the frame of ipcamera connected
      VideoCapture capture("sample.avi"); //to check sample video which is in rootfs



Mat frame;




for(;;)




{


       bool OK = capture.grab();


        if (OK == false){


        cout << "cannot grab" << endl;


        }


        else{


          // retrieve a frame of your source


           capture.read(frame);


          //OR


         // capture >> frame;


       }
return 0;
}
}
