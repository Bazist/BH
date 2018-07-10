using System;

namespace BH.BaseRobot
{
    public class RunOnceTimeTable : TimeTable
    {
        public RunOnceTimeTable(DateTime? date)
                : base(date, null)
        {
        }

        public override TimeSpan? HowLongToWait(DateTime nowDate)
        {
            if (nowDate < Date.Value)
                return Date.Value.Subtract(nowDate);
            else
                return null;
        }

        public override string ToString()
        {
            return $"Run once at {Date}";
        }
    }
}
