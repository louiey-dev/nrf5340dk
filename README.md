# nrf5340dk
Test code on nrf5340dk board

## Feature
- GPIO control via App
- Sensors read and push to App
- Display LCD
- Buzzer via PWM

## Environment
- EVM : nrf5340dk
- IDE : VS Code
- SDK : v3.0.2
- Toolchain : 0b393f9e1b ??
- Daughter Board : SeedStudio Xiao Expansion Board


## History
- 2025.07.09
  - Created workspace with simple blink sample code
  - CMakeLists.txt modified to include bsp folder path
  - Creates bsp folder and added gpio code here
- 2025.07.17
  - added branch, lbs_porting
  - lbs function added (copied from samples)
    - using prj_minimal.conf
  - lbs_porting : preparing basic read/write function working version
- 