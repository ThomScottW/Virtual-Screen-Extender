using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SnapPoint : MonoBehaviour
{
    private bool enabled = true;
    public void SnapTo(GameObject VirtualScreen)
    {
        VirtualScreen.transform.position = transform.position;
        VirtualScreen.transform.rotation = transform.rotation;
        enabled = false;
    }

    
}
