using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class SkcSite : Site
    {
        public SkcSite(FTService service) : base(service)
        {
            BaseUrl = "skoda-club.org.ua";
            Code = "skc";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=50"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=26"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=87"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=52"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=56"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=67"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=86"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=33"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=29"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=46"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=31"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=44"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=92"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=42"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=32"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=48"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=28"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=61"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=47"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=30"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=65"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=66"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=82"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=27"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=28"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=75"},
                new Page() {URL = "http://www.skoda-club.org.ua/forum/forumdisplay.php?fid=90"},
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://www.skoda-club.org.ua/forum/showthread.php?tid={0}&page={1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = ExtractByRegexp(page.HtmlContent, "tid=(?<num>[0-9]+)\"\\sclass=\"\\ssubject");

            List<string> labels = ExtractByRegexp(page.HtmlContent, "whoPosted\\(([0-9]+)\\);\">(?<num>[0-9\\s]+)");

            if (nums.Count == labels.Count)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    string url = GetUrlByDocNumber(nums[i], 1, null);
                    CheckLabelAndAddPage(pages, url, labels[i]);
                }
            }
           
            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div id=\"pid_", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("pagination_next") >= 0);
        }
    }
}
