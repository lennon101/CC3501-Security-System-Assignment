# CC3501-Security-System-Assignment

##ABSTRACT
The purpose of this project was to investigate, design and implement an indoor security system that involved wireless communication, motion detection, image processing as well as additional features including a website with a video stream and control features. The projects objective has been to innovate an easy to use and affordable system ready for consumer use. This technical report provides and overview of the investigation, design and implementation done to deploy the project prototype.

##CC3501
- GROUP 5: Dane Lennon, Michael Grasso, Stuart Hinchliff, Stephanie Baker

##Project Features and Functionality
This system involves the integration of both hardware and software. It features a custom microprocessor PCB with a PIR motion detecting sensor communicating with a Raspberry Pi via XBee communications.
The features of this project include:
- Webserver application interface with full functionality
- The ability to arm and disarm the system
- Low power initial motion detection using a PIR sensor
- A second check for motion detection with complex image detection algorithms
- Voice configuration mode 
- Ability to email and save screenshots

##Hardware Design
###Printed Circuit Board Design
When designing this system, it was a priority that it must be as small and efficient as possible and only include the minimum number of necessary hardware features on the PCB, these included:
- PIR sensor for motion detection,
- LED to indicate motion detection 
- USB 2.0 port for power and necessary power supply regulations 
- Xbee for Zigbee communication
- JTAG header for SEGA programming 
- Battery holder for backup power

To reduce the size of this board further, a custom microprocessor board was designed to replace the standard shield for a Freescale Board. The Freescale FRDM-K20D50M schematic was used as a reference through the design process. Therefore, the microprocessor design, including the clock source and decoupling capacitor designs, was executed similarly to a professional board design. Figure 3 shows the completed PCB. 

###Central Hub and Peripherals
Implementing a security system with advanced features such as image processing and voice control was a task that could only be performed using a highly functional and heavily customizable device. The obvious choice was a Raspberry Pi, with capabilities of a small computer running the Linux operating system. The Raspberry Pi offers all of the features required by the group to design the central hub. Additionally, many open-source libraries are available for Linux to assist with the necessary features of the system such as the development of image processing, voice control and video streaming.

The Raspberry Pi B+ model features two USB ports and a 3.5mm audio jack, allowing the group to attach all required devices with ease. Additionally, a ribbon camera designed specifically for the Raspberry Pi is available, allowed for easy connection of a camera. The list of peripherals used in this project are:
Raspberry Pi Camera Board 
- USB 2.0 Microphone 
- Wi-Pi USB 2.0 Connector
- Powered USB 2.0 Hub

##Software Design
Robust was fundamental to the project. The system integrates scripts in C, C++, Python, HTML, JavaScript, and many external frameworks and libraries. The two main scripts were the C++ code running the main hub, and the Python code operating the webserver. Details of the functionality are described below.

###Server and Web-application
In order to appropriately develop the project, the target audience and their needs were carefully considered. It was assumed that average users would have no prior technical experience with embedded systems, so the software was designed to be user friendly.
A number of control methods were contemplated in the design process, including the use of a Bluetooth or web application. Upon review of similar products available it was determined that a web application provided the flexibility desired with: 
- Range of use (only an internet connection required 
- Security (authentication methods)
- Interface (intuitive GUI)
- Accessibility on a range of devices
- Potential for expansion
 
Various frameworks were considered to host the server. Due to its popularity and power, the lightweight, Python-based, framework called Flask was chosen. Further, a Bootstrap template was used for the webpage structure to improve the visual appeal and enhance user experience.

####Video Streaming
Surveillance is becoming increasingly prevalent in security systems and was the main drive for developing a web application. The implementation of this was inspired by Miguel Grinberg’s live video stream [1], which demonstrated how versatile Flask could be.
The main complication with using Python was its compatibility with the existing C++ code. It was found that a conflict occurred when attempting to access the camera from Python (for video streaming) and C++ (for image processing) scripts simultaneously. To overcome this, it was decided that saving an image from the C++ code and loading it to Python was the most feasible option. This came with a variety of advantages and disadvantages. It reduced the complexity of the code and it also provided a saved image for use in other processes, but the continuous read/write of images does consume system time. Sockets were also considered, but for image matrices proved complicated and prone to problem.

####Web-Application Control
The website comprises of three main pages: dashboard, view screenshots, and email configuration. The functionality of the website is as follows: (see Figure 2 for flowchart):
- Dashboard: View the live stream and execute available commands.
- Arm (Default): Receive data from the PCB. If a 1 (motion detected) is received image processing code is executed and an email is sent to the saved address.
- Disarm: Stop receiving data from the PCB.
- Save a SnapShot: Capture image of the live stream.
- Email a SnapShot: Send an email of the current image to the saved address.
- Enable Voice: Turn on voice control, allowing the user to configure via voice. o Disable Voice (Default): Turn off voice control.
