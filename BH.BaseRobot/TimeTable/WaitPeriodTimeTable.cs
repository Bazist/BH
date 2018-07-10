using System;

namespace BH.BaseRobot
{
    public class WaitPeriodTimeTable : TimeTable
    {
        public WaitPeriodTimeTable(DateTime? startDate, TimeSpan period)
                : base(startDate, period)
        {
        }

        public override TimeSpan? HowLongToWait(DateTime nowDate)
        {
            if (Date != null && Date.Value < nowDate)
                return Date.Value.Subtract(nowDate) + Period.Value;
            else
                return Period.Value;
        }

        public override string ToString()
        {
            return $"Start from {Date} and run every {Period} time span";
        }
    }
}
