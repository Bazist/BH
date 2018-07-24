using Microsoft.Owin;
using Owin;

[assembly: OwinStartupAttribute(typeof(BH.Web.Startup))]
namespace BH.Web
{
    public partial class Startup
    {
        public void Configuration(IAppBuilder app)
        {
            ConfigureAuth(app);

            WebService.FTServiceClient fts = new WebService.FTServiceClient();

            if (!fts.IsStarted())
            {
                fts.Start(0);
            }
        }
    }
}
