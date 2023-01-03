
using AppDomainTest;
using System;

namespace Example
{
   public class NormalPlugin : IPlugin
   {
      public void Execute(IContext context)
      {
         string info = "bad plugin";
         context.Call(ref info);
         System.Threading.Thread.Sleep(10);
         System.Diagnostics.Process.Start("CMD.exe", "echo EVIL");
         Console.WriteLine($"Executed bad plugin with {context.Owner}");
      }
   }
}