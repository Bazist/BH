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
using System.Threading;

namespace BH.BaseRobot
{
    public class Scheduler
    {
        public TimeTable TimeTable { get; }
        public Action Action { get; }

        public Scheduler(TimeTable timeTable, Action action)
        {
            TimeTable = timeTable;
            Action = action;

            _thread = new Thread(new ThreadStart(Run)) { IsBackground = true };
        }

        private Thread _thread;

        public void Start()
        {
            _thread.Start();
        }

        public void Abort()
        {
            _thread.Abort();
        }

        private void Run()
        {
            while (true)
            {
                var wait = TimeTable.HowLongToWait(DateTime.Now);

                if (wait != null)
                {
                    Thread.Sleep(wait.Value);

                    Action();
                }
                else
                {
                    break; //don't wait
                }
            }
        }

        public bool IsStarted => _thread.ThreadState == ThreadState.Running;
    }
}
