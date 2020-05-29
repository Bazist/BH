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
    public class IxbtSite : Site
    {
        public IxbtSite(FTService service) : base(service)
        {
            BaseUrl = "ixbt.com";
            Code = "ixbt";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://forum.ixbt.com/?id=4"},
                new Page() {URL = "http://forum.ixbt.com/?id=42"},
                new Page() {URL = "http://forum.ixbt.com/?id=78"},
                new Page() {URL = "http://forum.ixbt.com/?id=101"},
                new Page() {URL = "http://forum.ixbt.com/?id=82"},
                new Page() {URL = "http://forum.ixbt.com/?id=79"},
                new Page() {URL = "http://forum.ixbt.com/?id=100"},
                new Page() {URL = "http://forum.ixbt.com/?id=6"},
                new Page() {URL = "http://forum.ixbt.com/?id=5"},
                new Page() {URL = "http://forum.ixbt.com/?id=97"},
                new Page() {URL = "http://forum.ixbt.com/?id=8"},
                new Page() {URL = "http://forum.ixbt.com/?id=30"},
                new Page() {URL = "http://forum.ixbt.com/?id=9"},
                new Page() {URL = "http://forum.ixbt.com/?id=27"},
                new Page() {URL = "http://forum.ixbt.com/?id=10"},
                new Page() {URL = "http://forum.ixbt.com/?id=73"},
                new Page() {URL = "http://forum.ixbt.com/?id=29"},
                new Page() {URL = "http://forum.ixbt.com/?id=28"},
                new Page() {URL = "http://forum.ixbt.com/?id=20"},
                new Page() {URL = "http://forum.ixbt.com/?id=12"},
                new Page() {URL = "http://forum.ixbt.com/?id=90"},
                new Page() {URL = "http://forum.ixbt.com/?id=95"},
                new Page() {URL = "http://forum.ixbt.com/?id=60"},
                new Page() {URL = "http://forum.ixbt.com/?id=61"},
                new Page() {URL = "http://forum.ixbt.com/?id=62"},
                new Page() {URL = "http://forum.ixbt.com/?id=11"},
                new Page() {URL = "http://forum.ixbt.com/?id=31"},
                new Page() {URL = "http://forum.ixbt.com/?id=13"},
                new Page() {URL = "http://forum.ixbt.com/?id=49"},
                new Page() {URL = "http://forum.ixbt.com/?id=32"},
                new Page() {URL = "http://forum.ixbt.com/?id=14"},
                new Page() {URL = "http://forum.ixbt.com/?id=7"},
                new Page() {URL = "http://forum.ixbt.com/?id=66"},
                new Page() {URL = "http://forum.ixbt.com/?id=17"},
                new Page() {URL = "http://forum.ixbt.com/?id=45"},
                new Page() {URL = "http://forum.ixbt.com/?id=16"},
                new Page() {URL = "http://forum.ixbt.com/?id=75"},
                new Page() {URL = "http://forum.ixbt.com/?id=74"},
                new Page() {URL = "http://forum.ixbt.com/?id=88"},
                new Page() {URL = "http://forum.ixbt.com/?id=47"},
                new Page() {URL = "http://forum.ixbt.com/?id=22"},
                new Page() {URL = "http://forum.ixbt.com/?id=23"},
                new Page() {URL = "http://forum.ixbt.com/?id=76"},
                new Page() {URL = "http://forum.ixbt.com/?id=96"},
                new Page() {URL = "http://forum.ixbt.com/?id=58"},
                new Page() {URL = "http://forum.ixbt.com/?id=24"},
                new Page() {URL = "http://forum.ixbt.com/?id=26"},
                new Page() {URL = "http://forum.ixbt.com/?id=94"},
                new Page() {URL = "http://forum.ixbt.com/?id=25"},
                new Page() {URL = "http://forum.ixbt.com/?id=87"},
                new Page() {URL = "http://forum.ixbt.com/?id=92"},
                new Page() {URL = "http://forum.ixbt.com/?id=48"},
                new Page() {URL = "http://forum.ixbt.com/?id=44"},
                new Page() {URL = "http://forum.ixbt.com/?id=15"},
                new Page() {URL = "http://forum.ixbt.com/?id=54"},
                new Page() {URL = "http://forum.ixbt.com/?id=55"},
                new Page() {URL = "http://forum.ixbt.com/?id=64"},
                new Page() {URL = "http://forum.ixbt.com/?id=57"},
                new Page() {URL = "http://forum.ixbt.com/?id=65"},
                new Page() {URL = "http://forum.ixbt.com/?id=63"},
                new Page() {URL = "http://forum.ixbt.com/?id=70"},
                new Page() {URL = "http://forum.ixbt.com/?id=91"},
                new Page() {URL = "http://forum.ixbt.com/?id=71"},
                new Page() {URL = "http://forum.ixbt.com/?id=80"},
                new Page() {URL = "http://forum.ixbt.com/?id=72"},
                new Page() {URL = "http://forum.ixbt.com/?id=103"},
                new Page() {URL = "http://forum.ixbt.com/?id=77"},
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "[0-9]+:(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://forum.ixbt.com/topic.cgi?id={0}:{1}-{2}", dashboardID, docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();
            //"<a\\shref=\"topic\\.cgi\\?id=77\\:(?<num>[0-9]+)\">"

            string dashboardID = this.ExtractByRegexp(page.URL, "(?<num>[0-9]+)")[0];

            List<string> nums = this.ExtractByRegexp(page.HtmlContent, "'topic\\.cgi\\?id=" + dashboardID + "\\:(?<num>[0-9]+)'");

            List<string> labels = this.ExtractByRegexp(page.HtmlContent, ",(?<num>[0-9]+),[0-9]+,'");

            if (nums.Count == labels.Count / 2)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    string url = GetUrlByDocNumber(nums[i], 1, dashboardID);
                    CheckLabelAndAddPage(pages, url, labels[i * 2 + 1], dashboardID);
                }
            }
           
            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<script>", "</script>", "script", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf(">далее</a>") >= 0);
        }
    }
}
