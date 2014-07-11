#pragma once
class IHook {
private:
	BOOL mIsHooked = FALSE; // whether the current hook is applied
protected:
	virtual void SetHooked(BOOL value) { mIsHooked = value; }
public:
	virtual ~IHook() { };
	virtual DWORD DoHook() = 0; // do the specific hook
	virtual DWORD DoUnhook() = 0; // unhook the specific hook
	virtual BOOL IsHooked() { return mIsHooked; }
};