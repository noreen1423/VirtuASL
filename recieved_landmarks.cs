
using UnityEngine;
using System.Diagnostics;
using System.IO;

public class HandLandmarkReader : MonoBehaviour
{
    private Process process;

    void Start()
    {
        StartPythonProcess();
    }

    void StartPythonProcess()
    {
        process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "python",
                Arguments = "path/to/your/python/script.py",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            }
        };
        process.Start();
        BeginRead();
    }

    async void BeginRead()
    {
        while (!process.StandardOutput.EndOfStream)
        {
            string line = await process.StandardOutput.ReadLineAsync();
            string[] parts = line.Split(',');
            if (parts.Length == 3)
            {
                float x = float.Parse(parts[0]);
                float y = float.Parse(parts[1]);
                float z = float.Parse(parts[2]);
                // Adjust the position based on your needs and coordinate system.
                // This example directly sets the local position.
                transform.localPosition = new Vector3(x, y, z);
            }
        }
    }

    void OnDestroy()
    {
        if (process != null)
        {
            process.Kill();
            process.Dispose();
        }
    }
}
