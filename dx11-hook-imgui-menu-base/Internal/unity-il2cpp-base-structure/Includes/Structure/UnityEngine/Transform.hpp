#pragma once

#include "Includes/Structure/UnityEngine/Vector3.hpp"
#include "Includes/Structure/UnityEngine/Quaternion.hpp"

namespace UnityEngine
{
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
	};
}
