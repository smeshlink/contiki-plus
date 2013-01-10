@echo off

netsh interface ipv6 show interface

set /p USBSTICKIF="Please select a interface: "

ipv6 ifc %USBSTICKIF% advertises forwards
ipv6 adu %USBSTICKIF%/aaaa::1
ipv6 rtu aaaa::/64 %USBSTICKIF% publish

echo Done!