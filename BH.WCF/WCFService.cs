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

using BH.FTServer;
using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Description;

namespace BH.WCF
{


    [ServiceContract]
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.Single)]
    public class WCFService
    {
        #region Members

        private static FTService _ftService;

        private static ServiceHost _host;

        #endregion

        #region Methods

        public bool IsStarted() => _ftService.IsStarted();

        [OperationContract]
        public FTSearch.ConfigurationDLL GetConfiguration() => _ftService.GetConfiguration();

        [OperationContract]
        public FTSearch.ConfigurationDLL GetDefaultConfiguration() => _ftService.GetDefaultConfiguration();

        [OperationContract]
        public void SetConfiguration(FTSearch.ConfigurationDLL configuration) => _ftService.SetConfiguration(configuration);

        [OperationContract]
        public void Start(int instanceNumber = 0) => _ftService.Start(instanceNumber);

        [OperationContract]
        public FTSearch.SearchResult SearchPhrase(string phrase,
                                                  string templateName,
                                                  int skip,
                                                  int take) => _ftService.SearchPhrase(phrase, templateName, skip, take);

        [OperationContract]
        public FTSearch.SearchResult SearchQuery(List<FTSearch.Selector> selectors,
                                                 int minPage,
                                                 int maxPage,
                                                 int skip,
                                                 bool agregateBySubject) => _ftService.SearchQuery(selectors, minPage, maxPage, skip, agregateBySubject);
        [OperationContract]
        public FTSearch.SearchResult SearchPhraseRel(string phrase,
                                                     int minPage,
                                                     int maxPage,
                                                     int skip,
                                                     int take) => _ftService.SearchPhraseRel(phrase, minPage, maxPage, skip, take);

        [OperationContract]
        public void InitSearchRel() => _ftService.InitSearchRel();

        [OperationContract]
        public string StemContent(string contentText) => _ftService.StemContent(contentText);

        [OperationContract]
        public string LoadDocumentContent(string documentName,
                                          string documentVersion,
                                          string robotName) => _ftService.LoadDocumentContent(documentName, documentVersion, robotName);

        [OperationContract]
        public string CalculateTrend(string phrase,
                                     int count,
                                     int minPage,
                                     int maxPage) => _ftService.CalculateTrend(phrase, count, minPage, maxPage);

        [OperationContract]
        public BH.FTServer.FTService.Info GetInfo() => _ftService.GetInfo();

        [OperationContract]
        public bool IndexText(string documentName,
                              string documentVersion,
                              string contentText,
                              string robotName) => _ftService.IndexText(documentName, documentVersion, contentText, robotName);

        [OperationContract]
        public bool IndexFile(string documentName,
                              string documentVersion,
                              string filePath,
                              string robotName) => _ftService.IndexFile(documentName, documentVersion, filePath, robotName);


        [OperationContract]
        public void SaveIndex() => _ftService.SaveIndex();

        [OperationContract]
        public void MergeIndexes() => _ftService.MergeIndexes();

        [OperationContract]
        public void Stop() => _ftService.Stop();

        [OperationContract]
        public void CheckIndexes() => _ftService.CheckIndexes();

        [OperationContract]
        public void ClearInstance() => _ftService.ClearInstance();

        public static void StartWebservice(FTService service,
                                           string url)
        {
            _ftService = service;

            Uri serviceUri = new Uri(url);

            _host = new ServiceHost(service, serviceUri);

            var basicHttpBinding = new BasicHttpBinding();
            basicHttpBinding.MaxReceivedMessageSize = int.MaxValue;
            basicHttpBinding.MaxBufferSize = int.MaxValue;

            var smb = new ServiceMetadataBehavior();
            smb.HttpGetEnabled = true;
            smb.MetadataExporter.PolicyVersion = PolicyVersion.Policy15;

            _host.Description.Behaviors.Add(smb);

            Type serviceType = typeof(FTService);
            _host.AddServiceEndpoint(serviceType, basicHttpBinding, serviceUri);
            _host.Open();
        }

        public static void StopWebService()
        {
            if (_host != null)
            {
                _host.Abort();
            }
        }
                
        #endregion
    }
}
