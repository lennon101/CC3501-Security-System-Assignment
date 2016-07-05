'''
The python-picamera library is available in the Raspbian archives. 
	Install with apt:

	sudo apt-get update
	sudo apt-get install python-picamera
'''

#import RPi.GPIO as GPIO
import time
#import numpy as np
#import cv2
#import picamera
from datetime import datetime
import os
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email import Encoders
gmail_user = "pi.secure.cc3501@gmail.com" #Sender email address
gmail_pwd = "CC3501gods" #Sender email password

file = open('email.txt', 'r')
to = file.read()

#to = "michael.grasso1@my.jcu.edu.au" #Receiver email address


subject = "Security Breach"
text = "There is some activity in your home. See the attached picture.\n"

picname = "c.jpg"

print "Saving Photo"
		
print "Sending email"
msg = MIMEMultipart()

msg['From'] = gmail_user
msg['To'] = to
msg['Subject'] = subject

msg.attach(MIMEText(text))

part = MIMEBase('application', 'octet-stream')
part.set_payload(open(picname, 'rb').read())
Encoders.encode_base64(part)
part.add_header('Content-Disposition','attachment; filename="%s"' % os.path.basename(picname))
msg.attach(part)

mailServer = smtplib.SMTP("smtp.gmail.com", 587)
mailServer.ehlo()
mailServer.starttls()
mailServer.ehlo()
mailServer.login(gmail_user, gmail_pwd)
mailServer.sendmail(gmail_user, to, msg.as_string())
# Should be mailServer.quit(), but that crashes...
mailServer.close()
print "Email Sent"
#os.remove(picname)
