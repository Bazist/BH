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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BH.BoobenRobot
{
    public class Page
    {
        public string DashboardURL;
        public string DashboardID;
        public string URL;
        public string RedirectURL;
        
        public string DocNumber;
        public int PageNumber;

        public string HtmlContent;
        public string FileContent;

        public string FilePath;

        public bool NeedLoadNextPage;
        public int CountMessages;

        public override string ToString()
        {
            string str = String.Empty;

            str += "URL: <a href='" + URL + "'>" + URL + "</a>;\r\n";
            str += "RedirectURL: <a href='" + RedirectURL + "'></a>;\r\n";
            str += "DocNumber: " + DocNumber + ";\r\n";
            str += "PageNumber: " + PageNumber.ToString() + ";\r\n";
            //str += "FilePath: " + FilePath + ";\r\n";

            return str;
        }
    }
}
