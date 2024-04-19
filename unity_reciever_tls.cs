using UnityEngine;
using System;
using System.IO;
using System.Net.Sockets;
using System.Threading;
using System.Collections.Concurrent;
using System.Net.Security;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;

public class HandDataReceiver : MonoBehaviour
{
    public string ip = "localhost";
    public int port = 5005;

    public Vector3 scale = new Vector3(1f, 1f, 1f);
    public Vector3 offset = new Vector3(0f, 0f, 0f);

    public Transform rightHandRoot;
    public Transform leftHandRoot;
    public Transform[] rightHandJoints;
    public Transform[] leftHandJoints;
    public LineRenderer rightHandLineRenderer;
    public LineRenderer leftHandLineRenderer;

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
    private SslStream sslStream;
    private StreamReader reader;
    private Thread networkThread;
    private ConcurrentQueue<string> dataQueue = new ConcurrentQueue<string>();
    private bool stopThread = false;

    private int[] path = {0,1,2,3,4,3,2,5,6,7,8,7,6,5,9,10,11,12,11,10,9,13,14,15,16,15,14,13,17,18,19,20,19,18,17,0};

    void Start()
    {
        networkThread = new Thread(NetworkThread);
        networkThread.IsBackground = true;
        networkThread.Start();

        SetupLineRenderers();
        // Initialize line renderer
    }

    void SetupLineRenderers()
    {
        rightHandLineRenderer.positionCount = path.Length;
        rightHandLineRenderer.startWidth = 0.01f;
        rightHandLineRenderer.endWidth = 0.01f;

        leftHandLineRenderer.positionCount = path.Length;
        leftHandLineRenderer.startWidth = 0.01f;
        leftHandLineRenderer.endWidth = 0.01f;
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
            // Wrap the client's stream with an SSL stream.
            sslStream = new SslStream(client.GetStream(), false, new RemoteCertificateValidationCallback(ValidateServerCertificate), null);
            sslStream.AuthenticateAsClient(ip);
            reader = new StreamReader(sslStream);

            Debug.Log("Connected to server with SSL.");
            while (!stopThread && client.Connected)
            {
                if (sslStream.CanRead)
                {
                    string data = reader.ReadLine();
                    if (!string.IsNullOrEmpty(data))
                    {
                        dataQueue.Enqueue(data);
                        Debug.Log("Data received: " + data);
                    }
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogError("SSL socket error: " + e.Message);
        }
    }
    // Validate the server certificate by returning true if the certificate is valid.
    private bool ValidateServerCertificate(object sender, X509Certificate certificate, X509Chain chain, SslPolicyErrors sslPolicyErrors)
    {
        if (sslPolicyErrors == SslPolicyErrors.None)
            return true;
        return false;
    }

    private void ProcessReceivedData(string data)
    {
        try
        {
            var handsData = JsonUtility.FromJson<HandsLandmarksData>(data);
            Debug.Log("Processing hand data: " + handsData.hands[0].handedness);

            foreach (var hand in handsData.hands)
            {
                if (hand.handedness == "Right")
                {
                    UpdateHandModel(hand.landmarks, rightHandJoints, rightHandRoot, rightHandLineRenderer);
                }
                else if (hand.handedness == "Left")
                {
                    UpdateHandModel(hand.landmarks, leftHandJoints, leftHandRoot, leftHandLineRenderer);
                }
            }
        }
        catch (Exception e)
        {
            Debug.LogError("Error processing received data: " + e.Message);
        }
    }

    private void UpdateHandModel(Landmark[] landmarks, Transform[] handJoints, Transform handRoot, LineRenderer lineRenderer)
    {
        if (landmarks.Length != handJoints.Length)
        {
            Debug.LogError("Landmark and joint array lengths do not match.");
            return;
        }

        for (int i = 0; i < landmarks.Length; i++)
        {
            Vector3 position = new Vector3(landmarks[i].x, landmarks[i].y, landmarks[i].z);
            handJoints[i].localPosition = Vector3.Scale(position, scale) + offset;

            // Apply rotation
            if (i > 0)
            {
                Vector3 direction = handJoints[i - 1].position - handJoints[i].position;
                handJoints[i].rotation = Quaternion.LookRotation(direction, Vector3.up);
            }
        }
        //render line according to path[]
        for (int j=0; j<path.Length; j++){
            lineRenderer.SetPosition(j, handJoints[path[j]].position);
        }
        
    }

    private void OnApplicationQuit()
    {
        stopThread = true;
        reader?.Close();
        sslStream?.Close();
        client?.Close();
        networkThread?.Join();
    }

}
