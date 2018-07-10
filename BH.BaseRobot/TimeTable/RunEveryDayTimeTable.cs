using System;

namespace BH.BaseRobot
{
    public class RenEveryDayTimeTable : TimeTable
    {
        public RenEveryDayTimeTable(DateTime date)
                : base(date, null)
        {
        }

        public override TimeSpan? HowLongToWait(DateTime nowDate)
        {
            if (nowDate.TimeOfDay < Date.Value.TimeOfDay)
                return Date.Value.TimeOfDay.Subtract(nowDate.TimeOfDay);
            else
                return nowDate.AddDays(1).Add(Date.Value.TimeOfDay).Subtract(nowDate);
        }

        public override string ToString()
        {
            return $"Run every day at {Date.Value.TimeOfDay}";
        }
    }
}
