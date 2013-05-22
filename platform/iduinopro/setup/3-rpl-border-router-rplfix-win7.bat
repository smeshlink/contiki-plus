@echo off

netsh interface ipv6 show interface

set /p MSLOOPIF="Please select a interface: "

netsh interface ipv6 add neighbor interface=%MSLOOPIF% aaaa::21a:4c00:15d5:a8c2 33-33-ff-00-00-14

echo Done!