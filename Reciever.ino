Noreen
noreen9029
Invisible

Noreen
 started a call that lasted a few seconds.
 — 04/18/2024 7:59 PM
Mustafa — 04/18/2024 8:09 PM
Oh sorry I was away from my phone
Can i call?
You missed a call from 
Mustafa
 that lasted a minute.
 — 04/18/2024 8:10 PM
Mustafa — 04/18/2024 8:11 PM
Call me later tonight if you have time
Mustafa
 started a call that lasted 3 minutes.
 — 04/19/2024 12:22 PM
Noreen — 04/19/2024 2:52 PM
Mustafa would you mind showing the video?
when it's our turn?
Mustafa — 04/19/2024 2:55 PM
yh sure
https://www.youtube.com/watch?v=xdbdz-OXyt0
YouTube
Belal Assaad
Tawakkul (Trusting in Allah) 45 mins + 40 mins Q&A - Amazing!
Image
this is the video I was tellin u about yesterday
I personally love the guy who's presenting
Mustafa — 04/19/2024 2:57 PM
Also, I'll read your paper a little later tonoight or tomorrow morning. When is it due?
Noreen — 04/19/2024 2:59 PM
its due on the 28th
Noreen — 04/19/2024 4:04 PM
Do you need me to stay back?
Mustafa — 04/19/2024 4:04 PM
nah its ok
Noreen — 04/19/2024 4:04 PM
Okay sounds good
Mustafa — 04/20/2024 11:01 AM
Assalamualaikum my dear and beloved sister
So I read your paper, and it looks good, but some quick comments from my end:

I dont really think that image is necessary, usually when ur writing an IEEE paper, the image need to give some sort of detailed description of what it is your talking about. Since your talking about the security of cloud computing, that images doesnt really make sense to me. To be safe, I would screenshot that page and send a quick email to the prof regarding his/her opinion on it.
Dunno if you haven't finished the paper yet, but there are 2 part 3s (III), one called "CLOUD COMPUTING AND CHALLENGES" and another called "CASE AND STUDIES AND EXAMPLE: ANALYSIS OF THE CAPITAL ONE BREACH" which has nothing in it.
On page 3, part 5 (V) needs to have a period
Also in part V and VI, some paragraphs need to be spaced out.
For part VII, give a brief paragraph (2-3 sentences) before listing the tools
Also, the first paragraph in every part should have an indent. Thats just an IEEE paper thing
 
If u have any questions, just ask here, but I'll be away from my computer for a while so I wont be able to answer them right away
Mustafa — 04/20/2024 10:55 PM
This is my term paper by ECE 430
Attachment file type: document
ECE_430_.docx
136.11 KB
Noreen — 04/21/2024 7:34 AM
Hey mustafa something urgent came up can we do 11 or 12pm?
Mustafa — 04/21/2024 7:39 AM
sure
Noreen
 started a call that lasted 6 minutes.
 — 04/21/2024 11:55 AM
Mustafa
 started a call that lasted a minute.
 — 04/22/2024 10:10 AM
Mustafa — 04/22/2024 10:39 AM
import cv2
import mediapipe as mp
import sys
import serial

