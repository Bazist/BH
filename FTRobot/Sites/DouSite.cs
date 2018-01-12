using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTRobot
{
    public class DouSite : Site
    {
        public DouSite(FTService service) : base(service)
        {
            BaseUrl = "dou.ua";
            Code = "dou";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://dou.ua/forums/"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://dou.ua/forums/topic/{0}/", docNumber);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> parts = GetParts(page.HtmlContent, "<h2>", "</h2>");

            for (int i = parts.Count - 1; i >= 0; i--)
            {
                string part = parts[i];

                List<string> urls = GetParts(part, "<a href=\"", "\"");

                List<string> label = ExtractByRegexp(part, "(?<num>[0-9K]+)</a");

                if (urls.Count > 0 && label.Count > 0)
                {
                    CheckLabelAndAddPage(pages, urls[0], label[0]);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = GetMessages("<article class=\"b-typo\">", "</article>", "article", page.HtmlContent);
            page.FileContent += (" " + GetMessages("<div class=\"text b-typo\">", "</div>", "div", page.HtmlContent, out page.CountMessages));

            //check load next page
            page.NeedLoadNextPage = false;
        }
    }
}
