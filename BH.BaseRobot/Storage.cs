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

        public IDictionary<string, string> ReadDocumentVersions(IEnumerable<Document> documents)
        {
            return _ftService.ReadDocumentVersions(documents.Select(x => x.Name).ToList());
        }

        public void IndexDocument(Document file)
        {
            if (_ftService.IndexText(file.Name, file.Version, file.Content))
                SaveIndex();
        }

        public bool IsdocumentExists(string name)
        {
            return false;
        }

        public void SaveIndex()
        {
            _ftService.SaveIndex();
        }
    }
}
