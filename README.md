# Mirage
Clone input to multiple program instances in X11.

1:
  Create multiple virtual devices.
  Forward input from a physical device to these virtual devices.

2:
  Launch multiple nested X servers with Xephyr.
  Allocate location/s on monitor/s.
  Allocate a virtual device per nested X server.

3:
  Use primary display to control each instance simultaneously.
