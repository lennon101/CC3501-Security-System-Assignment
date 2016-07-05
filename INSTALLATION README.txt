~~ BOYS PLS READ THIS ~~

Yay, voice control. I think the easiest option is just to not touch anything on my Pi ever again and
run the program from it, but if you want to give installing it on your Pi's follow these instructions:

~ PASSO UNO ~
- aka Step 1
- Just to be safe, update your Pi by typing the following commands:
	apt-get update
	apt-get upgrade
- pls note that you need Internet on your Pi for apt-get to work
	- There is a super handy trick you can use to get URLs for the packages you need, if you don't have an
  	  Internet connection on your Pi, type this into command:
		apt-get -y install --print-uris package_name

	  where package_name will be replaced with, well, the package name that you're looking for.
	- this handy tip will give you the URLs for downloading the packages and their dependencies! Yay!
	- then you download them, put them somewhere useful, navigate to the directory they're in and type in
  	  the following command:
		sudo dpkg -i *.deb
	- woohoo software! This comes in handy again later.

~ PASSO DUE ~
- Okay, now the instructions I followed told me to do more updates but yeah nah.
- Now we plug in our USB Mic, which in retrospect may be the hardest step seeing I have it.
- Type the following command:
	nano /etc/modprobe.d/alsa-base.conf
- Nano is a neat text editor in command line, so that's nice.
- Just be careful not to type anything in the wrong spot.
- Find where it says "options snd-usb-audio index=-2"
- Change that -2 to a 0
- Save & exit following the nano instructions that are on your screen.

~ PASSO TRE ~
- It's a good idea to force a reload of the alsa-base.conf file and the Pi itself
- In command type:
	alsa force-reload
	sudo reboot
- You'll have to log in again but everything should be there

~ PASSO QUATTRO ~
- Check that the last few steps worked by typing:
	cat /proc/asound/cards
- You should now see the USB Mic as the number '0' device. If this is the case, yay, good job!
- You can test further by doing this:
	arecord -d 5 -r 48000 test.wav
- IT HAS TO BE A .wav FILE
- This will record a 5 second sound clip in the current directory which you can listen to and check that your audio input is A-OK

~ PASSO CINQUE ~
- Now we need to install things.
- Please go to the software folder, I've already got it all there for you.
- Type the following command:
	sudo dpkg -i *.deb
- This should install all the Debian files that are needed to run Pocketsphinx
- What they do? No clue. Things I assume. I just know that we need them installed before we install Pocketsphinx & Sphinxbase
- And now that I've added the text-to-speech thing, there's some more software to install.
- Navigate to the espeak_software folder and type the same command:
	sudo dpkg -i *.deb
- That should be all you need to do for eSpeak

~ PASSO SEI ~
- Pocketsphinx is dependent on Sphinxbase, so we need to install it first
- Navigate into the 'sphinxbase-5prealpha' folder
- Again, I've done all the fiddly stuff, so you should just be able to type:
	./configure --enable-fixed
- And when that's done:
	make
- Wait a while longer, then to finish it off:
	sudo make install
- Go make a tea, that one takes a while.

~ PASSO SETTE ~
- Pretty much the same as the last step
- Navigate into the 'pocketsphinx-5prealpha' folder
- Type the following commands, in order:
 	./configure
	make
	sudo make install
- And now make another tea. Seriously, it takes a while.
- And to wrap it all up, type:
	LD_LIBRARY_PATH=/usr/local/lib

~ PASSO OTTO ~
- Okay, now to test things!
- To make it simple, do this:
	cd
	cd pocketsphinx/pocketsphinx-5prealpha/src/programs
	pocketsphinx_continuous -dict 9173.dic
- What should happen now is....
	- things will show your current configurations, which aren't really important
	- it will say READY... and listen
	- say "SETTINGS" to see the possible commands :)
	- probs have a clean pair of pants nearby cause this is exciting stuff and you may need 'em

THAT'S ALL FOLKS