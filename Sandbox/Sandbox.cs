using System;
using System.Collections.Generic;
using System.Reflection;
using System.Security;
using System.Security.Permissions;

namespace AppDomainTest
{
    public class Sandbox : MarshalByRefObject, IDisposable
    {
        private void SetRestrictedAppDomain(AppDomain restrictedAppDomain)
        {
            this.RestrictedAppDomain = restrictedAppDomain;
        }

        private void SetPluginData(Dictionary<string, byte[]> pluginData)
        {
            this.PluginData = pluginData;
        }

        public static Sandbox Create(Dictionary<string, byte[]> pluginData)
        {
            AppDomain restrictedAppDomain = CrateRestrtictedAppDomain();

            Sandbox sandbox = (Sandbox)restrictedAppDomain.CreateInstanceAndUnwrap(
                typeof(Sandbox).Assembly.FullName, typeof(Sandbox).FullName);

            sandbox.SetRestrictedAppDomain(restrictedAppDomain);
            sandbox.SetPluginData(pluginData);
            return sandbox;
        }

        public void ExecuteUntrustedCode(IContext context)
        {
            try
            {
                if (LoadedPlugins == null)
                {
                    LoadedPlugins = new Dictionary<string, Assembly>();
                    foreach (var kv in PluginData)
                        LoadedPlugins[kv.Key] = Assembly.Load(kv.Value);
                }

                foreach (var kv in LoadedPlugins)
                {
                    Type[] types = kv.Value.GetTypes();
                    foreach (Type t in types)
                        if (typeof(IPlugin).IsAssignableFrom(t))
                        {
                            IPlugin plugin = (IPlugin)Activator.CreateInstance(t);
                            plugin.Execute(context);
                        }
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        public void Dispose()
        {
            if (PluginData != null)
                PluginData.Clear();
            PluginData = null;

            if(LoadedPlugins != null)
            {
                LoadedPlugins.Clear();
                LoadedPlugins = null;
            }

            if(RestrictedAppDomain != null)
            {
                AppDomain.Unload(RestrictedAppDomain);
                RestrictedAppDomain = null;
            }
        }

        ~Sandbox()
        {
            Dispose();
        }

        private AppDomain RestrictedAppDomain;
        private Dictionary<string, byte[]> PluginData;
        private Dictionary<string, Assembly> LoadedPlugins;

        static AppDomain CrateRestrtictedAppDomain()
        {
            PermissionSet permissionSet = new PermissionSet(PermissionState.None);
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.Execution));
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.Infrastructure));
            permissionSet.AddPermission(new SecurityPermission(SecurityPermissionFlag.RemotingConfiguration));
            permissionSet.AddPermission(new FileIOPermission(PermissionState.Unrestricted));
            //permissionSet.AddPermission(new FileIOPermission(FileIOPermissionAccess.Read | FileIOPermissionAccess.PathDiscovery, AppDomain.CurrentDomain.BaseDirectory));

            AppDomainSetup appDomainSetup = new AppDomainSetup();
            appDomainSetup.ApplicationBase = AppDomain.CurrentDomain.SetupInformation.ApplicationBase;

            return AppDomain.CreateDomain("sandbox", null, appDomainSetup, permissionSet);
        }
    }
}