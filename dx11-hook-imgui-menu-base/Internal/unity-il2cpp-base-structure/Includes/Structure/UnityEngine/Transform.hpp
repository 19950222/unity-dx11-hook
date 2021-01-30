#pragma once

#include "Includes/Structure/UnityEngine/Vector3.hpp"
#include "Includes/Structure/UnityEngine/Quaternion.hpp"
#include "Includes/Utils/Macro.h"

namespace UnityEngine
{
	constexpr DWORD64 ofnGetPosition = 0x0;
	constexpr DWORD64 ofnGetRotation = 0x0;
	constexpr DWORD64 ofnGetChild = 0x0;
	constexpr DWORD64 ofnGetChildCount = 0x0;
	
	struct Transform
	{
		// System.Void UnityEngine.Transform::get_position_Injected(UnityEngine.Vector3&)
		typedef void fnGetPosition(Transform* target, Vector3* retPosition);

		// System.Void UnityEngine.Transform::get_rotation_Injected(UnityEngine.Quaternion&)
		typedef void fnGetRotation(Transform* target, Quaternion* retRotation);

		// UnityEngine.Transform UnityEngine.Transform::GetChild(System.Int32)
		typedef Transform* fnGetChild(Transform* target, int index);

		// System.Int32 UnityEngine.Transform::get_childCount()
		typedef int fnGetChildCount(Transform* target);

		void GetPosition(Vector3* retPosition)
		{
			reinterpret_cast<fnGetPosition*>(BASE_ADDRESS + ofnGetPosition)(this, retPosition);
		}

		void GetRotation(Quaternion* retQuaternion)
		{
			reinterpret_cast<fnGetRotation*>(BASE_ADDRESS + ofnGetRotation)(this, retQuaternion);
		}

		Transform* GetChild(const int index)
		{
			return reinterpret_cast<fnGetChild*>(BASE_ADDRESS + ofnGetChild)(this, index);
		}

		int GetChildCount()
		{
			return reinterpret_cast<fnGetChildCount*>(BASE_ADDRESS + ofnGetChildCount)(this);
		}
	};
}
