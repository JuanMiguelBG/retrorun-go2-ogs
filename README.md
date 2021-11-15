# retrorun-go2
Lightweight libretro frontend for ODROID-GO Super developed by OtherCrashOveride.

Build
======
```
git clone https://github.com/JuanMiguelBG/retrorun-go2.git
make
strip retrorun
```

The easiest way to build a 32bit version is to use Christians pre built virtual machine image with a chroot for armhf and arm64 https://forum.odroid.com/viewtopic.php?p=306185#p306185

Launch parameters:

* -s / --savedir : save directory, required string value, default ".".
* -d / --systemdir : system directory, required string value, default ".".
* -a / --aspect: aspect ratio, required float value, default 0.0. 
* -b / --backlight: backlight, required integer value, default -1.
* -v / --volume: volume value, required integer value, default -1.
* -r / --restart: restart retrorun, boolean value, default false.
* -t / --triggerright: enable right trigger button, boolean value, default false.
* -u / --triggerleft: enable left trigger button, boolean value, default false.
* -n / -analog: use analog stick, boolean value, default false.
* -l / --language_reicast: the languaje of the reicast core, required string value, default "Default".
* -w / --wide_reicast: wide screen on the reicast core, boolean value, default false.
* -x / --device: device type, required string value, default "rgb10maxtop".