Initialize MediaPipe Hands.
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(min_detection_confidence=0.5, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

Set up the serial connection to Arduino (update the port if necessary)
arduino_serial = serial.Serial('COM19', 115200, timeout=1)  # Update port as needed

cap = cv2.VideoCapture(0)

Flag to indicate whether hand tracking is active
tracking_active = True

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    if tracking_active:
        # Convert the frame colors from BGR to RGB.
        frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = hands.process(frame_rgb)

        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                # Assuming you want to track the index finger tip.
                index_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
                # Output the coordinates.
                sys.stdout.write(f'{index_tip.x},{index_tip.y},{index_tip.z}\n')
                sys.stdout.flush()

        # For debugging, show the frame with drawn landmarks.
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

    cv2.imshow("Frame", frame)

    # Read and process data from Arduino
    if arduino_serial.inWaiting() > 0:
        tracking_active = False  # Turn off tracking when Arduino sends something
        arduino_data = arduino_serial.readline().decode('utf-8').rstrip()
        print(arduino_data)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

arduino_serial.close()  # Close the serial connection to Arduino
cap.release()
cv2.destroyAllWindows()
You missed a call from 
Mustafa
 that lasted a few seconds.
 — 04/22/2024 10:53 AM
Mustafa — 04/22/2024 11:29 AM
#include <RH_ASK.h>
#include <SPI.h>

// reciever program (Arduino UNO)

RH_ASK driver(2000,4,2);

void setup() {
    Serial.begin(115200);
    if (!driver.init()) {
        Serial.println("Initialization failed");
    }
}

bool messagePrinted = false; // Flag to track if the message has been printed

void loop() {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv(buf, &buflen)) {
        // Check if the received message is "controls on!"
        if (buflen == 12 && strncmp((char*)buf, "controls on!", 12) == 0) {
        } else {
            // If it's not "controls on!", print the received message
            for (int i = 0; i < buflen; i++) {
                Serial.print((char)buf[i]);
            }
            Serial.println(); // Print a newline to separate messages
        }
    }
}
Noreen — 04/22/2024 11:37 AM
use this code and see if it's working: #include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver(2000, 4, 2); // Adjust parameters as necessary
int buttonPin = 3;         // Pin where the button is connected
int buttonState = LOW;     // Current state of the button
int lastButtonState = LOW; // Previous state of the button
unsigned long lastDebounceTime = 0; // Last time the button state was changed
unsigned long debounceDelay = 50;   // Debounce delay to prevent flickering
int pressCount = 0;                 // Number of button presses

void setup() {
    pinMode(buttonPin, INPUT);
    Serial.begin(115200);
    if (!driver.init()) {
        Serial.println("Initialization failed");
    }
}

void loop() {
    int reading = digitalRead(buttonPin);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                pressCount++;
                handleButtonPress(pressCount);
            }
        }
    }

    lastButtonState = reading;
}

void handleButtonPress(int count) {
    if (count % 2 == 0) {
        driver.send((uint8_t )"controls on!", 12);
        driver.waitPacketSent();
        Serial.println("Controls ON");
    } else {
        driver.send((uint8_t)"controls off!", 13);
        driver.waitPacketSent();
        Serial.println("Controls OFF");
    }
}
Mustafa — 04/22/2024 11:40 AM
lol it didnt work
thanks tho, appreciate ya helpin out😊 
Mustafa — 04/22/2024 12:10 PM
#include <RH_ASK.h> // Include the RadioHead library for ASK modulation
#include <SPI.h> // Include the SPI library for communication with the RF module

RH_ASK driver(2000, 4, 2); // Initialize the RH_ASK driver with a bitrate of 2000bps and specify the data pin (4) and the chip select pin (2)

