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
    public class AutouaSite : Site
    {
        public AutouaSite(FTService service) : base(service)
        {
            BaseUrl = "autoua.net";
            Code = "aua";
            SiteEncoding = Encoding.GetEncoding("windows-1251");
            PageDelay = TimeSpan.FromSeconds(1);
            SiteDelay = TimeSpan.FromHours(4);
            ErrorDelay = TimeSpan.FromMinutes(15);
        }

        protected override List<Page> GetDashboards()
        {
            return new List<Page>
            {
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autoua"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=vopros-otvet"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autouaclub"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=repair"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=avto_video_registr"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=carchoice"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=insurance"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=roadlaw"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=tyres_and_wheels"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=premium_motor_fuel"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=car_body"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=electrician"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=shield"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=tuning"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=audi"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=bmw"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=chevrolet"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=citroen"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=daewoo"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=ford"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=honda"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=hyundai"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=kia"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=mazda"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=mercedes"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=mitsubishi"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=nissan"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=opel"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=peugeot"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=renault"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=seat"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=Skoda"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=ssangyong"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=subaru"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=suzuki"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=toyota"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=volkswagen"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=volvo"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=vaz"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=volga"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=tavria"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=americanscar"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=italauto"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=chinacar"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=commercial_vehicles"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=car_rental"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=taxi"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autotravel_ukraine"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autotravel_contries"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=travel_and_rest"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=navigation"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autotravel_club_life"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=karavaning"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=autotravel"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=fellow"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=photo"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=radio"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=model"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=football"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=auto_sport"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=offroad"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=moto"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=cycle"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=pets"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=philosophy"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=cottage"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=off_top"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=traktir"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=boudoir"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=cinema"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=computers"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=kids"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=charity"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=household"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=building"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=business"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=seek_jobs"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=dneprr"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=ua_region"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=sale_and_buy_auto"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=adv"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=lie"},
                new Page() {URL = "http://forum.autoua.net/postlist.php?Cat=0&Board=reccomend"}
            };
        }

        protected override List<string> GetDocNumberByUrl(string url)
        {
            return this.ExtractByRegexp(url, "Number=(?<num>[0-9]+)");
        }

        protected override string GetUrlByDocNumber(string docNumber, int page, string dashboardID)
        {
            return string.Format("http://forum.autoua.net/showflat.php?Number={0}&fpart={1}", docNumber, page);
        }

        protected override List<Page> OnDashboardLoaded(Page page)
        {
            List<Page> pages = new List<Page>();

            List<string> nums = GetParts(page.HtmlContent, "hspace=\"0\"", "</a>");

            List<string> labels = GetParts(page.HtmlContent, "<td align=\"center\" nowrap=\"nowrap\" class=", "</td>");

            List<string> labels2 = new List<string>();

            foreach (string lbl in labels)
            {
                int idx = lbl.LastIndexOf(">");

                if (idx > 0)
                {
                    string lbl2 = lbl.Substring(idx + 1).Trim();

                    int res;
                    if (int.TryParse(lbl2, out res))
                    {
                        labels2.Add(res.ToString());
                    }
                }
            }

            if (nums.Count == labels2.Count)
            {
                for (int i = nums.Count - 1; i >= 0; i--)
                {
                    List<string> nums2 = GetDocNumberByUrl(nums[i]);

                    string url = GetUrlByDocNumber(nums2[0], 1, null);
                    CheckLabelAndAddPage(pages, url, labels2[i]);
                }
            }

            return pages;
        }

        protected override void OnPageLoaded(Page page)
        {
            //content
            page.FileContent = (" " + GetMessages("<font class=\"post\">", "</font>", "font", page.HtmlContent));

            //check load next page
            page.NeedLoadNextPage = (page.HtmlContent.IndexOf("Следующая</div>") >= 0);
        }
    }
}

