#!/usr/bin/env python
from flask import Flask, render_template, Response

import shutil

i=0

app = Flask(__name__)

@app.route('/')
def index():
	return render_template('index.html')

@app.route('/screenshot', methods = ['GET','POST'])
def screenshot():
	global i
	i += 1
	buf = 'static/' + str(i) + '.png'
	shutil.copy2('Mario.png',buf)
	return render_template('index.html')

@app.route('/email', methods = ['GET','POST'])
def email():
	execfile('test.py')
	return render_template('index.html')

if __name__ == '__main__':
	app.run(host='0.0.0.0', debug=True, threaded=True)
