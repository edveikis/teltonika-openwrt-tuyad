## Task

Connecting the router to the Tuya IoT cloud

This task is intended to help consolidate your acquired theoretical knowledge in practice. It will also help you better to understand the structure of the OpenWRT system, create new packages for it and apply new software.

For this task you will need to change your Tuya IoT daemon program so that it would work on the RutOS system. 

--------------------------------------------------------------------------------------------------------------------------------------------------------------------

Task result:

    A daemon type program has been prepared that will send data to the Tuya IoT cloud.
    Founded library or SDK that will be used to communicate with the Tuya IoT cloud. The library/SDK is prepared as a separate package.

--------------------------------------------------------------------------------------------------------------------------------------------------------------------

A total of two different packages must be prepared:

    A daemon type program is responsible for communicating with Tuya IoT cloud
    A library/SDK which will be used to communicate and send data to Tuya IoT cloud

--------------------------------------------------------------------------------------------------------------------------------------------------------------------

Requirements for the task:

    The library/SDK code must be automatically downloaded from the remote server where the library/SDK was found. 
    In the OpenWRT system, the package directory must not contain this library/SDK code. 
    Code is not considered a Makefile, shell and lua scripts, patches or configuration files.
    The ubus system must be used in the daemon program to obtain data about the router. 
    You should gather and send the same data as in the previous task. If some data is missing or unable to get it from the ubus, just leave as it is. 
    The daemon program must write messages to the log system, whose messages can be viewed by executing the logread command in the router's command line. The messages should be clear and informative, allowing the user to understand about the successful and erroneous operation of the program.

    A minimal API endpoint to control your program must be implemented. Don't implement front-end with JavaScript.
    In directory vuci-examples (which is located in the root directory of the OpenWRT project) provides an example of the structure and some code examples.
    More detailed documentation on how to create an API can be found here.
    Here you can find information how you can test the API using Postman.
    If you have any questions about the implementation, just ask your internship manager/coordinator.

