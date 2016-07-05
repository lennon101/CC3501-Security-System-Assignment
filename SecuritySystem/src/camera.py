from time import time

class Camera(object):
    def __init__(self):
    	pass
        
    def get_frame(self):
    	self.frames = [open(f + '.jpg', 'rb').read() for f in 'c']
        return self.frames[int(time()) % 1]