using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public enum ErrorCode
    {
        General = 1
    }

    public class Error
    {
        public ErrorCode Code { get; }
        public string Message { get; }
        public string CallStack { get; }
    }
}
