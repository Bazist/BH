using BH.FTServer;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Hosting;

namespace BH.REST
{
    public class RESTService
    {
        public static void StartWebService(FTService ftService)
        {
            Startup.FTService = ftService;

            CreateHostBuilder(ftService).Build().Run();
        }

        public static IHostBuilder CreateHostBuilder(FTService ftService) =>
            Host.CreateDefaultBuilder()
                .ConfigureWebHostDefaults(webBuilder =>
                {
                    webBuilder.UseStartup<Startup>();
                });
    }
}
