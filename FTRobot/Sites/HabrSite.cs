using FTServiceWCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTRobot
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
                new Page() {URL = "http://habrahabr.ru/posts/api/"},
                new Page() {URL = "http://habrahabr.ru/posts/administration//"},
                new Page() {URL = "http://habrahabr.ru/posts/databases/"},
                new Page() {URL = "http://habrahabr.ru/posts/security/"},
                new Page() {URL = "http://habrahabr.ru/posts/design-and-media/"},
                new Page() {URL = "http://habrahabr.ru/posts/programming/"},
                new Page() {URL = "http://habrahabr.ru/posts/software/"},
                new Page() {URL = "http://habrahabr.ru/posts/telecommunications/"},
                new Page() {URL = "http://habrahabr.ru/posts/fw-and-cms/"},
                new Page() {URL = "http://habrahabr.ru/posts/frontend/"},
                new Page() {URL = "http://habrahabr.ru/posts/others/"},
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

            //List<string> labels = ExtractByRegexp(page.HtmlContent, "<span class=\"all\">(?<num>[0-9а-яА-Я]+)</span>");

            List<string> parts = this.GetParts(page.HtmlContent, "<div class=\"post-comments\">", "</div>");

            if (docs.Count == parts.Count)
            {
                for (int i = docs.Count - 1; i >= 0; i--)
                {
                    string url = GetUrlByDocNumber(docs[i], 1, null);

                    var labels = this.ExtractByRegexp(parts[i], ">(?<num>[0-9]+)<");

                    if (labels.Count > 0)
                    {
                        CheckLabelAndAddPage(pages, url, labels[0]);
                    }
                    else
                    {
                        CheckLabelAndAddPage(pages, url, "0");
                    }
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = GetMessages("<div class=\"content html_format\">", "</div>", "div", page.HtmlContent);
            page.FileContent += " " + GetMessages("<div class=\"message html_format \">", "</div>", "div", page.HtmlContent, out page.CountMessages);

            //check load next page
            page.NeedLoadNextPage = false;
        }
    }
}
