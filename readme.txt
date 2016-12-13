========================================================================
    QIC1832 Encoder Camera SDK : QIC1832 Library & Sample Code
========================================================================

binary : The binary of Directshow filters and sample program.
   -dependency : The installation files for using Quanta XU or QTCam.
      -ffdshow_rev4422_20120409.exe: ffdshow for decoding H.264, if you're using Windows 7 (or upon), 
                                     you may not need H.264 decoder because Windows includes one.
   -QTcam_Small_2012.exe : The binary of the sample program. Two way YUV+H.264.
   -QTcam_Small_2012_static_linked.exe: Like QTcam_Small_2012.exe but uses static link to MFC.

include : The header files of BaseClasses, XUControl library, and H264demux.

lib : the static library of BaseClasses, XUControl, H264demux
   -vc2012: built with VC 2012
      [remark]: all the libraries are built in release mode and library with a following "s" in the end of name, 
                is built with code generation /MT option. (default is /MD). We prepare baseclasses library here 
                for convenient. Of course, you could compile it yourself.		
      -x64: libraries in 64 bits.
   

sample : The sample program source code.

QIC1832_Encoding_Camera_Application_Implementation_Guide.pdf :
   -The QIC encoding camera implementation guide describes the fundamental of sample code.

QIC1832_Encoding_Camera_XULib_APIs).pdf :
   -The XULib APIs document describes the details of library usage.