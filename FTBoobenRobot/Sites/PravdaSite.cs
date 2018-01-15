using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class PravdaSite : Site
    {
        public PravdaSite(FTService service) : base(service)
        {
            BaseUrl = "pravda.com.ua";
            Code = "pravda";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://forum.pravda.com.ua/index.php?board=2.0"},
                new Page() {URL = "http://forum.pravda.com.ua/index.php?board=2.50"},
                new Page() {URL = "http://forum.pravda.com.ua/index.php?board=2.100"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://forum.pravda.com.ua/index.php?topic={0}.{1}", docNumber, (page - 1) * 30);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = GetParts(page.HtmlContent, "<h4>", "</h4>");

            List<string> labels = this.ExtractByRegexp(page.HtmlContent, "nowrap=\"nowrap\">(?<num>[0-9\\s]+)</td>");

            if (nums.Count == labels.Count / 2)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    var ids = this.ExtractByRegexp(nums[i], "topic=(?<num>[0-9]+)");
                    var ids2 = this.ExtractByRegexp(labels[i * 2], "(?<num>[0-9]+)");

                    
                    string url = GetUrlByDocNumber(ids[0], 1, null);
                    CheckLabelAndAddPage(pages, url, ids2[0]);
                }
            }
           
            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"msg\">", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("Наступна</a>") >= 0) &&
                                    (page.HtmlContent.IndexOf("<a href=\"\">Наступна</a>") == -1);
        }
    }
}
