import cv2
import mediapipe as mp
from threading import Thread, Lock
#import serial
import json

# Initialize MediaPipe Hands for each camera separately
mp_hands = mp.solutions.hands
hands0 = mp_hands.Hands()
hands1 = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils  # to draw landmarks

# Set up the serial connection to Arduino (update the port if necessary)
#arduino_serial = serial.Serial('COM3', 9600, timeout=1)  # Update port as needed

# Function to convert landmarks to a dictionary
def landmarks_to_dict(landmarks):
    landmark_dict = [{'x': l.x, 'y': l.y, 'z': l.z} for l in landmarks.landmark]
    return landmark_dict

# Video capture function for each camera
def capture_frames(camera_index, frame_queue, lock):
    cap = cv2.VideoCapture(camera_index)
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        with lock:  # Using a lock to safely update the frame in the queue
            frame_queue[camera_index] = frame
    cap.release()

# Start video capture for two cameras in separate threads
frame_queue = {}
lock = Lock()  # Lock object to protect the frame queue
threads = [
    Thread(target=capture_frames, args=(0, frame_queue, lock), daemon=True),
    Thread(target=capture_frames, args=(1, frame_queue, lock), daemon=True)
]
for t in threads:
    t.start()

try:
    while True:
        hand_landmarks_data = []  # This will store the landmarks of both hands
        with lock:  # Using a lock to safely access the frame queue
            if 0 in frame_queue and 1 in frame_queue:
                frame0 = frame_queue[0]
                frame1 = frame_queue[1]

                # Resize frames to the same size for display
                frame0 = cv2.resize(frame0, (640, 480))
                frame1 = cv2.resize(frame1, (640, 480))

                # Process frame from Camera 1 with its dedicated MediaPipe Hands instance
                results0 = hands0.process(cv2.cvtColor(frame0, cv2.COLOR_BGR2RGB))
                if results0.multi_hand_landmarks:
                    for hand_landmarks in results0.multi_hand_landmarks:
                        landmarks_dict = landmarks_to_dict(hand_landmarks)
                        hand_landmarks_data.append(landmarks_dict)
                        mp_drawing.draw_landmarks(frame0, hand_landmarks, mp_hands.HAND_CONNECTIONS)

                # Process frame from Camera 2 with its dedicated MediaPipe Hands instance
                results1 = hands1.process(cv2.cvtColor(frame1, cv2.COLOR_BGR2RGB))
                if results1.multi_hand_landmarks:
                    for hand_landmarks in results1.multi_hand_landmarks:
                        landmarks_dict = landmarks_to_dict(hand_landmarks)
                        hand_landmarks_data.append(landmarks_dict)
                        mp_drawing.draw_landmarks(frame1, hand_landmarks, mp_hands.HAND_CONNECTIONS)

                # Concatenate the two frames and display horizontally
                combined_frame = cv2.hconcat([frame0, frame1])

                # Display the combined frame
                cv2.imshow('Combined Video', combined_frame)

        # # Read and process data from Arduino
        # if arduino_serial.inWaiting() > 0:
        #     arduino_data = arduino_serial.readline().decode('utf-8').rstrip()
        #     print("Data from Arduino:", arduino_data)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    # Release resources and close windows
    #arduino_serial.close()  # Close the serial connection to Arduino
    for t in threads:
        t.join()  # Ensure threads are cleanly shut down
    cv2.destroyAllWindows()
