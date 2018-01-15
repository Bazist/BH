using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class CyberSite : Site
    {
        public CyberSite(FTService service) : base(service)
        {
            BaseUrl = "cyberforum.ru";
            Code = "cyber";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.cyberforum.ru/ios-dev/", DashboardURL = "ios-dev"},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
                //new Page() {URL = "", DashboardURL = ""},
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "thread(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboard)
        {
            if (page == 1)
            {
                return string.Format("http://www.cyberforum.ru/{0}/thread{1}.html", dashboard, docNumber);
            }
            else
            {
                return string.Format("http://www.cyberforum.ru/{0}/thread{1}-page{2}.html", dashboard, docNumber, page);
            }
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = ExtractByRegexp(page.HtmlContent, "thread_title_(?<num>[0-9]+)");

            List<string> labels = GetParts(page.HtmlContent, "<td class=\"alt1\" align=\"center\">", "</td>");

            if (nums.Count == labels.Count)
            {
                for (int i = 0; i < nums.Count; i++)
                {
                    string url = GetUrlByDocNumber(nums[i], 1, page.DashboardURL);
                    CheckLabelAndAddPage(pages, url, labels[i], page.DashboardURL);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div id=\"post_message_", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("Ctrl+Shift &#8594;") >= 0);
        }
    }
}
