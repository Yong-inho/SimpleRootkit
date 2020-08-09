# Simple Linux Rootkit

## Properties    
### Can (un)hide module itself.    
Module itself will be hidden as soon as it is loaded.    
Module can be visible by giving a command : sudo ./controller unhide-module    
### Can (un)hide certain process by pid.   
sudo ./controller hide-proc {pid}    
sudo ./controller unhide-proc {pid}    
### Can (un)hide certain file by file path.    
sudo ./controller hide-file {file path}    
sudo ./controller unhide-file    
All files hidden will be visible if this command is given.   

## Kernel Version    
5.4.0-42-generic    

## Reference    
http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device    
https://github.com/f0rb1dd3n/Reptile    
https://www.codeproject.com/Articles/444995/Driver-to-hide-files-in-Linux-OS

