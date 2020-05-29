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
    public class SqlSite : Site
    {
        public SqlSite(FTService service) : base(service)
        {
            BaseUrl = "sql.ru";
            Code = "sql";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.sql.ru/forum/microsoft-sql-server"},
                new Page() {URL = "http://www.sql.ru/forum/interbase"},
                new Page() {URL = "http://www.sql.ru/forum/ibexpert"},
                new Page() {URL = "http://www.sql.ru/forum/oracle"},
                new Page() {URL = "http://www.sql.ru/forum/oracle-apex"},
                new Page() {URL = "http://www.sql.ru/forum/oracle-forms"},
                new Page() {URL = "http://www.sql.ru/forum/access"},
                new Page() {URL = "http://www.sql.ru/forum/db2"},
                new Page() {URL = "http://www.sql.ru/forum/mysql"},
                new Page() {URL = "http://www.sql.ru/forum/postgresql"},
                new Page() {URL = "http://www.sql.ru/forum/olap-dwh"},
                new Page() {URL = "http://www.sql.ru/forum/sybase"},
                new Page() {URL = "http://www.sql.ru/forum/informix"},
                new Page() {URL = "http://www.sql.ru/forum/db-other"},
                new Page() {URL = "http://www.sql.ru/forum/foxpro"},
                new Page() {URL = "http://www.sql.ru/forum/cache"},
                new Page() {URL = "http://www.sql.ru/forum/sqlite"},
                new Page() {URL = "http://www.sql.ru/forum/nosql-bigdata"},
                new Page() {URL = "http://www.sql.ru/forum/db-comparison"},
                new Page() {URL = "http://www.sql.ru/forum/db-design"},
                new Page() {URL = "http://www.sql.ru/forum/db-design"},
                new Page() {URL = "http://www.sql.ru/forum/job-offers"},
                new Page() {URL = "http://www.sql.ru/forum/erp-crm"},
                new Page() {URL = "http://www.sql.ru/forum/1c"},
                new Page() {URL = "http://www.sql.ru/forum/systems-architecture"},
                new Page() {URL = "http://www.sql.ru/forum/testing-qa"},
                new Page() {URL = "http://www.sql.ru/forum/reporting"},
                new Page() {URL = "http://www.sql.ru/forum/pt"},
                new Page() {URL = "http://www.sql.ru/forum/za-rubezhom"},
                new Page() {URL = "http://www.sql.ru/forum/certification"},
                new Page() {URL = "http://www.sql.ru/forum/hardware"},
                new Page() {URL = "http://www.sql.ru/forum/dev-management"},
                new Page() {URL = "http://www.sql.ru/forum/legal"},
                new Page() {URL = "http://www.sql.ru/forum/dotnet"},
                new Page() {URL = "http://www.sql.ru/forum/asp-net"},
                new Page() {URL = "http://www.sql.ru/forum/ado-linq-ef-orm"},
                new Page() {URL = "http://www.sql.ru/forum/wpf-silverlight"},
                new Page() {URL = "http://www.sql.ru/forum/wcf-ws-remoting"},
                new Page() {URL = "http://www.sql.ru/forum/delphi"},
                new Page() {URL = "http://www.sql.ru/forum/cpp"},
                new Page() {URL = "http://www.sql.ru/forum/visual-basic"},
                new Page() {URL = "http://www.sql.ru/forum/programming"},
                new Page() {URL = "http://www.sql.ru/forum/java"},
                new Page() {URL = "http://www.sql.ru/forum/mobile-dev"},
                new Page() {URL = "http://www.sql.ru/forum/powerbuilder"},
                new Page() {URL = "http://www.sql.ru/forum/ms-office"},
                new Page() {URL = "http://www.sql.ru/forum/sharepoint"},
                new Page() {URL = "http://www.sql.ru/forum/xml"},
                new Page() {URL = "http://www.sql.ru/forum/php-perl"},
                new Page() {URL = "http://www.sql.ru/forum/html-javascript-css"},
                new Page() {URL = "http://www.sql.ru/forum/ssjs"},
                new Page() {URL = "http://www.sql.ru/forum/windows"},
                new Page() {URL = "http://www.sql.ru/forum/linux"},
                new Page() {URL = "http://www.sql.ru/forum/other-os"},
                new Page() {URL = "http://www.sql.ru/forum/sqlru"},
                new Page() {URL = "http://www.sql.ru/forum/question-answer"},
                new Page() {URL = "http://www.sql.ru/forum/test"},
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://www.sql.ru/forum/{0}-{1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> parts = GetParts(page.HtmlContent, "<tr>", "</tr>");

            for (int i = parts.Count - 1; i >= 0; i--)
            {
                string part = parts[i];

                List<string> nums = ExtractByRegexp(part, "<a href=\"http://www.sql.ru/forum/(?<num>[0-9]+)/");

                List<string> labels = ExtractByRegexp(part, "<td style=\"text-align:center\">(?<num>[0-9]+)</td>");

                if (nums.Count > 0 && labels.Count > 0)
                {
                    string url = GetUrlByDocNumber(nums[0], 1, null);
                    string label = labels[0];

                    CheckLabelAndAddPage(pages, url, label);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<td class=\"msgBody\">", "</td>", "td", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("<a id=\"urlForumNext\"") >= 0);
        }
    }
}
