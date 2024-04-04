# import cv2
# import mediapipe as mp
# from threading import Thread, Lock
# #import serial
# import json
# import time
# import socket

# # Initialize MediaPipe Hands for each camera separately
# mp_hands = mp.solutions.hands
# hands0 = mp_hands.Hands()
# hands1 = mp_hands.Hands()
# mp_drawing = mp.solutions.drawing_utils  # to draw landmarks

# # Function to convert landmarks to a dictionary
# def landmarks_to_dict(landmarks, handedness):
#     # Include handedness in the dictionary
#     handedness_class = handedness.classification[0].label
#     landmark_dict = {
#         'handedness': handedness_class,
#         'landmarks': [{'x': l.x, 'y': l.y, 'z': l.z} for l in landmarks.landmark]
#     }
#     return landmark_dict

# # Video capture function for each camera
# def capture_frames(camera_index, frame_queue, lock):
#     cap = cv2.VideoCapture(camera_index)
#     while True:
#         ret, frame = cap.read()
#         if not ret:
#             break
#         with lock:  # Using a lock to safely update the frame in the queue
#             frame_queue[camera_index] = frame
#     cap.release()

# # Initialize server details
# SERVER_HOST = "localhost"
# SERVER_PORT = 5005
# server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# server_socket.bind((SERVER_HOST, SERVER_PORT))
# server_socket.listen(1)
# print(f"Listening as {SERVER_HOST}:{SERVER_PORT} ...")
# client_socket, client_address = server_socket.accept()
# print(f"{client_address[0]}:{client_address[1]} Connected!")

# frame_queue = {}
# lock = Lock()  # Lock object to protect the frame queue
# threads = [
#     Thread(target=capture_frames, args=(0, frame_queue, lock), daemon=True),
#     Thread(target=capture_frames, args=(1, frame_queue, lock), daemon=True)
# ]
# for t in threads:
#     t.start()

# hand_landmarks_data = []  # This will store the landmarks of both hands

# try:
#     while True:
#         with lock:  # Using a lock to safely access the frame queue
#             if 0 in frame_queue and 1 in frame_queue:
#                 frame0 = frame_queue[0]
#                 frame1 = frame_queue[1]

#                 # Resize frames to the same size for display
#                 frame0 = cv2.resize(frame0, (640, 480))
#                 frame1 = cv2.resize(frame1, (640, 480))

#                 # Process frame from Camera 1 with its dedicated MediaPipe Hands instance
#                 if results0.multi_hand_landmarks:
#                     for idx, hand_landmarks in enumerate(results0.multi_hand_landmarks):
#                         handedness = results0.multi_handedness[idx]
#                         landmarks_dict = landmarks_to_dict(hand_landmarks, handedness)
#                         hand_landmarks_data.append(landmarks_dict)
#                         mp_drawing.draw_landmarks(frame0, hand_landmarks, mp_hands.HAND_CONNECTIONS)

#                 # Process frame from Camera 2 with its dedicated MediaPipe Hands instance
#                 results1 = hands1.process(cv2.cvtColor(frame1, cv2.COLOR_BGR2RGB))
#                 if results1.multi_hand_landmarks:
#                     for hand_landmarks in results1.multi_hand_landmarks:
#                         landmarks_dict = landmarks_to_dict(hand_landmarks)
#                         hand_landmarks_data.append(landmarks_dict)
#                         mp_drawing.draw_landmarks(frame1, hand_landmarks, mp_hands.HAND_CONNECTIONS)
                
#                 # Concatenate the two frames and display horizontally
#                 combined_frame = cv2.hconcat([frame0, frame1])

#                 # Display the combined frame
#                 cv2.imshow('Combined Video', combined_frame)

#         # Instead of printing, send the landmarks data to the Unity client
#         if hand_landmarks_data:
#             json_data = json.dumps(hand_landmarks_data) + '\n'  # Add a newline as a terminator
#             client_socket.sendall(json_data.encode('utf-8'))
#             hand_landmarks_data.clear()  # Clear the list after sending
        
#         # Sleep for 5 seconds before processing the next set of frames
#         time.sleep(0.02)

#         if cv2.waitKey(1) & 0xFF == ord('q'):
#             break

# finally:
#     for t in threads:
#         t.join()  # Ensure threads are cleanly shut down
#     cv2.destroyAllWindows()
#     client_socket.close()
#     server_socket.close()
import cv2
import mediapipe as mp
from threading import Thread, Lock
import json
import time
import socket

# Initialize MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=2)
mp_drawing = mp.solutions.drawing_utils  # Drawing utilities for landmarks

# Function to convert landmarks to a dictionary and include handedness
def landmarks_to_dict(landmarks, handedness):
    handedness_class = handedness.classification[0].label  # 'Left' or 'Right'
    landmark_dict = {
        'handedness': handedness_class,
        'landmarks': [{'x': l.x, 'y': l.y, 'z': l.z} for l in landmarks.landmark]
    }
    return landmark_dict

# Capture frames from the webcam
def capture_frames(camera_index, frame_queue, lock):
    cap = cv2.VideoCapture(camera_index)
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print(f"Unable to capture video from camera {camera_index}")
            break
        with lock:  # Use lock to safely update the frame in the queue
            frame_queue[camera_index] = frame

# Initialize server details
SERVER_HOST = "localhost"
SERVER_PORT = 5005
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((SERVER_HOST, SERVER_PORT))
server_socket.listen(1)
print(f"Listening as {SERVER_HOST}:{SERVER_PORT} ...")
client_socket, client_address = server_socket.accept()
print(f"{client_address[0]}:{client_address[1]} Connected!")

frame_queue = {}
lock = Lock()  # Lock for thread-safe operations on frame_queue
threads = [Thread(target=capture_frames, args=(0, frame_queue, lock), daemon=True),
           Thread(target=capture_frames, args=(1, frame_queue, lock), daemon=True)]

# Start the threads
for t in threads:
    t.start()

# Process and send landmark data
try:
    while True:
        with lock:
            if 0 in frame_queue:
                frame0 = frame_queue[0]
                frame0 = cv2.resize(frame0, (640, 480))
                results0 = hands.process(cv2.cvtColor(frame0, cv2.COLOR_BGR2RGB))
                hand_landmarks_data = []
                if results0.multi_hand_landmarks:
                    for idx, hand_landmarks in enumerate(results0.multi_hand_landmarks):
                        handedness = results0.multi_handedness[idx]
                        landmarks_dict = landmarks_to_dict(hand_landmarks, handedness)
                        hand_landmarks_data.append(landmarks_dict)
                        # Debug print each hand's data
                        print(json.dumps(landmarks_dict, indent=2))
                        mp_drawing.draw_landmarks(frame0, hand_landmarks, mp_hands.HAND_CONNECTIONS)
                    cv2.imshow('Frame 0', frame0)
                    # Prepare and send all collected hand landmarks data
                    json_data = json.dumps({"hands": hand_landmarks_data}) + '\n'  # Newline as a message terminator
                    print("Sending data to Unity:", json_data)  # Debug print before sending
                    client_socket.sendall(json_data.encode('utf-8'))
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        time.sleep(0.02)  # Add a short delay to avoid overloading the CPU

finally:
    for t in threads:
        t.join()  # Ensure threads are cleanly shut down
    cv2.destroyAllWindows()
    client_socket.close()
    server_socket.close()
