using System;

namespace BH.BaseRobot
{
    public class RunNowTimeTable : TimeTable
    {
        public RunNowTimeTable()
                : base(null, null)
        {
        }

        public override TimeSpan? HowLongToWait(DateTime nowDate)
        {
            return TimeSpan.FromSeconds(0);
        }

        public override string ToString()
        {
            return $"Run now";
        }
    }
}
