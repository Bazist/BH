using BH.WCF;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BH.BaseRobot
{
    public class Storage
    {
        private FTService _ftService;

        public Storage(FTService ftService)
        {
            _ftService = ftService;
        }

        public FTService.Info GetInfo()
        {
            return _ftService.GetInfo();
        }

        public IEnumerable<File> ReadFileVersions(IEnumerable<File> files)
        {
            return _ftService.ReadDocumentVersions(files.Select(x => x.ToDocumentName())
                                                        .ToList());
        }

        public bool IsFileExists(string name)
        {
            return false;
        }
    }
}
