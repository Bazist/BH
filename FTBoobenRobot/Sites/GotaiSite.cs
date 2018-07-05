using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class GotaiSite : Site
    {
        public GotaiSite(FTService service) : base(service)
        {
            BaseUrl = "gamedev.ru";
            Code = "gotai";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.gotai.net/forum/"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://www.gotai.net/forum/default.aspx?threadid={0}&page={1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();
            
            List<string> parts = GetParts(page.HtmlContent, "<tr>", "</tr>");

            for (int i = parts.Count - 1; i >= 0; i--)
            {
                string part = parts[i];

                List<string> nums = ExtractByRegexp(part, "<a href=\"/forum/default.aspx\\?threadid=(?<num>[0-9]+)\\#[0-9]+\" class=\"f\"");

                List<string> labels = ExtractByRegexp(part, "<span class=\"Normal\">(?<num>[0-9]+)</span>");

                if (nums.Count > 0 && labels.Count > 0)
                {
                    string url = GetUrlByDocNumber(nums[0], 1, null);
                    string label = labels[0];

                    CheckLabelAndAddPage(pages, url, label);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = GetMessages("<div class=\"MessageContent\">", "</div>", "div", page.HtmlContent);
            
            //check load next page
            page.NeedLoadNextPage = page.HtmlContent.IndexOf(">След.") >= 0;
        }
    }
}
