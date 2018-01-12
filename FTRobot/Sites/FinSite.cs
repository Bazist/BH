using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTRobot
{
    public class FinSite : Site
    {
        public FinSite(FTService service) : base(service)
        {
            BaseUrl = "finance.ua";
            Code = "fin";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=20"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=23"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=52"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=62"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=4"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=129"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=128"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=88"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=113"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=119"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=29"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=53"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=51"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=58"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=54"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=64"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=19"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=123"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=120"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=6"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=74"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=75"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=76"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=77"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=78"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=79"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=90"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=86"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=13"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=7"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=22"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=24"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=27"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=59"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=30"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=31"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=32"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=125"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=117"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=131"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=33"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=14"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=80"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=40"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=63"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=47"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=48"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=49"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=50"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=68"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=118"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=115"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=121"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=85"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=43"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=127"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=69"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=35"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=36"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=39"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=38"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=37"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=84"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=72"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=3"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=116"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=107"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=83"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=66"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=73"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=70"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=71"},
                new Page() {URL = "http://forum.finance.ua/viewforum.php?f=114"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "topic(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboard)
        {
            return string.Format("http://forum.finance.ua/topic{0}.html?start={1}", docNumber, (page - 1) * 10);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = GetParts(page.HtmlContent, "<td class=\"topic_name\">", "</td>");

            List<string> labels = GetParts(page.HtmlContent, "<td class=\"font14 reply\">", "</td>");

            if (nums.Count == labels.Count)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    List<string> nums2 = GetDocNumberByUrl(nums[i]);

                    string url = GetUrlByDocNumber(nums2[0], 1, null);
                    CheckLabelAndAddPage(pages, url, labels[i]);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"postbody\"", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf(">&gt;</a>") >= 0);
        }
    }
}

