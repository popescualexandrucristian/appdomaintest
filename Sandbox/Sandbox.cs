using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.Remoting.Lifetime;
using System.Security;
using System.Security.Permissions;

namespace AppDomainTest
{
    public struct SandboxData
    {
        public Sandbox Sandbox;
        public AppDomain AppDomain;
        public ClientSponsor Sponsor;
    }

    public class Sandbox : MarshalByRefObject
    {
        private void LoadPlugins(Dictionary<string, byte[]> pluginData)
        {
            try
            {
                if (LoadedPlugins == null)
                {
                    LoadedPlugins = new Dictionary<string, Assembly>();
                    foreach (var kv in pluginData)
                        LoadedPlugins[kv.Key] = Assembly.Load(kv.Value);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

      public static SandboxData Create(Dictionary<string, byte[]> pluginData)
      {
         AppDomain restrictedAppDomain = CrateRestrtictedAppDomain();

         Sandbox sandbox = (Sandbox)restrictedAppDomain.CreateInstanceAndUnwrap(
             typeof(Sandbox).Assembly.FullName, typeof(Sandbox).FullName);

         ClientSponsor clientSponsor = new ClientSponsor();
         clientSponsor.Register(sandbox);

         sandbox.LoadPlugins(pluginData);
         return new SandboxData
         {
            Sandbox = sandbox,
            AppDomain = restrictedAppDomain,
            Sponsor = clientSponsor,
         };
      }

        public void ExecuteUntrustedCode(IContext context)
        {

            try
            {
                foreach (var kv in LoadedPlugins)
                {
                    Type[] types = kv.Value.GetTypes();
                    foreach (Type t in types)
                        if (typeof(IPlugin).IsAssignableFrom(t))
                        {
                            try
                            {
                                IPlugin plugin = (IPlugin)Activator.CreateInstance(t);
                                plugin.Execute(context);
                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine($"Oh no {kv.Key} from {context.Owner} is bad !!");
                                Console.WriteLine(ex.ToString());
                            }
                        }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private Dictionary<string, Assembly> LoadedPlugins;

        static AppDomain CrateRestrtictedAppDomain()
        {
            PermissionSet permissionSet = new PermissionSet(PermissionState.None);
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.Execution));
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.RemotingConfiguration));
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.Infrastructure));

            AppDomainSetup appDomainSetup = new AppDomainSetup
            {
                ApplicationBase = AppDomain.CurrentDomain.SetupInformation.ApplicationBase
            };

            return AppDomain.CreateDomain("sandbox", null, appDomainSetup, permissionSet);
        }
    }
}