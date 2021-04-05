/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

using BH.FTServer;
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

            //for (int i = 17365; i < 478134; i++)
            //{
            //    pages.Add(new Page()
            //    {
            //        DashboardURL = null,
            //        DashboardID = null,
            //        URL = GetUrlByDocNumber(i.ToString(), 1, null),
            //        DocNumber = i.ToString(),
            //        PageNumber = 1
            //    });
            //}

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
            page.FileContent = GetMessages("<div class=\"post__body post__body_full\">", "</div>", "div", page.HtmlContent);

            var comments = " " + GetMessages("<div class=\"comment__message \">", "</div>", "div", page.HtmlContent, out page.CountMessages);

            page.FileContent += comments;

            //check load next page
            page.NeedLoadNextPage = false;
        }
    }
}
