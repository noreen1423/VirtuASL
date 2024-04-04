using UnityEngine;
using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Concurrent;

public class HandDataReceiver : MonoBehaviour
{
    public string ip = "127.0.0.1";
    public int port = 5005;

    public Vector3 scale = new Vector3(10f, 10f, 10f);
    public Vector3 offset = new Vector3(0f, 0f, 0f);

    public Transform rightHandRoot;
    public Transform leftHandRoot;
    public Transform[] rightHandJoints;
    public Transform[] leftHandJoints;
    //public CustomHandController customHandController;


    [Serializable]
    public class HandData
    {
        public string handedness; // "Left" or "Right"
        public Landmark[] landmarks;
    }

    [Serializable]
    public class Landmark
    {
        public float x;
        public float y;
        public float z;
    }

    [Serializable]
    public class HandsLandmarksData
    {
        public HandData[] hands; // Array to hold data for both hands
    }

    private TcpClient client;
    private NetworkStream stream;
    private StreamReader reader;
    private Thread networkThread;
    private ConcurrentQueue<string> dataQueue = new ConcurrentQueue<string>();
    private bool stopThread = false;

    void Start()
    {
        networkThread = new Thread(NetworkThread);
        networkThread.IsBackground = true;
        networkThread.Start();
    }

    void Update()
    {
        // Process all available data in the queue
        while (dataQueue.TryDequeue(out string data))
        {
            ProcessReceivedData(data);
        }
    }

    private void NetworkThread()
    {
        try
        {
            client = new TcpClient(ip, port);
            stream = client.GetStream();
            reader = new StreamReader(stream);
            Debug.Log("Connected to server.");

            while (!stopThread && client.Connected)
            {
                if (stream.DataAvailable)
                {
                    string data = reader.ReadLine();
                    if (!string.IsNullOrEmpty(data))
                    {
                        dataQueue.Enqueue(data);
                        Debug.Log("Data received: " + data);//Debug

                    }
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogError("Socket error: " + e.Message);
        }
    }

    private void ProcessReceivedData(string data)
    {
        try
        {
            var handsData = JsonUtility.FromJson<HandsLandmarksData>(data);
            Debug.Log("Processing hand data: " + handsData.hands[0].handedness);

            // Use the landmarks and handedness to update the hand model positions
            foreach (var hand in handsData.hands)
            {
                if(hand.handedness == "Right")
                {
                    UpdateHandModel(hand.landmarks, rightHandJoints, rightHandRoot);
                }
                else if(hand.handedness == "Left")
                {
                    UpdateHandModel(hand.landmarks, leftHandJoints, leftHandRoot);
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogError("Error processing received data: " + e.Message);
        }
    }

    private void UpdateHandModel(Landmark[] landmarks, Transform[] handJoints, Transform handRoot)
    {
        for (int i = 0; i < landmarks.Length && i < handJoints.Length; i++)
        {
            Vector3 localPosition = new Vector3(landmarks[i].x, landmarks[i].y, landmarks[i].z) - new Vector3(0.5f, 0.5f, 0.5f);
            Vector3 adjustedPosition = Vector3.Scale(localPosition, scale) + offset;
            handJoints[i].localPosition = adjustedPosition;
        }
    }

    private void OnApplicationQuit()
    {
        stopThread = true;
        reader?.Close();
        stream?.Close();
        client?.Close();
        networkThread?.Join();
    }
}