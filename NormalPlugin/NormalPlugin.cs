
using AppDomainTest;
using System;

namespace Example
{
    public class NormalPlugin : AppDomainTest.IPlugin
    {
        public string Execute(string owner, string data)
        {
            string newData = data.ToUpper();
            Console.WriteLine($"Executed normal plugin with {owner}");
            return newData;
        }
    }
}