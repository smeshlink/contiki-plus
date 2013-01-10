@echo off

netsh interface ipv6 show interface

set /p USBSTICKIF="Please select a interface: "

netsh interface ipv6 set interface interface=%USBSTICKIF% forwarding=enabled advertise=enabled
netsh interface ipv6 add address interface=%USBSTICKIF% address=aaaa::1/64
netsh interface ipv6 add route ::/0 interface=%USBSTICKIF% publish=yes
netsh interface ipv6 add route aaaa::/64 interface=%USBSTICKIF% publish=yes

echo Done!