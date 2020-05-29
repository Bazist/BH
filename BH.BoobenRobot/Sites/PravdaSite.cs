﻿/*
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
    public class PravdaSite : Site
    {
        public PravdaSite(FTService service) : base(service)
        {
            BaseUrl = "pravda.com.ua";
            Code = "pravda";
            SiteEncoding = Encoding.UTF8;
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "https://forum.pravda.com.ua/index.php?board=2.0"},
                new Page() {URL = "https://forum.pravda.com.ua/index.php?board=2.50"},
                new Page() {URL = "https://forum.pravda.com.ua/index.php?board=2.100"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("https://forum.pravda.com.ua/index.php?topic={0}.{1}", docNumber, (page - 1) * 30);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = GetParts(page.HtmlContent, "<h4>", "</h4>");

            List<string> labels = this.ExtractByRegexp(page.HtmlContent, "nowrap=\"nowrap\">(?<num>[0-9\\s]+)</td>");

            if (nums.Count == labels.Count / 2)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    var ids = this.ExtractByRegexp(nums[i], "topic=(?<num>[0-9]+)");
                    var ids2 = this.ExtractByRegexp(labels[i * 2], "(?<num>[0-9\\s]+)");
                    
                    string url = GetUrlByDocNumber(ids[0], 1, null);
                    CheckLabelAndAddPage(pages, url, ids2[0]);
                }
            }
           
            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"msg\">", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = page.HtmlContent.Contains("Наступна</span>") &&
                                    !page.HtmlContent.Contains("<a href=\"\"><span class=\"hideonmobile\">Наступна</span>");
        }
    }
}
