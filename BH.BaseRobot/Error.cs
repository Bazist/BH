using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class Error
    {
        public enum Code
        {
            General = 1
        }

        public string Message { get; set; }
        public string CallStack { get; set; }
    }
}
