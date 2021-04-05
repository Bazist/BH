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
    public class GamedevSite : Site
    {
        public GamedevSite(FTService service) : base(service)
        {
            BaseUrl = "gamedev.ru";
            Code = "game";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?gamedevelopment"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?programming"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?games"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?proects"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?hardware"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?soft"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?science"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?movies"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?regions"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?politics"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?humor"},
                new Page() {URL = "http://www.gamedev.ru/flame/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?graphics"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?2dgraph"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?physics"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?ai"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?sound"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?network"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?web"},
                new Page() {URL = "http://www.gamedev.ru/code/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/art/forum/?modeling"},
                new Page() {URL = "http://www.gamedev.ru/art/forum/?appreciatemodel"},
                new Page() {URL = "http://www.gamedev.ru/art/forum/?appreciateart"},
                new Page() {URL = "http://www.gamedev.ru/art/forum/?appreciateart"},
                new Page() {URL = "http://www.gamedev.ru/art/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/gamedesign/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/gamedesign/forum/?leveldesign"},
                new Page() {URL = "http://www.gamedev.ru/gamedesign/forum/?scenarios"},
                new Page() {URL = "http://www.gamedev.ru/industry/forum/?management"},
                new Page() {URL = "http://www.gamedev.ru/industry/forum/?events"},
                new Page() {URL = "http://www.gamedev.ru/industry/forum/?marketing"},
                new Page() {URL = "http://www.gamedev.ru/sound/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/mobile/forum/?common"},
                new Page() {URL = "http://www.gamedev.ru/projects/forum/?appreciate"},
                new Page() {URL = "http://www.gamedev.ru/projects/forum/?findteammembers"},
                new Page() {URL = "http://www.gamedev.ru/projects/forum/?releases"},
                new Page() {URL = "http://www.gamedev.ru/projects/forum/?tools"},
                new Page() {URL = "http://www.gamedev.ru/projects/forum/?contests"},
                new Page() {URL = "http://www.gamedev.ru/job/forum/?vacancy"},
                new Page() {URL = "http://www.gamedev.ru/job/forum/?once-only"},
                new Page() {URL = "http://www.gamedev.ru/job/forum/?resume"},
                new Page() {URL = "http://www.gamedev.ru/site/forum/?discussion"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://www.gamedev.ru/flame/forum/?id={0}&page={1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> parts = GetParts(page.HtmlContent, "<tr", "</tr>");

            for (int i = parts.Count - 1; i >= 0; i--)
            {
                string part = parts[i];

                List<string> parts2 = GetParts(part, "<b>", "</b>");

                foreach (string part2 in parts2)
                {
                    List<string> urls = GetParts(part2, "\"", "\"");

                    if (urls.Count > 0)
                    {
                        List<string> label = ExtractByRegexp(part, ">(?<num>[0-9]+)<");

                        CheckLabelAndAddPage(pages, urls[0], label[0]);
                    }
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<div class=\"block\">", "</div>", "div", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf(">Следующая</a>") >= 0);
        }
    }
}
