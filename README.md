# wlan_station_exosite_write_read
 * TI CC2640R2F Sensortag using TI CC2640R2F Launchpad + Educational BoosterPack MKII
 *
 * Maker/Author - Markel T. Robregado
 *
 * Modification Details : CC2640R2F Launchpad with SensorTag and Key Fob codes 
 *                        ported from BLE Stack 2.2.1.
 *                        
 * Device Setup: TI CC2640R2F Launchpad + Educational BoosterPack MKII
 *
 */

Need to do:

1. If at TI BLE Device Monitor, SensorTag IO and SensorTag Register does not appear as 
   Services, copy gatt_uuid.xml from TI BLE Device Monitor folder and save it at 
   "C:\Program Files (x86)\Texas Instruments\BLE Device Monitor". Overwrite the current
   gatt_uuid.xml.
   
2. As of this code publishment, the SensorTag App crashes when going to Sensor View. 
   However, the SensorTag App does not crash going to Services.
   
3. If your device setup is TI CC2640R2F Launchpad and TI Educational BoosterPack MKII, 
   you can get the app and stack hex files at "CC2640R2 LP EDU BP MKII Hex Files" folder.
   At CCS set predefined symbols "SENSORTAG_CC2640R2_LAUNCHXL" and "Board_BUZZER"
   
4. If your device setup is TI CC2640R2F Launchpad, you can get the app and stack hex files
   at CC2640R2 LP Hex Files. At CCS set predefined symbol "CC2640R2_LAUNCHXL
   

