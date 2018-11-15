using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BH.BoobenRobot
{
    public class HabrSite : Site
    {
        public HabrSite(FTService service) : base(service)
        {
            BaseUrl = "habrahabr.ru";
            Code = "habr";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "https://habr.com/all/"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://habrahabr.ru/post/{0}/", docNumber);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> docs = ExtractByRegexp(page.HtmlContent, "id=\"post_(?<num>[0-9]+)\"");

            List<string> labels = this.GetParts(page.HtmlContent, "<span class=\"post-stats__comments", "</span>");

            if (docs.Count == labels.Count)
            {
                for (int i = docs.Count - 1; i >= 0; i--)
                {
                    string url = GetUrlByDocNumber(docs[i], 1, null);

                    CheckLabelAndAddPage(pages, url, labels[i]);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = GetMessages("<article", "</article>", "article", page.HtmlContent);

            page.FileContent += " " + GetMessages("<div class=\"comments-section\"", "</div>", "div", page.HtmlContent, out page.CountMessages);

            //check load next page
            page.NeedLoadNextPage = false;
        }
    }
}
