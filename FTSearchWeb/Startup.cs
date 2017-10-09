using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(FTSearchWeb.Startup))]
namespace FTSearchWeb
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);

            BH.FTServiceClient fts = new BH.FTServiceClient();

            if (!fts.IsStarted())
            {
                fts.Start(0);
            }
        }
    }
}
