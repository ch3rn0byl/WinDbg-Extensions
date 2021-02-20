# WinDbg-Extensions
## Callback Extension
Callbacks.dll is a WinDbg extension that will enumerate all pointers inside of the following symbols for callbacks that are registered on the system:
1. nt!PspCreateProcessNotifyRoutine
2. nt!PspCreateThreadNotifyRoutine
3. nt!PspLoadImageNotifyRoutine

![picture](https://github.com/ch3rn0byl/WinDbg-Extensions/blob/main/Images/all.PNG)

## How to use?
The extension takes in either of the four parameters: _process_, _image_, _thread_, or _all_. 
This just makes life a little bit easier when you're doing whatever it is you're doing with them.  

This just lists the callbacks, nothing more nothing less but as I continue working on my shenanigans...I will make sure to continue adding on to this.

Have fun!
