using BH.FTServer;
using System.Linq;

namespace BH.REST.Models
{
    public struct ResultPosition
    {
        public int StartPosition;
        
        public int Length;
    }

    public struct Result
    {
        public string Name { get; set; }

        public string Version { get; set; }

        public string RobotName { get; set; }

        public ResultPosition[] Positions { get; set; }

        public static Result From(FTSearch.Result result)
        {
            var nameParts = result.Name.Split(';');

            return new Result
            {
                Name = nameParts[0],
                Version = nameParts[1],
                RobotName = nameParts[2],

                Positions = result.Positions?.Select(x => new ResultPosition
                {
                    StartPosition = x.StartPosition,
                    Length = x.Length
                }).ToArray()
            };
        }
    }
}
