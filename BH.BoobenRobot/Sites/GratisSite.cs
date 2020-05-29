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
    public class GratisSite : Site
    {
        public GratisSite(FTService service) : base(service)
        {
            BaseUrl = "gratis.pp.ru";
            Code = "gratis";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://www.gratis.pp.ru/index.php?s=&act=SF&f=137", DashboardID = "137"},
                new Page() {URL = "http://www.gratis.pp.ru/index.php?s=&act=SF&f=161", DashboardID = "161"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=92",DashboardID="92"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=44",DashboardID="44"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=14",DashboardID="14"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=11",DashboardID="11"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=108",DashboardID="108"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=27",DashboardID="27"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=89",DashboardID="89"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=70",DashboardID="70"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=39",DashboardID="39"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=120",DashboardID="120"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=123",DashboardID="123"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=121",DashboardID="121"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=73",DashboardID="73"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=127",DashboardID="127"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=2",DashboardID="2"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=9",DashboardID="9"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=46",DashboardID="46"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=12",DashboardID="12"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=74",DashboardID="74"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=26",DashboardID="26"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=35",DashboardID="35"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=10",DashboardID="10"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=18",DashboardID="18"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=28",DashboardID="28"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=29",DashboardID="29"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=86",DashboardID="86"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=84",DashboardID="84"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=17",DashboardID="17"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=164",DashboardID="164"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=165",DashboardID="165"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=166",DashboardID="166"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=167",DashboardID="167"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=186",DashboardID="186"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=16",DashboardID="16"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=22",DashboardID="22"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=179",DashboardID="179"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=212",DashboardID="212"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=99",DashboardID="99"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=188",DashboardID="188"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=160",DashboardID="160"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=100",DashboardID="100"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=147",DashboardID="147"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=90",DashboardID="90"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=101",DashboardID="101"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=175",DashboardID="175"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=138",DashboardID="138"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=98",DashboardID="98"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=215",DashboardID="215"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=157",DashboardID="157"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=191",DashboardID="191"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=193",DashboardID="193"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=196",DashboardID="196"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=195",DashboardID="195"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=197",DashboardID="197"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=198",DashboardID="198"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=199",DashboardID="199"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=200",DashboardID="200"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=201",DashboardID="201"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=202",DashboardID="202"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=205",DashboardID="205"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=203",DashboardID="203"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=204",DashboardID="204"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=206",DashboardID="206"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=192",DashboardID="192"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=194",DashboardID="194"},

                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=214",DashboardID="214"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=213",DashboardID="213"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=106",DashboardID="106"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=163",DashboardID="163"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=173",DashboardID="173"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=174",DashboardID="174"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=187",DashboardID="187"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=126",DashboardID="126"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=158",DashboardID="158"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=148",DashboardID="148"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=153",DashboardID="153"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=155",DashboardID="155"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=176",DashboardID="176"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=30",DashboardID="30"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=211",DashboardID="211"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=162",DashboardID="162"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=207",DashboardID="207"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=149",DashboardID="149"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=156",DashboardID="156"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=154",DashboardID="154"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=40",DashboardID="40"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=76",DashboardID="76"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=168",DashboardID="168"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=209",DashboardID="209"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=119",DashboardID="119"},
                new Page(){URL="http://www.gratis.pp.ru/index.php?s=&act=SF&f=216",DashboardID="216"}

            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            List<string> docNums = this.ExtractByRegexp(url, "(?<num>[0-9]+)");
            docNums.RemoveAt(0); //skip dashboard

            return docNums;
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://www.gratis.pp.ru/index.php?s=&act=ST&f={0}&t={1}&st={2}", dashboardID, docNumber, (page - 1) * 10);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = this.GetParts(page.HtmlContent, "<span class='linkthru'", "</span>");

            List<string> labels = ExtractByRegexp(page.HtmlContent, "who_posted\\([0-9]+\\);'>(?<num>[0-9\\s]+)</a");

            if (nums.Count == labels.Count)
            {
                for (int i = 0; i < nums.Count; i++)
                {
                    List<string> docNums = ExtractByRegexp(nums[i], "t=(?<num>[0-9]+)");

                    string url = GetUrlByDocNumber(docNums[0], 1, page.DashboardID);
                    CheckLabelAndAddPage(pages, url, labels[i], page.DashboardID);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<td width='100%' valign='top' class='post1'>", "</td>", "td", page.HtmlContent));
            page.FileContent += (" " + GetMessages("<td width='100%' valign='top' class='post2'>", "</td>", "td", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("Последняя &raquo;</a>") >= 0);
        }
    }
}