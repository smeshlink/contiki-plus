@echo off

netsh interface ipv6 show interface

set /p USBSTICKIF="Please select a interface: "

netsh interface ipv6 add neighbor interface=%USBSTICKIF% aaaa::11:22ff:fe33:4433 33-33-ff-00-00-14

echo Done!