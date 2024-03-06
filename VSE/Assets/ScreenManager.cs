using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScreenManager : MonoBehaviour
{
    public GameObject virtualScreenPrefab; // Assign your VirtualScreen prefab

    // Call this method when you want to create a new VirtualScreen
    public void CreateNewMonitor(SnapPoint snapPoint)
    {
        GameObject newMonitor = Instantiate(virtualScreenPrefab);
        snapPoint.SnapTo(newMonitor); // Snap the new monitor to the provided snap point
    }
}

