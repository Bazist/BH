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
            if (Date == null)
            {
                Date = nowDate;

                return TimeSpan.FromSeconds(0);
            }
            else
            {
                return null;
            }
        }

        public override string ToString()
        {
            return $"Run now";
        }
    }
}
