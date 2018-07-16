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

        public DateTime? Date { get; protected set; }
        public TimeSpan? Period { get; protected set; }

        public abstract TimeSpan? HowLongToWait(DateTime nowDate);
    }
}
