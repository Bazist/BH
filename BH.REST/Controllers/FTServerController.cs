using System.Collections.Generic;
using BH.FTServer;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

namespace BH.REST.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class FTServerController : ControllerBase
    {
        private readonly ILogger<FTServerController> _logger;

        public FTServerController(ILogger<FTServerController> logger)
        {
            _logger = logger;
        }

        #region Members

        private static FTService _ftService;

        #endregion

        #region Methods

        public bool IsStarted() => _ftService.IsStarted();

        [HttpGet]
        public FTSearch.ConfigurationDLL GetConfiguration() => _ftService.GetConfiguration();

        [HttpGet]
        public FTSearch.ConfigurationDLL GetDefaultConfiguration() => _ftService.GetDefaultConfiguration();

        [HttpPost]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration) => _ftService.SetConfiguration(configuration);

        [HttpPost]
        public void Start(int instanceNumber = 0) => _ftService.Start(instanceNumber);

        [HttpGet]
        public FTSearch.SearchResult SearchPhrase(string phrase,
                                                  string templateName,
                                                  int skip,
                                                  int take) => _ftService.SearchPhrase(phrase, templateName, skip, take);

        [HttpGet]
        public FTSearch.SearchResult SearchQuery(List<FTSearch.Selector> selectors,
                                                 int minPage,
                                                 int maxPage,
                                                 int skip,
                                                 bool agregateBySubject) => _ftService.SearchQuery(selectors, minPage, maxPage, skip, agregateBySubject);
        [HttpGet]
        public FTSearch.SearchResult SearchPhraseRel(string phrase,
                                                     int minPage,
                                                     int maxPage,
                                                     int skip,
                                                     int take) => _ftService.SearchPhraseRel(phrase, minPage, maxPage, skip, take);

        [HttpPost]
        public void InitSearchRel() => _ftService.InitSearchRel();

        [HttpGet]
        public string StemContent(string contentText) => _ftService.StemContent(contentText);

        [HttpGet]
        public string LoadDocumentContent(string documentName,
                                          string documentVersion,
                                          string robotName) => _ftService.LoadDocumentContent(documentName, documentVersion, robotName);

        [HttpGet]
        public string CalculateTrend(string phrase,
                                     int count,
                                     int minPage,
                                     int maxPage) => _ftService.CalculateTrend(phrase, count, minPage, maxPage);

        [HttpGet]
        public BH.FTServer.FTService.Info GetInfo() => _ftService.GetInfo();

        [HttpPost]
        public bool IndexText(string documentName,
                              string documentVersion,
                              string contentText,
                              string robotName) => _ftService.IndexText(documentName, documentVersion, contentText, robotName);

        [HttpPost]
        public bool IndexFile(string documentName,
                              string documentVersion,
                              string filePath,
                              string robotName) => _ftService.IndexFile(documentName, documentVersion, filePath, robotName);


        [HttpPost]
        public void SaveIndex() => _ftService.SaveIndex();

        [HttpPost]
        public void MergeIndexes() => _ftService.MergeIndexes();

        [HttpPost]
        public void Stop() => _ftService.Stop();

        [HttpPost]
        public void CheckIndexes() => _ftService.CheckIndexes();

        [HttpPost]
        public void ClearInstance() => _ftService.ClearInstance();

        #endregion
    }
}
