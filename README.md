Connecting the display enviorment on a personal device:

1. Install VSCode (DON'T USE ARDUINO IDE!)
2. Install PlatformIO exteniton in the extensions tab on the VSCode application 
3. Clone the project repository using the terminal command `git clone (insert link to this repository)`
    -  This will give you access to the LVGL graphics library the display uses + the code for the display components
4. Open the cloned project folder in VSCode 
5. Connect display using Type-C cable from "USB" port on the display to personal device
6. Navigate to PlatformIO extension (the alien icon on the left side of the screen) and notice three options: 
    > "Default", "Native", and "esp32-s3-devkitc-1"

    `esp32-s3-devkitc-1` and `Native` (when expanded) gives options to build, upload, and monitor code

    - esp32-s3-devkitc-1 options launch the code onto the display directly 
    - Native options launch the display enviorment onto personal device 

***Optional*** for testing on personal device:
      (install SDL2 drivers online to allow for native to work)
- On Mac: brew install sdl2
- On Windows: vcpkg install sdl2
