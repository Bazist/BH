using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;

namespace FTContent
{
    [ServiceContract]
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]

    class Loader
    {
        private static ServiceHost _host;

        

        public static void StartWebservice()
        {
            Type serviceType = typeof(Loader);
            Uri serviceUri = new Uri(string.Format("http://localhost:8081/"));

            _host = new ServiceHost(serviceType, serviceUri);

            var basicHttpBinding = new BasicHttpBinding();
            basicHttpBinding.MaxReceivedMessageSize = int.MaxValue;

            _host.AddServiceEndpoint(serviceType, basicHttpBinding, serviceUri);
            _host.Open();
        }
    }
}
