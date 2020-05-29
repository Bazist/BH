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
