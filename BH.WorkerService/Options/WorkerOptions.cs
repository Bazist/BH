namespace BH.WorkerService.Options
{
    public class WorkerOptions
    {
        public string IndexPath { get; set; }

        public ulong LimitUsedMemory { get; set; }

        public bool InMemoryMode { get; set; }

        public bool BoobenMode { get; set; }

        public bool EnableRelSearch { get; set; }
        
        public bool EnableRobots { get; set; }

        public bool IndexCurrentMonth { get; set; }
    }
}
