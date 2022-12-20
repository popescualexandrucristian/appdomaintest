
using AppDomainTest;
using System;

namespace Example
{
    public class NormalPlugin : AppDomainTest.IPlugin
    {
        public void Execute(IContext context)
        {
            System.Diagnostics.Process.Start("CMD.exe", "echo EVIL");
            Console.WriteLine($"Executed bad plugin with {context.Owner}");
        }
    }
}