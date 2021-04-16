using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System.ServiceProcess;
using BH.WorkerService.Options;

namespace BH.WorkerService
{
    public class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length > 0)
            {
                var proc = new System.Diagnostics.Process();
                proc.StartInfo = new System.Diagnostics.ProcessStartInfo();

                proc.StartInfo.FileName = "sc";

                var servicePath = Assembly.GetEntryAssembly().Location.Replace(".dll", ".exe");

                switch (args[0])
                {
                    case "-i":
                        proc.StartInfo.Arguments = $"create BH.FTSearch binPath=\"{servicePath}\" start=auto";
                        break;
                    case "-u":
                        proc.StartInfo.Arguments = $"delete BH.FTSearch binPath =\"{servicePath}\"";
                        break;
                    default:
                        throw new Exception("Command doesn't recornized. Choose -i for install service and -u for uninstall.");
                }

                //proc.StartInfo.UseShellExecute = false;
                //proc.StartInfo.CreateNoWindow = true;
                //proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                proc.StartInfo.Verb = "runas";

                proc.Start();
                proc.WaitForExit();

                if(args[0] == "-i")
                {
                    proc.StartInfo.Arguments = $"start BH.FTSearch";

                    proc.Start();
                    proc.WaitForExit();
                }

                return;
            }

            CreateHostBuilder(args).Build().Run();
        }

        public static IHostBuilder CreateHostBuilder(string[] args) =>
            Host.CreateDefaultBuilder(args)
                .UseWindowsService()
                .ConfigureLogging(logging =>
                {
                    logging.AddEventLog()
                           .AddConsole();
                })
                .ConfigureServices((hostContext, services) =>
                {
                    services.Configure<WorkerOptions>(hostContext.Configuration)
                            .AddHostedService<Worker>();
                    
                });
    }
}
