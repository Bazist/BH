﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.42000
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace FTSearchWeb.BH {
    using System.Runtime.Serialization;
    using System;
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="FTSearch.ConfigurationDLL", Namespace="http://schemas.datacontract.org/2004/07/FTSearchNet")]
    [System.SerializableAttribute()]
    public partial struct FTSearchConfigurationDLL : System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool AutoSaveIndexField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint AutoStemmingOnField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint CountWordInPhraseField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint DocumentNameSizeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private byte[] IndexPathField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint InstanceNumberField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsCreateNewInstanceOnUpdateField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsCustomPathField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsUseEnglishAlphabetField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsUseNumberAlphabetField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsUseRussianAlphabetField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool IsUseUkranianAlphabetField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint LimitTopResultsField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong LimitUsedMemoryField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint MaxLenWordField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint MemoryModeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint MinLenWordField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint RelevantLevelField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint WordsHeaderBaseField;
        
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool AutoSaveIndex {
            get {
                return this.AutoSaveIndexField;
            }
            set {
                if ((this.AutoSaveIndexField.Equals(value) != true)) {
                    this.AutoSaveIndexField = value;
                    this.RaisePropertyChanged("AutoSaveIndex");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint AutoStemmingOn {
            get {
                return this.AutoStemmingOnField;
            }
            set {
                if ((this.AutoStemmingOnField.Equals(value) != true)) {
                    this.AutoStemmingOnField = value;
                    this.RaisePropertyChanged("AutoStemmingOn");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint CountWordInPhrase {
            get {
                return this.CountWordInPhraseField;
            }
            set {
                if ((this.CountWordInPhraseField.Equals(value) != true)) {
                    this.CountWordInPhraseField = value;
                    this.RaisePropertyChanged("CountWordInPhrase");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint DocumentNameSize {
            get {
                return this.DocumentNameSizeField;
            }
            set {
                if ((this.DocumentNameSizeField.Equals(value) != true)) {
                    this.DocumentNameSizeField = value;
                    this.RaisePropertyChanged("DocumentNameSize");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public byte[] IndexPath {
            get {
                return this.IndexPathField;
            }
            set {
                if ((object.ReferenceEquals(this.IndexPathField, value) != true)) {
                    this.IndexPathField = value;
                    this.RaisePropertyChanged("IndexPath");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint InstanceNumber {
            get {
                return this.InstanceNumberField;
            }
            set {
                if ((this.InstanceNumberField.Equals(value) != true)) {
                    this.InstanceNumberField = value;
                    this.RaisePropertyChanged("InstanceNumber");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsCreateNewInstanceOnUpdate {
            get {
                return this.IsCreateNewInstanceOnUpdateField;
            }
            set {
                if ((this.IsCreateNewInstanceOnUpdateField.Equals(value) != true)) {
                    this.IsCreateNewInstanceOnUpdateField = value;
                    this.RaisePropertyChanged("IsCreateNewInstanceOnUpdate");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsCustomPath {
            get {
                return this.IsCustomPathField;
            }
            set {
                if ((this.IsCustomPathField.Equals(value) != true)) {
                    this.IsCustomPathField = value;
                    this.RaisePropertyChanged("IsCustomPath");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsUseEnglishAlphabet {
            get {
                return this.IsUseEnglishAlphabetField;
            }
            set {
                if ((this.IsUseEnglishAlphabetField.Equals(value) != true)) {
                    this.IsUseEnglishAlphabetField = value;
                    this.RaisePropertyChanged("IsUseEnglishAlphabet");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsUseNumberAlphabet {
            get {
                return this.IsUseNumberAlphabetField;
            }
            set {
                if ((this.IsUseNumberAlphabetField.Equals(value) != true)) {
                    this.IsUseNumberAlphabetField = value;
                    this.RaisePropertyChanged("IsUseNumberAlphabet");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsUseRussianAlphabet {
            get {
                return this.IsUseRussianAlphabetField;
            }
            set {
                if ((this.IsUseRussianAlphabetField.Equals(value) != true)) {
                    this.IsUseRussianAlphabetField = value;
                    this.RaisePropertyChanged("IsUseRussianAlphabet");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool IsUseUkranianAlphabet {
            get {
                return this.IsUseUkranianAlphabetField;
            }
            set {
                if ((this.IsUseUkranianAlphabetField.Equals(value) != true)) {
                    this.IsUseUkranianAlphabetField = value;
                    this.RaisePropertyChanged("IsUseUkranianAlphabet");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint LimitTopResults {
            get {
                return this.LimitTopResultsField;
            }
            set {
                if ((this.LimitTopResultsField.Equals(value) != true)) {
                    this.LimitTopResultsField = value;
                    this.RaisePropertyChanged("LimitTopResults");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong LimitUsedMemory {
            get {
                return this.LimitUsedMemoryField;
            }
            set {
                if ((this.LimitUsedMemoryField.Equals(value) != true)) {
                    this.LimitUsedMemoryField = value;
                    this.RaisePropertyChanged("LimitUsedMemory");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint MaxLenWord {
            get {
                return this.MaxLenWordField;
            }
            set {
                if ((this.MaxLenWordField.Equals(value) != true)) {
                    this.MaxLenWordField = value;
                    this.RaisePropertyChanged("MaxLenWord");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint MemoryMode {
            get {
                return this.MemoryModeField;
            }
            set {
                if ((this.MemoryModeField.Equals(value) != true)) {
                    this.MemoryModeField = value;
                    this.RaisePropertyChanged("MemoryMode");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint MinLenWord {
            get {
                return this.MinLenWordField;
            }
            set {
                if ((this.MinLenWordField.Equals(value) != true)) {
                    this.MinLenWordField = value;
                    this.RaisePropertyChanged("MinLenWord");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint RelevantLevel {
            get {
                return this.RelevantLevelField;
            }
            set {
                if ((this.RelevantLevelField.Equals(value) != true)) {
                    this.RelevantLevelField = value;
                    this.RaisePropertyChanged("RelevantLevel");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint WordsHeaderBase {
            get {
                return this.WordsHeaderBaseField;
            }
            set {
                if ((this.WordsHeaderBaseField.Equals(value) != true)) {
                    this.WordsHeaderBaseField = value;
                    this.RaisePropertyChanged("WordsHeaderBase");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="FTSearch.Result", Namespace="http://schemas.datacontract.org/2004/07/FTSearchNet")]
    [System.SerializableAttribute()]
    public partial struct FTSearchResult : System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private string NameField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private FTSearchWeb.BH.FTSearchResultPosition[] PositionsField;
        
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public string Name {
            get {
                return this.NameField;
            }
            set {
                if ((object.ReferenceEquals(this.NameField, value) != true)) {
                    this.NameField = value;
                    this.RaisePropertyChanged("Name");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public FTSearchWeb.BH.FTSearchResultPosition[] Positions {
            get {
                return this.PositionsField;
            }
            set {
                if ((object.ReferenceEquals(this.PositionsField, value) != true)) {
                    this.PositionsField = value;
                    this.RaisePropertyChanged("Positions");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="FTSearch.ResultPosition", Namespace="http://schemas.datacontract.org/2004/07/FTSearchNet")]
    [System.SerializableAttribute()]
    public partial struct FTSearchResultPosition : System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private int LengthField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private int StartPositionField;
        
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public int Length {
            get {
                return this.LengthField;
            }
            set {
                if ((this.LengthField.Equals(value) != true)) {
                    this.LengthField = value;
                    this.RaisePropertyChanged("Length");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public int StartPosition {
            get {
                return this.StartPositionField;
            }
            set {
                if ((this.StartPositionField.Equals(value) != true)) {
                    this.StartPositionField = value;
                    this.RaisePropertyChanged("StartPosition");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "4.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="FTService.Info", Namespace="http://schemas.datacontract.org/2004/07/FTSearchNet")]
    [System.SerializableAttribute()]
    public partial struct FTServiceInfo : System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint AmountInstancesField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong CountWordsHDDField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong CountWordsRAMField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint DocumentNameSizeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private bool HasErrorField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong IndexSizeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private string LastErrorMessageField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint LastNameIDHDDField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint LastNameIDRAMField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint RelevantLevelField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong TextSizeField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong TotalMemoryField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private ulong UsedMemoryField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint VersionField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private uint WordsHeaderBaseField;
        
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint AmountInstances {
            get {
                return this.AmountInstancesField;
            }
            set {
                if ((this.AmountInstancesField.Equals(value) != true)) {
                    this.AmountInstancesField = value;
                    this.RaisePropertyChanged("AmountInstances");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong CountWordsHDD {
            get {
                return this.CountWordsHDDField;
            }
            set {
                if ((this.CountWordsHDDField.Equals(value) != true)) {
                    this.CountWordsHDDField = value;
                    this.RaisePropertyChanged("CountWordsHDD");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong CountWordsRAM {
            get {
                return this.CountWordsRAMField;
            }
            set {
                if ((this.CountWordsRAMField.Equals(value) != true)) {
                    this.CountWordsRAMField = value;
                    this.RaisePropertyChanged("CountWordsRAM");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint DocumentNameSize {
            get {
                return this.DocumentNameSizeField;
            }
            set {
                if ((this.DocumentNameSizeField.Equals(value) != true)) {
                    this.DocumentNameSizeField = value;
                    this.RaisePropertyChanged("DocumentNameSize");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public bool HasError {
            get {
                return this.HasErrorField;
            }
            set {
                if ((this.HasErrorField.Equals(value) != true)) {
                    this.HasErrorField = value;
                    this.RaisePropertyChanged("HasError");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong IndexSize {
            get {
                return this.IndexSizeField;
            }
            set {
                if ((this.IndexSizeField.Equals(value) != true)) {
                    this.IndexSizeField = value;
                    this.RaisePropertyChanged("IndexSize");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public string LastErrorMessage {
            get {
                return this.LastErrorMessageField;
            }
            set {
                if ((object.ReferenceEquals(this.LastErrorMessageField, value) != true)) {
                    this.LastErrorMessageField = value;
                    this.RaisePropertyChanged("LastErrorMessage");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint LastNameIDHDD {
            get {
                return this.LastNameIDHDDField;
            }
            set {
                if ((this.LastNameIDHDDField.Equals(value) != true)) {
                    this.LastNameIDHDDField = value;
                    this.RaisePropertyChanged("LastNameIDHDD");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint LastNameIDRAM {
            get {
                return this.LastNameIDRAMField;
            }
            set {
                if ((this.LastNameIDRAMField.Equals(value) != true)) {
                    this.LastNameIDRAMField = value;
                    this.RaisePropertyChanged("LastNameIDRAM");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint RelevantLevel {
            get {
                return this.RelevantLevelField;
            }
            set {
                if ((this.RelevantLevelField.Equals(value) != true)) {
                    this.RelevantLevelField = value;
                    this.RaisePropertyChanged("RelevantLevel");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong TextSize {
            get {
                return this.TextSizeField;
            }
            set {
                if ((this.TextSizeField.Equals(value) != true)) {
                    this.TextSizeField = value;
                    this.RaisePropertyChanged("TextSize");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong TotalMemory {
            get {
                return this.TotalMemoryField;
            }
            set {
                if ((this.TotalMemoryField.Equals(value) != true)) {
                    this.TotalMemoryField = value;
                    this.RaisePropertyChanged("TotalMemory");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public ulong UsedMemory {
            get {
                return this.UsedMemoryField;
            }
            set {
                if ((this.UsedMemoryField.Equals(value) != true)) {
                    this.UsedMemoryField = value;
                    this.RaisePropertyChanged("UsedMemory");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint Version {
            get {
                return this.VersionField;
            }
            set {
                if ((this.VersionField.Equals(value) != true)) {
                    this.VersionField = value;
                    this.RaisePropertyChanged("Version");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public uint WordsHeaderBase {
            get {
                return this.WordsHeaderBaseField;
            }
            set {
                if ((this.WordsHeaderBaseField.Equals(value) != true)) {
                    this.WordsHeaderBaseField = value;
                    this.RaisePropertyChanged("WordsHeaderBase");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(ConfigurationName="BH.FTService")]
    public interface FTService {
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/IsStarted", ReplyAction="http://tempuri.org/FTService/IsStartedResponse")]
        bool IsStarted();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/IsStarted", ReplyAction="http://tempuri.org/FTService/IsStartedResponse")]
        System.Threading.Tasks.Task<bool> IsStartedAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetConfiguration", ReplyAction="http://tempuri.org/FTService/GetConfigurationResponse")]
        FTSearchWeb.BH.FTSearchConfigurationDLL GetConfiguration();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetConfiguration", ReplyAction="http://tempuri.org/FTService/GetConfigurationResponse")]
        System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchConfigurationDLL> GetConfigurationAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetDefaultConfiguration", ReplyAction="http://tempuri.org/FTService/GetDefaultConfigurationResponse")]
        FTSearchWeb.BH.FTSearchConfigurationDLL GetDefaultConfiguration();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetDefaultConfiguration", ReplyAction="http://tempuri.org/FTService/GetDefaultConfigurationResponse")]
        System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchConfigurationDLL> GetDefaultConfigurationAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SetConfiguration", ReplyAction="http://tempuri.org/FTService/SetConfigurationResponse")]
        void SetConfiguration(FTSearchWeb.BH.FTSearchConfigurationDLL configuration);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SetConfiguration", ReplyAction="http://tempuri.org/FTService/SetConfigurationResponse")]
        System.Threading.Tasks.Task SetConfigurationAsync(FTSearchWeb.BH.FTSearchConfigurationDLL configuration);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/Start", ReplyAction="http://tempuri.org/FTService/StartResponse")]
        void Start(int instanceNumber);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/Start", ReplyAction="http://tempuri.org/FTService/StartResponse")]
        System.Threading.Tasks.Task StartAsync(int instanceNumber);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SearchPhrase", ReplyAction="http://tempuri.org/FTService/SearchPhraseResponse")]
        FTSearchWeb.BH.FTSearchResult[] SearchPhrase(string phrase, int skip, int take);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SearchPhrase", ReplyAction="http://tempuri.org/FTService/SearchPhraseResponse")]
        System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchResult[]> SearchPhraseAsync(string phrase, int skip, int take);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetInfo", ReplyAction="http://tempuri.org/FTService/GetInfoResponse")]
        FTSearchWeb.BH.FTServiceInfo GetInfo();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/GetInfo", ReplyAction="http://tempuri.org/FTService/GetInfoResponse")]
        System.Threading.Tasks.Task<FTSearchWeb.BH.FTServiceInfo> GetInfoAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/IndexText", ReplyAction="http://tempuri.org/FTService/IndexTextResponse")]
        bool IndexText(string aliasName, string contentText);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/IndexText", ReplyAction="http://tempuri.org/FTService/IndexTextResponse")]
        System.Threading.Tasks.Task<bool> IndexTextAsync(string aliasName, string contentText);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SaveIndex", ReplyAction="http://tempuri.org/FTService/SaveIndexResponse")]
        void SaveIndex();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/SaveIndex", ReplyAction="http://tempuri.org/FTService/SaveIndexResponse")]
        System.Threading.Tasks.Task SaveIndexAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/MergeIndexes", ReplyAction="http://tempuri.org/FTService/MergeIndexesResponse")]
        void MergeIndexes();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/MergeIndexes", ReplyAction="http://tempuri.org/FTService/MergeIndexesResponse")]
        System.Threading.Tasks.Task MergeIndexesAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/Stop", ReplyAction="http://tempuri.org/FTService/StopResponse")]
        void Stop();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/Stop", ReplyAction="http://tempuri.org/FTService/StopResponse")]
        System.Threading.Tasks.Task StopAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/CheckIndexes", ReplyAction="http://tempuri.org/FTService/CheckIndexesResponse")]
        void CheckIndexes();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/CheckIndexes", ReplyAction="http://tempuri.org/FTService/CheckIndexesResponse")]
        System.Threading.Tasks.Task CheckIndexesAsync();
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/LoadContent", ReplyAction="http://tempuri.org/FTService/LoadContentResponse")]
        string LoadContent(string name, string aroundPhrase);
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/FTService/LoadContent", ReplyAction="http://tempuri.org/FTService/LoadContentResponse")]
        System.Threading.Tasks.Task<string> LoadContentAsync(string name, string aroundPhrase);
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public interface FTServiceChannel : FTSearchWeb.BH.FTService, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "4.0.0.0")]
    public partial class FTServiceClient : System.ServiceModel.ClientBase<FTSearchWeb.BH.FTService>, FTSearchWeb.BH.FTService {
        
        public FTServiceClient() {
        }
        
        public FTServiceClient(string endpointConfigurationName) : 
                base(endpointConfigurationName) {
        }
        
        public FTServiceClient(string endpointConfigurationName, string remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public FTServiceClient(string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(endpointConfigurationName, remoteAddress) {
        }
        
        public FTServiceClient(System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(binding, remoteAddress) {
        }
        
        public bool IsStarted() {
            return base.Channel.IsStarted();
        }
        
        public System.Threading.Tasks.Task<bool> IsStartedAsync() {
            return base.Channel.IsStartedAsync();
        }
        
        public FTSearchWeb.BH.FTSearchConfigurationDLL GetConfiguration() {
            return base.Channel.GetConfiguration();
        }
        
        public System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchConfigurationDLL> GetConfigurationAsync() {
            return base.Channel.GetConfigurationAsync();
        }
        
        public FTSearchWeb.BH.FTSearchConfigurationDLL GetDefaultConfiguration() {
            return base.Channel.GetDefaultConfiguration();
        }
        
        public System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchConfigurationDLL> GetDefaultConfigurationAsync() {
            return base.Channel.GetDefaultConfigurationAsync();
        }
        
        public void SetConfiguration(FTSearchWeb.BH.FTSearchConfigurationDLL configuration) {
            base.Channel.SetConfiguration(configuration);
        }
        
        public System.Threading.Tasks.Task SetConfigurationAsync(FTSearchWeb.BH.FTSearchConfigurationDLL configuration) {
            return base.Channel.SetConfigurationAsync(configuration);
        }
        
        public void Start(int instanceNumber) {
            base.Channel.Start(instanceNumber);
        }
        
        public System.Threading.Tasks.Task StartAsync(int instanceNumber) {
            return base.Channel.StartAsync(instanceNumber);
        }
        
        public FTSearchWeb.BH.FTSearchResult[] SearchPhrase(string phrase, int skip, int take) {
            return base.Channel.SearchPhrase(phrase, skip, take);
        }
        
        public System.Threading.Tasks.Task<FTSearchWeb.BH.FTSearchResult[]> SearchPhraseAsync(string phrase, int skip, int take) {
            return base.Channel.SearchPhraseAsync(phrase, skip, take);
        }
        
        public FTSearchWeb.BH.FTServiceInfo GetInfo() {
            return base.Channel.GetInfo();
        }
        
        public System.Threading.Tasks.Task<FTSearchWeb.BH.FTServiceInfo> GetInfoAsync() {
            return base.Channel.GetInfoAsync();
        }
        
        public bool IndexText(string aliasName, string contentText) {
            return base.Channel.IndexText(aliasName, contentText);
        }
        
        public System.Threading.Tasks.Task<bool> IndexTextAsync(string aliasName, string contentText) {
            return base.Channel.IndexTextAsync(aliasName, contentText);
        }
        
        public void SaveIndex() {
            base.Channel.SaveIndex();
        }
        
        public System.Threading.Tasks.Task SaveIndexAsync() {
            return base.Channel.SaveIndexAsync();
        }
        
        public void MergeIndexes() {
            base.Channel.MergeIndexes();
        }
        
        public System.Threading.Tasks.Task MergeIndexesAsync() {
            return base.Channel.MergeIndexesAsync();
        }
        
        public void Stop() {
            base.Channel.Stop();
        }
        
        public System.Threading.Tasks.Task StopAsync() {
            return base.Channel.StopAsync();
        }
        
        public void CheckIndexes() {
            base.Channel.CheckIndexes();
        }
        
        public System.Threading.Tasks.Task CheckIndexesAsync() {
            return base.Channel.CheckIndexesAsync();
        }
        
        public string LoadContent(string name, string aroundPhrase) {
            return base.Channel.LoadContent(name, aroundPhrase);
        }
        
        public System.Threading.Tasks.Task<string> LoadContentAsync(string name, string aroundPhrase) {
            return base.Channel.LoadContentAsync(name, aroundPhrase);
        }
    }
}
