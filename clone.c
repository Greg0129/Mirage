// clone.c - Clone keyboard and mouse devices

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>

// Target devices to clone
// eg.
  // "/dev/input/event2" // Keyboard
  // "/dev/input/event6" // Mouse

int create_physical_keyboard(const char* path) {
  int physicalKeyboard = open(path, O_RDONLY);
  if (physicalKeyboard < 0) return 1;
  if (ioctl(physicalKeyboard, EVIOCGRAB, 1) < 0) return 1;
  return physicalKeyboard;
}

int create_physical_mouse(const char* path) {
  int physicalMouse = open(path, O_RDONLY);
  if (physicalMouse < 0) return 1;
  if (ioctl(physicalMouse, EVIOCGRAB, 1) < 0) return 1;
  return physicalMouse;
}

int create_virtual_keyboard(const char* keyboardName) {
	int virtualKeyboard = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (virtualKeyboard < 0) return 1;
	ioctl(virtualKeyboard, UI_SET_EVBIT, EV_KEY);
	ioctl(virtualKeyboard, UI_SET_EVBIT, EV_SYN);
	for (int i = 0; i < KEY_MAX; i++) {
		ioctl(virtualKeyboard, UI_SET_KEYBIT, i);
	}
	struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));

	snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, keyboardName);
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor  = 0x1234;
	usetup.id.product = 0x5678;
	usetup.id.version = 1;

	ioctl(virtualKeyboard, UI_DEV_SETUP, &usetup);
	ioctl(virtualKeyboard, UI_DEV_CREATE);

	usleep(100000);

	return virtualKeyboard;
}

int create_virtual_mouse(const char* mouseName) {
  int virtualMouse = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (virtualMouse < 0) return 1;

  ioctl(virtualMouse, UI_SET_EVBIT, EV_KEY);
  ioctl(virtualMouse, UI_SET_EVBIT, EV_REL);
  
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_LEFT);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_RIGHT);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_MIDDLE);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_SIDE);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_EXTRA);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_FORWARD);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_BACK);
  ioctl(virtualMouse, UI_SET_KEYBIT, BTN_TASK);
  
  ioctl(virtualMouse, UI_SET_RELBIT, REL_X);
  ioctl(virtualMouse, UI_SET_RELBIT, REL_Y);
  ioctl(virtualMouse, UI_SET_RELBIT, REL_WHEEL);
  ioctl(virtualMouse, UI_SET_RELBIT, REL_HWHEEL);

  struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));

	snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, mouseName);
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor  = 0x1234;
	usetup.id.product = 0x5678;
	usetup.id.version = 1;

	ioctl(virtualMouse, UI_DEV_SETUP, &usetup);
	ioctl(virtualMouse, UI_DEV_CREATE);

	usleep(100000);

	return virtualMouse;
}

void destroy_physical_keyboard(int physicalKeyboard) {
  close(physicalKeyboard);
}

void destroy_physical_mouse(int physicalMouse) {
  close(physicalMouse);
}

void destroy_virtual_keyboard(int virtualKeyboard) {
	close(virtualKeyboard);
}

void destroy_virtual_mouse(int virtualMouse) {
  close(virtualMouse);
}



int main() {
	int virtualKeyboard1 = create_virtual_keyboard("VirtualKeyboard1");
  int virtualKeyboard2 = create_virtual_keyboard("VirtualKeyboard2");

  int physicalKeyboard = create_physical_keyboard("/dev/input/event2");

	struct input_event ev;
	while (1) {
		ssize_t n = read(physicalKeyboard, &ev, sizeof(ev));
		if (n != sizeof(ev)) continue;

		write(virtualKeyboard1, &ev, sizeof(ev));
    write(virtualKeyboard2, &ev, sizeof(ev));
	}

  destroy_physical_keyboard(physicalKeyboard);
	destroy_virtual_keyboard(virtualKeyboard1);
}
