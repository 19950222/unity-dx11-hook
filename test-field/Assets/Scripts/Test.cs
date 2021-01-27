using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using AOT;
using UnityEngine.UI;

public class Test : MonoBehaviour
{
	public delegate void TestDelegate();
 
	[DllImport("LdrRegisterDllNotificationExample.dll")]
	public static extern void _UnmanagedTest(TestDelegate callback);
	
	[DllImport("LdrRegisterDllNotificationExample.dll")]
	public static extern void Call();	
	[DllImport("LdrRegisterDllNotificationExample.dll")]
	public static extern void Register();
	
	[MonoPInvokeCallback(typeof(TestDelegate))]
	private static void ManagedTest()
	{
		detected = true;
	}

	private  static bool detected;
 
	private void Update()
	{
		if (detected)
		{
			FindObjectOfType<Text>().text = $"Detected! {value}";	
		}
		else
		{
			FindObjectOfType<Text>().text = $"{value}";
		}
	}

	private void Start()
	{
		_UnmanagedTest(ManagedTest);
		Register();
		
		FindObjectOfType<Button>().onClick.AddListener(OnClick);
	}

	public ObscuredInt value = 0;

	private void OnClick()
	{
		value += Random.Range(1, 16);
	}

	public static void OnDetect()
	{
		FindObjectOfType<Text>().color = Color.red;
	}
}