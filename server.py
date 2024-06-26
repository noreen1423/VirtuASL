import cv2
import mediapipe as mp
from threading import Thread, Lock
import json
import time
import socket
import ssl
import serial

# Initialize MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=2)
mp_drawing = mp.solutions.drawing_utils

# Set up the serial connection to Arduino (update the port if necessary)
arduino_serial = serial.Serial('COM5', 115200, timeout=1)  # Update port as needed

# Flag to indicate whether hand tracking is active
tracking_active = True

# Timestamp for the last received data from Arduino
last_arduino_data_time = time.time()

# Time interval (in seconds) to wait before turning tracking back on
tracking_reenable_interval = 5  # Adjust as needed

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
            if tracking_active:
                frame_queue[camera_index] = frame
    cap.release()

# Initialize server details
SERVER_HOST = "localhost"
SERVER_PORT = 5005
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((SERVER_HOST, SERVER_PORT))
server_socket.listen(1)
print(f"Listening as {SERVER_HOST}:{SERVER_PORT} ...")

# Wrap the server socket with SSL
context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.load_cert_chain(certfile='C:/Users/Spide/Downloads/Noreen_certificate.crt', keyfile='C:/Users/Spide/Downloads/Noreen_private_key.key')
wrapped_socket = context.wrap_socket(server_socket, server_side=True)

# Accept a new connection
print('Waiting for a connection')
client_socket, client_address = wrapped_socket.accept()
print(f"Connection from {client_address}")

frame_queue = {}
lock = Lock()  # Lock for thread-safe operations on frame_queue
threads = [Thread(target=capture_frames, args=(0, frame_queue, lock), daemon=True)]

# Start the threads
for t in threads:
    t.start()

# Process and send landmark data
try:
    while True:
        with lock:
            print(tracking_active)
            if 0 in frame_queue and tracking_active:
                frame0 = frame_queue[0]
                frame0 = cv2.resize(frame0, (640, 480))
                results0 = hands.process(cv2.cvtColor(frame0, cv2.COLOR_BGR2RGB))
                hand_landmarks_data = []
                if results0.multi_hand_landmarks:
                    for idx, hand_landmarks in enumerate(results0.multi_hand_landmarks):
                        handedness = results0.multi_handedness[idx]
                        landmarks_dict = landmarks_to_dict(hand_landmarks, handedness)
                        hand_landmarks_data.append(landmarks_dict)
                    json_data = json.dumps({"hands": hand_landmarks_data}) + '\n'
                    client_socket.sendall(json_data.encode('utf-8'))
                
        # Read and process data from Arduino
        if arduino_serial.inWaiting() > 0:
            arduino_data = arduino_serial.readline().decode('utf-8').rstrip()
            print(arduino_data)
            if (arduino_data == "Controls On!"):
                tracking_active = False
                last_arduino_data_time = time.time()
        
        # Check if it's time to re-enable tracking
        if not tracking_active and time.time() - last_arduino_data_time > tracking_reenable_interval:
            tracking_active = True

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        time.sleep(0.02)  # Throttle the loop

finally:
    for t in threads:
        t.join()
    cv2.destroyAllWindows()
    client_socket.close()
    server_socket.close()
    arduino_serial.close()  # Close the serial connection to Arduino
