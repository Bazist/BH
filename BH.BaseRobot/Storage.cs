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

        public IDictionary<string, string> ReadFileVersions(IEnumerable<File> files)
        {
            return _ftService.ReadDocumentVersions(files.Select(x => x.Name).ToList());
        }

        public void IndexFile(File file)
        {
            _ftService.IndexText(file.Name, file.Version, file.Content);
        }

        public bool IsFileExists(string name)
        {
            return false;
        }
    }
}
