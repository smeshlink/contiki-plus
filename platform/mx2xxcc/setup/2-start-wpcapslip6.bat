@echo off
for /f "tokens=5" %%c in ('getmac /V ^| grep "Loop"') do wpcapslip6\wpcapslip6 -s COM2 -B 38400 -b aaaa:: -a aaaa::1/128 %%c
rem wpcapslip6\wpcapslip6 -s COM2 -B 38400 -b aaaa:: -a aaaa::1/128 02-00-4c-4f-4f-50
