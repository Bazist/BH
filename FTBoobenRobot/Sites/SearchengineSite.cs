using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class SearchengineSite : Site
    {
        public SearchengineSite(FTService service) : base(service)
        {
            BaseUrl = "searchengines.guru";
            Code = "search";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=10"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=26"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=60"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=111"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=80"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=7"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=38"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=20"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=12"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=33"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=59"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=61"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=19"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=71"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=25"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=15"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=26"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=14"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=45"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=114"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=46"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=23"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=22"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=35"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=85"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=47"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=65"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=117"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=66"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=118"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=119"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=48"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=36"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=39"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=56"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=37"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=113"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=24"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=82"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=87"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=49"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=81"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=123"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=40"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=69"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=83"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=121"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=122"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=112"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=16"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=17"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=90"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=78"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=57"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=55"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=106"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=75"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=51"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=124"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=74"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=34"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=92"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=93"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=94"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=95"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=79"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=97"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=63"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=68"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=73"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=107"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=86"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=104"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=98"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=76"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=62"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=89"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=70"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=99"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=58"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=108"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=109"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=101"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=102"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=42"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=72"},
                new Page() {URL = "http://searchengines.guru/forumdisplay.php?f=103"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://searchengines.guru/showthread.php?t={0}&page={1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> urls = ExtractByRegexp(page.HtmlContent, "td_threadtitle_(?<num>[0-9]+)");

            List<string> labels = ExtractByRegexp(page.HtmlContent, "return false;\">(?<num>[0-9,]+)</a>");

            if (urls.Count == labels.Count)
            {
                for (int i = urls.Count - 1; i >= 0; i--)
                {
                    string url = GetUrlByDocNumber(urls[i], 1, null);
                    CheckLabelAndAddPage(pages, url, labels[i]);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div id=\"post_message", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf(">&gt;</a>") >= 0);
        }
    }
}
