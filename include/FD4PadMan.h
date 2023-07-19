#pragma once
#include <cstdint>
#include "Global.h"
#include "Pointer.h"

using namespace EW;

class FD4PadMan {
public:
	enum Button {
		// controller
		A,
		B,

		// keyboard
		E,
		Space,
	} ButtonInput;

	struct PressedButton {
		int32_t pressedButton[8];
	};


	static void RegisterDownButtons();
	static bool IsButtonDown(Button button);
	static bool IsButtonUp(Button button);
	static bool IsButtonPressed(Button button);

private:
	// controller
	static bool A_down;
	static bool B_down;

	// keyboard
	static bool E_down;
	static bool Space_down;
};

inline bool FD4PadMan::A_down;
inline bool FD4PadMan::B_down;
inline bool FD4PadMan::E_down;
inline bool FD4PadMan::Space_down;

// Buttons will be added as needed
inline void FD4PadMan::RegisterDownButtons() {
	PressedButton* pb = AccessDeepPtr<PressedButton>(Global::FD4PAD_MAN.ptr(), 0x18, 0x8, 0x7C8, 0x10);

	if (pb == nullptr) {
		printf("Something went wrong. Couldn't get pressed button!");
		return;
	}

	for (int i = 0; i < 7; i++) {
		std::cout << "0: " << pb->pressedButton[0] << std::endl;
		std::cout << "1: " << pb->pressedButton[1] << std::endl;
		std::cout << "2: " << pb->pressedButton[2] << std::endl;
		std::cout << "3: " << pb->pressedButton[3] << std::endl;
		std::cout << "4: " << pb->pressedButton[4] << std::endl;
		std::cout << "5: " << pb->pressedButton[5] << std::endl;
		std::cout << "6: " << pb->pressedButton[6] << std::endl;
		std::cout << "7: " << pb->pressedButton[7] << std::endl;
	}

	A_down     = (pb->pressedButton[4] == 0x00014000 && pb->pressedButton[7] == 0x02000000);
	B_down     = (pb->pressedButton[4] == 0x00041000 && pb->pressedButton[7] == 0x00800000);
	E_down	   = (pb->pressedButton[3] == 0x00010000 && pb->pressedButton[6] == 0x08000000);
	Space_down = (pb->pressedButton[4] == 0x00001000 && pb->pressedButton[7] == 0x00800000);

}

inline bool FD4PadMan::IsButtonDown(Button button) {
	PressedButton* pb = AccessDeepPtr<PressedButton>(Global::FD4PAD_MAN.ptr(), 0x18, 0x8, 0x7C8, 0x10);

	if (pb == nullptr) {
		printf("Something went wrong. Couldn't get pressed button!");
		return false;
	}

	if (button == A)	 { return pb->pressedButton[4] == 0x00014000 && pb->pressedButton[7] == 0x02000000; }
	if (button == B)	 { return pb->pressedButton[4] == 0x00041000 && pb->pressedButton[7] == 0x00800000; }
	if (button == E)	 { return pb->pressedButton[3] == 0x00010000 && pb->pressedButton[6] == 0x08000000; }
	if (button == Space) { return pb->pressedButton[4] == 0x00001000 && pb->pressedButton[7] == 0x00800000; }
}

inline bool FD4PadMan::IsButtonUp(Button button) {
	return IsButtonDown(button) == false;
}

inline bool FD4PadMan::IsButtonPressed(Button button) {
	if (button == Button::A)	 return A_down && IsButtonUp(Button::A);
	if (button == Button::B)	 return B_down && IsButtonUp(Button::B);
	if (button == Button::E)	 return E_down && IsButtonUp(Button::E);
	if (button == Button::Space) return Space_down && IsButtonUp(Button::Space);
}