void setup() {
Expand
receiver.txt
3 KB
#include <RH_ASK.h>
#include <SPI.h>

#define pwmPin 14

RH_ASK driver(2000, 0, 4);
Expand
transeiver.txt
2 KB
Mustafa — 04/22/2024 4:11 PM
U forgot ur charger my friend 
Noreen — 04/22/2024 8:54 PM
Thanks for letting me know Oumy told me she has it now
I’ll get it from her Tom
Mustafa
 started a call that lasted 2 minutes.
 — 04/23/2024 12:39 AM
Noreen — 04/23/2024 5:03 AM
No need for a reminder at 7:30. I’ll remember to bring it
Mustafa — 04/23/2024 6:40 AM
Oki oki
Thank you
Mustafa — 04/23/2024 12:35 PM
Assalamualaikum
for the handtracking, do the webcams use the cv2 library to capture the frames?
Noreen — 04/23/2024 12:42 PM
yes
Noreen
 started a call that lasted 5 minutes.
 — 04/24/2024 3:33 PM
Mustafa — 04/24/2024 3:38 PM
http://ecasp.ece.iit.edu/
Noreen — 04/24/2024 4:22 PM
Did Brandon send you the presentation yet?
Noreen
 started a call that lasted a few seconds.
 — 04/24/2024 5:12 PM
Noreen
 started a call that lasted 4 minutes.
 — 04/24/2024 5:13 PM
Mustafa
 started a call that lasted 7 minutes.
 — 04/24/2024 6:37 PM
Mustafa — 04/24/2024 6:38 PM
one sec my mics connecting
Mustafa — 04/24/2024 7:21 PM
sendin the powerpoint to ya
it'll take a min tho cuz the file is big
Noreen
 started a call that lasted 2 minutes.
 — 04/24/2024 7:39 PM
Noreen
 started a call that lasted 3 minutes.
 — 04/24/2024 7:41 PM
Mustafa — 04/24/2024 7:48 PM
sorru I was prayin asr, nd my phone was on dnd
Noreen — 04/24/2024 7:54 PM
please stay here for submission
Mustafa — 04/24/2024 7:55 PM
bet
Mustafa — 04/24/2024 8:16 PM
let me know when its submitted I need to go pray
Mustafa
 started a call that lasted a minute.
 — 04/24/2024 8:50 PM
Mustafa — 04/24/2024 9:24 PM
Thank you!🙏
I dont have the time to be exporting it
I wont be able to resubmit rn
Noreen — 04/24/2024 9:28 PM
I have to also leave and one of your recordings were not playing not sure why was that happening
I’ll just send it but we can submit later
Or have Brandon do it
Mustafa — 04/24/2024 9:28 PM
I wont be doing anything else till tomorrow morning, so we'll have to leave it till tomorrow morning
are you still set on redoing the demo video tomorrow?
Please respond, I need to know now so I can set up the hardware for tomorrow morning
Noreen — 04/24/2024 9:34 PM
That’s fine we won’t re-record
Mustafa — 04/24/2024 9:38 PM
oki oki
Thank you for submitting
Your the best!🎉
Noreen — 04/24/2024 10:24 PM
No problem
Could you please bring the hardware to the lab?
Mustafa — 04/25/2024 5:25 AM
Yh sure, I’m bringing everything today so that I can give it to Xinrui
Noreen — 04/25/2024 6:49 AM
Would it be possible for you to assemble the hardware during lab time?
Or is it not possible anymore?
Mustafa — 04/25/2024 6:57 AM
Tbh I don’t know if it’ll be possible. Too much to put together 
But if Xinrui’s advice scares me, I’ll reassemble it and we can try to make a new demo
Mustafa — 04/25/2024 7:15 AM
Actually, I might be able to do it
Noreen — 04/25/2024 3:14 PM
Mustafa
would you ever be coming back to campus?
or would you be at your dorm working?
Mustafa — 04/25/2024 4:00 PM
I’ll be at my dorm
Wanna be by myself for a moment
Noreen — 04/25/2024 4:01 PM
im sorry
if i hurt you
Mustafa — 04/25/2024 4:48 PM
Hey
I know u have a physics quiz later, but would you have a couple of minutes to meet?
The others can join if they’d like
Noreen — 04/25/2024 4:52 PM
yeah sure
Noreen
 started a call that lasted a minute.
 — 04/25/2024 5:42 PM
Noreen
 started a call that lasted a minute.
 — 04/26/2024 9:36 AM
Mustafa — 04/26/2024 10:43 AM
oh sorry, just saw that u called
ur slides are all good 
Mustafa — 04/26/2024 2:25 PM
Noreen — 04/26/2024 2:26 PM
shut up
Mustafa — 04/26/2024 2:26 PM
😭 😭
ngl I was gonna do the same but I was afriad that I was gonna get caught
Noreen — 04/26/2024 2:54 PM
Mustafa are you on campus right now?
Could you please do me a favor?
Mustafa — 04/26/2024 2:55 PM
wassup
Noreen — 04/26/2024 2:55 PM
Image
could you please go pickup my exam and scan it and send it to me
Mustafa — 04/26/2024 2:56 PM
sure but damn
hopefully thats not out of 100
Noreen — 04/26/2024 2:56 PM
why?
Mustafa — 04/26/2024 2:56 PM
lol
Noreen — 04/26/2024 2:56 PM
it is out of 100
Mustafa — 04/26/2024 2:56 PM
I mean like if a B is 19 out of 100, thats pretty bad lol
ohhhh
Noreen — 04/26/2024 2:57 PM
she meant 11 people got an A
Mustafa — 04/26/2024 2:57 PM
nvm nvm nvm
Noreen — 04/26/2024 2:57 PM
🤦‍♀️
Mustafa — 04/26/2024 2:57 PM
yh yh i was rlly slow on that😓
Noreen — 04/26/2024 2:57 PM
lol that's fine
Mustafa — 04/26/2024 2:57 PM
lol
Noreen — 04/26/2024 2:57 PM
dummy
Mustafa — 04/26/2024 2:58 PM
bigger sibling = bigger dummy
lol I'll head there after class
Noreen — 04/26/2024 3:02 PM
Okay thank you
Noreen — 04/26/2024 3:02 PM
lol 😝
Mustafa — 04/26/2024 3:06 PM
lol I like this emoji, its cute
Mustafa — 04/26/2024 3:39 PM
Image
😂 😂
Noreen — 04/26/2024 3:42 PM
i knew someone would do it
Mustafa — 04/26/2024 3:43 PM
lol dont worry im sure no one noticed, I only know cuz I was editing earlier
Mustafa
 started a call that lasted 2 minutes.
 — 04/26/2024 4:03 PM
Noreen — 04/26/2024 4:03 PM
Why are you calling?
Mustafa — 04/26/2024 4:03 PM
Pick up
Please
Noreen — 04/26/2024 4:07 PM
Mustafa
Noreen
 started a call that lasted 5 minutes.
 — 04/26/2024 4:07 PM
Noreen — 04/26/2024 4:14 PM
What’s my grade for exam2?
Mustafa — 04/26/2024 4:15 PM
Sending rn 
One sec my phones being slow
Mustafa — 04/26/2024 4:23 PM
U got a 98/105
I’ll send it later my phone is acting up
Noreen — 04/26/2024 4:24 PM
Okay thank you
Mustafa — 04/26/2024 4:31 PM
No problem 🙂
You missed a call from 
Mustafa
 that lasted a minute.
 — 04/27/2024 7:26 PM
Mustafa — 04/27/2024 7:27 PM
Hey Noreen
actually nvm
Noreen — 04/27/2024 7:31 PM
hey just saw this
whats up
Im confused in term os hardware. How much stuff should i put in my report
for hardware
like should i put each component?
Mustafa — 04/27/2024 7:33 PM
I was gonna ask if u can send ur individual report so that we can make sure it's consistent
Mustafa — 04/27/2024 7:33 PM
look at my individual report
Noreen — 04/27/2024 7:37 PM
I just shared it 
Also do you know how to add linked to the table of conten?
Noreen — 04/27/2024 7:39 PM
i dont have you individual report
Mustafa — 04/27/2024 7:43 PM
yh yh, highlight the word you want to link, and do ctrl + k
there should be options that you want to link to, so link it to the appropriate title
Mustafa — 04/27/2024 7:43 PM
lol i posted it in the GC
its pinned
Noreen — 04/27/2024 7:45 PM
the link to my report isn't working
could you try it in my report
Mustafa — 04/27/2024 7:45 PM
yh sure
one sec tho
Noreen — 04/27/2024 7:46 PM
okay
Mustafa — 04/27/2024 8:09 PM
got the links for ya
Noreen — 04/27/2024 8:24 PM
Oh I’m not on my desk but you didn’t have to do them all only try one so I could see why the kinks aren’t working for me 😅
But thank you
🙏
Mustafa — 04/27/2024 8:43 PM
lol too late🙃
No problem 😉
Noreen — 04/27/2024 9:00 PM
are you tryna finish these tonight?
Mustafa — 04/27/2024 9:31 PM
Imma try to 95% of everything done
tomorrow im not touchin any of this stuff till like 6-7pm in the evening
cuz imma spend the majority of the day studying for my final on Monday
Noreen — 04/27/2024 9:37 PM
oh okay
I have 444 paper due tomorrow as well
Mustafa — 04/27/2024 9:55 PM
good luck with that! I wrote some feedback a while back in this message section
btw if u have time later on tomorrow after you submit all ur deliverables, can you give me some quick feedback on my ECE 430 paper? U dont have to read the entire thing, just a quick 10-15 min skim to make sure it looks good overall
Noreen — 04/27/2024 11:03 PM
YEAH ILL DO THAT IN THE MORNING
oh sorry
i think my individual report is done
Mustafa — 04/27/2024 11:22 PM
thank you🙏
Noreen — Yesterday at 2:04 PM
Should i just add the reference numbers or mention the paragraph and stuff for in line citation?
Mustafa — Yesterday at 3:36 PM
Where are you guys sitting?
Noreen — Yesterday at 3:39 PM
at the lobrary
Noreen — Yesterday at 8:50 PM
what the hell have u been typing for the last 10 min?
Mustafa — Yesterday at 8:50 PM
sorry, I just saw this. The references look good as they are. Though having the hardware references doesnt rlly make sense since you didn't write anything about them
lol I was determining whether or not I should send that message lol
Noreen — Yesterday at 8:51 PM
this question was for me 444 paper
Mustafa — Yesterday at 8:51 PM
oh
lol
but
why i stalkin me huh😬
Noreen — Yesterday at 8:51 PM
i had my discord open
Mustafa — Yesterday at 8:51 PM
this was u the last 10 mins
im gonna finish the IEEE paper
then knock out 
what are u working on?
Noreen — Yesterday at 9:45 PM
I just submitted my individual paper
Mustafa — Yesterday at 10:17 PM
same here
can u post some pictures of something from the software? Anything you think u can add would help because we're not even at 7 pages yet
Mustafa — Yesterday at 11:04 PM
imma be away for 15ish minutes. Just text if u need something🙂
﻿
#include <RH_ASK.h> // Include the RadioHead library for ASK modulation
#include <SPI.h> // Include the SPI library for communication with the RF module

RH_ASK driver(2000, 4, 2); // Initialize the RH_ASK driver with a bitrate of 2000bps and specify the data pin (4) and the chip select pin (2)

void setup() {
    Serial.begin(115200); // Initialize serial communication at a baud rate of 115200
    if (!driver.init()) { // Initialize the driver
        Serial.println("Initialization failed"); // Print a message if initialization fails
    }
}

void loop() {
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN]; // Create a buffer to store received data
    uint8_t buflen = sizeof(buf); // Get the size of the buffer

    if (driver.recv(buf, &buflen)) { // Check if a message is received
        if (buflen == 12 && strncmp((char*)buf, "controls on!", 12) == 0) { // Check if the received message is "controls on!"
            Serial.println("controls on!"); // Print "controls on!" if the message matches
        } else {
            // If it's not "controls on!", print the received message
            for (int i = 0; i < buflen; i++) {
                Serial.print((char)buf[i]); // Print each character of the message
            }
            Serial.println(); // Print a newline to separate messages
        }
        // Reset the flag when a message is received
        static bool printedOff = false; // Declare a static variable to track if "controls off" has been printed
        printedOff = false; // Reset the flag to indicate that a message has been received
    } else {
        // If no message is received, print "controls off!" once
        static bool printedOff = false; // Declare a static variable to track if "controls off" has been printed
        if (!printedOff) { // Check if "controls off" has not been printed yet
            Serial.println("controls off!"); // Print "controls off!" if it has not been printed yet
            printedOff = true; // Set the flag to indicate that "controls off" has been printed
        }
    }
}
receiver.txt
3 KB
