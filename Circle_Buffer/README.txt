Introduction:

This is a C++ project I made for an assignment during my third year at Blekinge Institute of Technology. A Circular Buffer.

The main component of the application is the CircBuffer class that posses the ability to send data packages from one application to another. 
During a follow-up assignment, this circBuffer class was used to convert model and material data on the fly from Autodesk Maya into a gameplay3D (game engine) application.
The result of this was that all models in Maya also appeared in the gameplay3D application, 
and all changes in texture, size and position of the models were also transmitted into gameplay3D on the fly.

The materials provided in this folder mainly tests the circular buffer to make sure it works well. 
These tests are done by sending random strings from a producer circBuffer and then reading it with one or several client CircBuffer applications.
a python application compares the strings that the producer pushes with the strings that the client application receives.
The buffer works if they continue to be identical after thousands of messages.

Because this is a circular buffer. This process should work no matter how many messages are sent and the allocated memory should never run out.


How to test the circular buffer:

Run the test1414.py file found in the release folder(using python). If the CircularBuffer.exe file is in the same directory, and python is used, the test should start.
The test consists of several small tests that force the circular buffer to run thousands of messages to slow clients, fast clients and multiple clients. 
Each test should take about 60 secounds. Tests with multiple clients take longer. The last one is quick.

The circular buffer can also be run by executing the batch file found in the main directory. This prints the producers and clients strings but does not compare them. 
Its quick (only 100 messages, each below a quarter megabite).  


Also do read the code by opening the Circle_Buffer.sln found in the main directory.


  

 
