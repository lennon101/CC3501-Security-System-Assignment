#!/usr/bin/env python
from flask import Flask, render_template, make_response, Response, request, redirect, send_from_directory
from camera import Camera
import time
import shutil
import socket
from functools import wraps, update_wrapper
from datetime import datetime
app = Flask(__name__)

cam = Camera()

i=0
isArm = 1;
isVoiceControl = 0;


def check_auth(username, password):
    return username == 'Group5' and password == 'HD'

def authenticate():
    return Response(
    'Could not verify your access level for that URL.\n'
    'You have to login with proper credentials', 401,
    {'WWW-Authenticate': 'Basic realm="Login Required"'})

def requires_auth(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        auth = request.authorization
        if not auth or not check_auth(auth.username, auth.password):
            return authenticate()
        return f(*args, **kwargs)
    return decorated

def nocache(view):
    @wraps(view)
    def no_cache(*args, **kwargs):
        response = make_response(view(*args, **kwargs))
        response.headers['Last-Modified'] = datetime.now()
        response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0'
        response.headers['Pragma'] = 'no-cache'
        response.headers['Expires'] = '-1'
        return response
        
    return update_wrapper(no_cache, view)

@app.route('/')
@requires_auth
def index():
    return render_template('index.html')

def gen(camera):
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/image.jpeg')
def image():
    return Response(cam.get_frame(), mimetype='image/jpeg')

@app.route('/emailSetup')
def my_form():
    return render_template('emailSetup.html')

@app.route('/emailSetup', methods=['POST'])
def my_form_post():
    text = request.form['text']
    file = open('email.txt','w')
    file.write(text)
    file.close()
    return render_template('emailSetup.html')


@app.route('/viewSnapShots')
@nocache
def viewSnapShots():
    return render_template('viewSnapShots.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen(Camera()),
                    mimetype='multipart/x-m, mixed-replace; boundary=frame')


@app.route('/images/<path:path>')
@nocache
def static_file(path):
    return send_from_directory('images',path)


@app.route('/screenshot', methods = ['GET','POST'])
def doScreenshot():
    global i
    i += 1
    if (i==13):
        i=1

    buf = 'images/' + str(i) + '.png'
    shutil.copy2('c.jpg',buf)
    return render_template('index.html')

@app.route('/email', methods = ['GET','POST'])
def doEmail():
    execfile('emailWeb.py')
    return render_template('index.html')

@app.route('/arm', methods = ['GET','POST'])
def doArm():
    global isArm
    isArm = 1
    sendMessage()
    return render_template('index.html')

@app.route('/disarm', methods = ['GET','POST'])
def doDisarm():
    global isArm
    isArm = 0
    sendMessage()
    return render_template('index.html')

@app.route('/voiceoff', methods = ['GET','POST'])
def doDisableVoice():
    global isVoiceControl
    isVoiceControl = 0
    sendMessage()
    return render_template('index.html')

@app.route('/voiceon', methods = ['GET','POST'])
def doEnableVoice():
    global isVoiceControl
    isVoiceControl = 1
    sendMessage()
    return render_template('index.html')

def sendMessage():
    global isArm
    global isVoiceControl

    UDP_IP = ''
    UDP_PORT = 3501
    MESSAGE = str(isArm) + ',' + str(isVoiceControl);

    sendSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sendSocket.connect((UDP_IP, UDP_PORT));
    sendSocket.sendall(MESSAGE)
    sendSocket.close()
    return 1




if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, threaded=True)
