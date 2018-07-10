using System;

namespace BH.BaseRobot
{
    public abstract class TimeTable
    {
        protected TimeTable(DateTime? date, TimeSpan? period)
        {
            Date = date;
            Period = period;
        }

        public DateTime? Date { get; }
        public TimeSpan? Period { get; }

        public abstract TimeSpan? HowLongToWait(DateTime nowDate);
    }
}
