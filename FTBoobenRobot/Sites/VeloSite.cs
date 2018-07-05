using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class VeloSite : Site
    {
        public VeloSite(FTService service) : base(service)
        {
            BaseUrl = "velokyiv.com";
            Code = "velo";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=36"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=124"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=80"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=180"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=200"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=33"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=121"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=122"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=1"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=177"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=73"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=26"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=79"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=104"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=105"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=106"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=107"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=108"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=110"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=185"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=109"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=114"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=22"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=170"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=68"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=12"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=13"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=131"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=4"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=92"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=93"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=94"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=95"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=34"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=162"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=178"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=182"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=130"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=61"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=14"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=2"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=32"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=97"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=98"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=151"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=9"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=133"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=134"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=135"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=136"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=137"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=138"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=139"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=140"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=19"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=161"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=125"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=126"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=113"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=51"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=127"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=141"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=128"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=129"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=66"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=62"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=3"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=30"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=31"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=173"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=115"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=49"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=70"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=52"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=81"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=89"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=123"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=44"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=24"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=156"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=41"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=15"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=54"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=76"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=152"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=180"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=11"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=23"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=158"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=184"},
                new Page() {URL = "http://velokyiv.com/forum/viewforum.php?f=25"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://velokyiv.com/forum/viewtopic.php?t={0}&start={1}", docNumber, (page - 1) * 25);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = GetTagsByAttr(page.HtmlContent, "topictitle");

            List<string> labels = GetTagsByAttr(page.HtmlContent, "<dfn>Відповіді</dfn>");

            if (nums.Count == labels.Count)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    var ids = this.ExtractByRegexp(nums[i], "(?<num>[0-9]+)");
                    var ids2 = this.ExtractByRegexp(labels[i], "(?<num>[0-9]+)");

                    string url = GetUrlByDocNumber(ids[1], 1, null);
                    CheckLabelAndAddPage(pages, url, ids2[0]);
                }
            }
           
            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"content\">", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("class=\"right-box right\">Далі</a>") >= 0);
        }
    }
}
