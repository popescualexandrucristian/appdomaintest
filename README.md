# AppDomain test

This is a small proof of concept on using AppDomains to create a sand boxed environment for .net assemblies. 
In scenarios where c++/CLI is used. Also wanted to test CMake for c# and for c++/CLI.

## What

The project contains a client that reads from it's local environment some managed DLLs serializes their data and sends it to a server. 
The server creates a sandbox in a new restricted app domain and asks it to instantiate and call the serialized assemblies from the client.
In this code the client sends 3 assemblies (NormalPlugin, SlowPlugin and BadPlugin). 
BadPlugin tries and fails to call a console command and slow plugin goes in an infinite loop. Any plugin that tries to do any action not covered by the restricted app domain in a given time (hardcoded at 3 seconds for this test) will get an exception.

## How
* Generate the projects with CMake (3.14 or greater)
* Start the server with the port where it will listen as argument (if you start it from the a standard IDE it should be configured to start on the target port 666 )
* Start the client, make sure that the plugins are in the working directory under Plugins/[solution configuration] ex. Plugins/Debug. Start it with the command arguments IP PORT. If the server was started from an IDE it should be on port 666, so if you have the client on the same machine call it with  "127.0.0.1 666"
* When you start the server it will wait in a loop for client data.
* When a client is instantiate it will send the bytes from the plugin assemblies to the server.
* The server will create a restricted AppDomain instantiate and run the client assemblies data in and print some data to the console. Ex console output :
```
Data state : some data sent to the server
Oh no BadPlugin.dll from [::ffff:127.0.0.1]:57856 is bad !!
System.Security.SecurityException: Request failed.
   at Example.NormalPlugin.Execute(IContext context)
   at AppDomainTest.Sandbox.ExecuteUntrustedCode(IContext context)
The action that failed was:
LinkDemand
The type of the first permission that failed was:
System.Security.PermissionSet
The Zone of the assembly that failed was:
MyComputer
A call from the plugin "normal plugin"(00000270EF5ADB70, 000000DD075FA9C0) was made to the c++/CLI land from a sandboxed plugin.
A call from the plugin "NORMAL PLUGIN"(00000270EF5AD850, 000000DD075FA9C0) was made to the c++/CLI land from a sandboxed plugin.
Executed normal plugin with [::ffff:127.0.0.1]:57856
Started slow plugin execution with [::ffff:127.0.0.1]:57856
Data state : SOME DATA SENT TO THE SERVER
Oh no SlowPlugin.dll from [::ffff:127.0.0.1]:57856 is bad !!
System.Threading.ThreadAbortException: Thread was being aborted.
   at Example.NormalPlugin.Execute(IContext context)
   at AppDomainTest.Sandbox.ExecuteUntrustedCode(IContext context)
System.Threading.ThreadAbortException: Thread was being aborted.
   at AppDomainTest.Sandbox.ExecuteUntrustedCode(IContext context)
```
##
[Licensed under MIT](MIT-LICENSE.txt)

