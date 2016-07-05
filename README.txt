To run the code, do it the way we do in labs: 
> $ cd SecuritySystem/build
> $ ./main

The main.cpp file - photo capture:
> There is one function that takes one photo as input and simply saves it
> This is the photo we will use for our stream
> It doesn't matter how often you upload the photo because there will always be an
  image with the right name there
> But the main loop is capturing an image every 10 loops
> The image is called "streamImage.jpg" and is found in the src directory

The main.cpp file - motion detection:
> There is one function that takes three frames and compares them for motion
> The difference between frame 1 and 3 is taken (3 being most recent capture)
> The difference between frame 2 and 3 is taken
> An "and" operation is performed on the differences so that only pixels that
  were changed in difference 1 AND difference 2 are counted (this eliminates
  a LOT of background noise)
> Then the image is converted to greyscale and thresholded 
> Thresholding could be upped from 25 to 30, but I found best accuracy with 25
  in my bedroom
> A very small amount of background noise may occur when a fan is on (I found that
  the clothes on my clothes line were moving just enough to trigger about 2-8 pixels
  of "difference") so the pixel count for "motion" has been set so that more than
  10 pixels of movement will signify motion. Might have to finetune this for the 
  robo club room.
> It picks up even small motion up to ~3m away, pls appreciate how stupid I felt jumping
  up and down on my bed and yelling "TAKE PHOTO" at my Pi to test this.
> It should pick up motion further away, still need to run those tests.
> If motion is detected, the images are saved. And this is where we need to add the email
  functionality.

The main.cpp file - voice recognition
> There are 5 commands as it stands, but I'm happy to add more.
> "HELLO" - puts the voice control system into configuration mode, the system WILL NOT LISTEN
  for any other commands until you say "HELLO" to wake it up.
> "ARM" - arms the system, we still need to talk about what we're going to classify as "arming
  the system".
> "DISARM" - disarms the system
> "TAKE PHOTO" - tests the motion detection. Ask the program to take a photo and it will capture
  three frames, and it will verbally tell you whether it detected motion or not. GREAT way to
  check that motion detection is working as it should, and it will let us know if we need to
  fine tune for background noise in a bigger room. Probably my favourite voice control feature.
> "GOODBYE" - puts the voice control back into "not listening" mode. He won't listen for anything
  except the word "HELLO", so the loop will run full speed and the stream will be updated constantly.
> If we want to change these PLEASE PLEASE PLEASE let me do it, I've spent a lot of time
  working on the algorithms behind language models and dictation and I have a pretty deep
  understanding of what's going to work best -- and on that note I've tested a lot of words for the
  above functions and these are the ones I've had the most success and highest accuracy rate with.
> If any of you find that voice control isn't understanding you when you say one of the 
  commands, send me a voice clip of you saying the command and I'll add your pronunciation of the
  command to our dictation model.