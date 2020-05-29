/*
# Copyright(C) 2010-2020 Viacheslav Makoveichuk (email: slv709@gmail.com, skype: vyacheslavm81)
# This file is part of Black Hole FTS.
#
# Black Hole FTS is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Black Hole FTS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

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
