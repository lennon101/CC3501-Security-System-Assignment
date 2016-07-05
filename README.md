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
1. Printed Circuit Board Design
When designing this system, it was a priority that it must be as small and efficient as possible and only include the minimum number of necessary hardware features on the PCB, these included:
- PIR sensor for motion detection,
- LED to indicate motion detection 
- USB 2.0 port for power and necessary power supply regulations 
- Xbee for Zigbee communication
- JTAG header for SEGA programming 
- Battery holder for backup power

To reduce the size of this board further, a custom microprocessor board was designed to replace the standard shield for a Freescale Board. The Freescale FRDM-K20D50M schematic was used as a reference through the design process. Therefore, the microprocessor design, including the clock source and decoupling capacitor designs, was executed similarly to a professional board design. Figure 3 shows the completed PCB. 
