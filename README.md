# MiniUPNP Client


This is a small fork of the miniupnp library created by Thomas Bernard.
The code of the library was changed with his consent, but Thomas consider
it to be disruptive to be merged with miniupnp library.

## Why?

I got one of those at the time nice Speedtouch 546 ADSL routers already pre-configured by
Portugal Telecom (PTPrime actually). Not having a login and password for it,
i was unable preform simple tasks such as verify the connection status,
view byte count statistics or add port forward rules.

I found miniupnp and created a command line tool capable of
managing every aspect allowed by the router. The result is the upnpc.c file and couple of
changes to the miniupnpc library. I am able to invoke any method
exported by my devices. However, please consider that your mileage may vary.

## Main changes of this version

* A new client is provided which eventually supports any action and IGD devices. Thomas initial libray only supported a small set of functions.
* Devices, services and actions are not hard coded but downloaded from the devices at runtime. This required changing the parsing methods to support dynamic memory together with a different structure. The benefit is even if devices export non-standard methods, this client hopefully still can invoke them. Also, if someone wishes to invoke a method, it is not required to add the corresponding function to libminiupnpc. The drawback is a slightly more complicated scheme and higher memory usage.

## Using upnpc

You should first list (-l) the services, actions and arguments supported by your IGD device.

Then you can invoke any command following the syntax: 

    ./upnpc -a device:service:action arg1,arg2,arg3

One example would be: 

    ./upnpc -a 0:3:2 80,TCP,1922.168.0.1,80

The actual parameters and values will depend on the device. Using -l you will have a list of
the parameters you can use.


I will keep updating this code as i find necessary. Still, as my problem is solved,
this code may stall until i require some additional feature, or someone submits a patch.

## Contact
Any question regarding miniupnp, please contact Thomas Bernard at http://miniupnp.free.fr

If the questions are related to this specific version, you may contact me.
