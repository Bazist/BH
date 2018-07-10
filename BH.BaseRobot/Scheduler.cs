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
