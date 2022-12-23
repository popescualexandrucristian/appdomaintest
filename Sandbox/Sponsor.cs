using System;
using System.Runtime.Remoting.Lifetime;
using System.Security.Permissions;

namespace AppDomainTest
{
    [Serializable]
    [SecurityPermission(SecurityAction.Demand, Infrastructure = true)]
    public class Sponsor : ISponsor, IDisposable
    {
        private MarshalByRefObject TargetObject = null;
        private bool Disposed = false;

        public Sponsor(MarshalByRefObject obj)
        {
            TargetObject = obj;

            ILease lease = (ILease)obj.GetLifetimeService();
            lease.Register(this);
        }

        [SecurityPermissionAttribute(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
        TimeSpan ISponsor.Renewal(ILease lease)
        {
            if (Disposed)
                return TimeSpan.Zero;
            else
                return lease.RenewOnCallTime;
        }

        ~Sponsor()
        {
            Dispose(false);
        }

        public void Destroy()
        {
            Dispose(true);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (Disposed)
            {
                return;
            }

            if (disposing)
            {
                if (TargetObject is IDisposable disposable)
                    disposable.Dispose();
            }

            object leaseObj;
            try
            {
                leaseObj = TargetObject.GetLifetimeService();
            }
            catch (Exception)
            {
                leaseObj = null;
            }

            if (leaseObj is ILease)
            {
                ILease lease = (ILease)leaseObj;
                lease.Unregister(this);
            }
            Disposed = true;
        }
    }
}
