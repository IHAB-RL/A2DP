BC127_to_A2DP
===============
Contains Arduino Code for sending stereo audio signal via Bluetooth A2DP to an Android Smartphone (Android App like in "AndroidStudio_Receive_A2DP_Simple_Example.zip" is neccessary)

AndroidStudio_Receive_A2DP_Simple_Example.zip
====================================================
Simple Android App to demonstrate receiving stereo audio via Bluetooth A2DP

AOSP - Custom Android Firmware HowTo
====================================
DISCLAIMER: Carrying out the steps provided could cause damage to the Nexus 5 device. The authors of this article will not be held responsible for any losses or damages of any type caused by following the steps below.

(This method is not limited to Android 4.4. Newer kernel versions with the car-hammerhead feature are possible.)

Technical requirements:
A PC with 64 bit with Debian-based Linux OS is recommended. Furthermore, a Nexus 5 device, with unlocked bootloader (and for best experience rooted) is provided.

Set up the environment:
Installed Android SDK tools, libs, python, Java, gcc, make and git are necessary.
- download the drivers for the Nexus 5
- download and sync the android 4.4.3 source
- choose the car-hammerhead build target- 
start the compile process with  ́make ́

Once the compiling has been completed, connect the phone using an USB cable to the PC and be sure that USB debugging is enabled.

- reboot the phone to bootloader mode
- choose the new boot image
- boot it again

The Android-based smartphone with A2DP-sink function is ready to use.

For more details see: 
- Forum XDA, https://forum.xda-developers.com/google-nexus-5/general/guide-how-to-build-aosp-4-4-3-source-t2703036 (13.03.2018)
- Blog,Csdn.net,http://blog.csdn.net/wendell_gong/article/details/47950781 (13.03.2018)
