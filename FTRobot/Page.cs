using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FTRobot
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
