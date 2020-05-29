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

using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BH.BoobenRobot
{
    public class SevasSite : Site
    {
        public SevasSite(FTService service) : base(service)
        {
            BaseUrl = "sevastopol.info";
            Code = "sevas";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=1"},
                new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=56"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=22"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=27"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=24"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=3"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=34"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=4"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=6"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=44"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=20"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=5"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=21"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=47"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=26"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=16"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=14"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=52"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=53"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=25"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=55"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=12"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=51"},
                //new Page() {URL = "http://forum.sevastopol.info/viewforum.php?f=13"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://forum.sevastopol.info/viewtopic.php?t={0}&start={1}", docNumber, (page - 1) * 25);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> parts = GetParts(page.HtmlContent, "<tr>", "</tr>");

            for (int i = parts.Count - 1; i >= 0; i--)
            {
                string part = parts[i];

                List<string> parts2 = GetParts(part, "<a title=\"Добавлено:", "</td>");

                if (parts2.Count > 0)
                {
                    List<string> nums = ExtractByRegexp(parts2[0], "t=(?<num>[0-9]+)");

                    List<string> labels = ExtractByRegexp(part, "<p class=\"topicdetails\">(?<num>[0-9]+)</p>");

                    if (nums.Count > 0 && labels.Count > 0)
                    {
                        string url = GetUrlByDocNumber(nums[0], 1, null);
                        string label = labels[0];

                        CheckLabelAndAddPage(pages, url, label);
                    }
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"postbody\">", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf(">След.</a>") >= 0);
        }
    }
}
