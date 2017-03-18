# invoke SourceDir generated makefile for app_ble.pem3
app_ble.pem3: .libraries,app_ble.pem3
.libraries,app_ble.pem3: package/cfg/app_ble_pem3.xdl
	$(MAKE) -f C:\ti\simplelink_cc2640r2_sdk_1_00_00_22\examples\rtos\CC2640R2_LAUNCHXL\blestack\simple_peripheral\tirtos\ccs\config/src/makefile.libs

clean::
	$(MAKE) -f C:\ti\simplelink_cc2640r2_sdk_1_00_00_22\examples\rtos\CC2640R2_LAUNCHXL\blestack\simple_peripheral\tirtos\ccs\config/src/makefile.libs clean

