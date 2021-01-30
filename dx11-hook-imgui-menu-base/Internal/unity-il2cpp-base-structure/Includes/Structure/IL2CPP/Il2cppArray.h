#pragma once

namespace IL2CPP {
	template <class T>
	struct Il2CppArray
	{
		char padding_0[0x18];
		int length;

		T GetItem(int index)
		{
			return *(T*)((DWORD64)&padding_0 + (sizeof(T) * index) + 0x20);
		}

		void SetItem(int index, T item)
		{
			*(T*)((DWORD64)&padding_0 + (sizeof(T) * index) + 0x20) = item;
		}
	};
}