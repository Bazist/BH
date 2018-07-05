using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;

namespace FTBoobenRobot
{
    public class Archive
    {
        public static Encoding Encoding = Encoding.GetEncoding("windows-1251");

        public static void AddFiles(string archivePath,
                                    string directoryPath,
                                    string[] filePaths,
                                    string[] fileContents = null,
                                    Action<int, int> addLog = null)
        {
            const int portion = 20000;

            int startIndex = 0;
            int endIndex = portion;

            for (int i = startIndex; i < filePaths.Length; i++)
            {
                using (FileStream zipToOpen = new FileStream(archivePath, FileMode.OpenOrCreate))
                {
                    using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Update))
                    {
                        for (; i < endIndex && i < filePaths.Length; i++)
                        {
                            string fileContent;

                            if (fileContents != null)
                            {
                                fileContent = fileContents[i];
                            }
                            else
                            {
                                string path = filePaths[i];
                                
                                if (File.Exists(path))
                                {
                                    fileContent = File.ReadAllText(filePaths[i], Archive.Encoding);
                                }
                                else
                                {
                                    continue;
                                }
                            }

                            ZipArchiveEntry zipEntry = archive.CreateEntry(filePaths[i].ToLower().Replace(directoryPath.ToLower(), string.Empty));

                            using (StreamWriter writer = new StreamWriter(zipEntry.Open(), Archive.Encoding))
                            {
                                writer.Write(fileContent);
                            }

                            if (i % 100 == 0)
                            {
                                if (addLog != null)
                                {
                                    addLog(i, filePaths.Length);
                                }
                            }
                        }

                        startIndex += portion;
                        endIndex += portion;
                    }
                }
            }
        }

        public static void ReadFiles(string archivePath,
                                     string directoryPath,
                                     string[] filePaths,
                                     string[] fileContents)
        {
            using (FileStream zipToOpen = new FileStream(archivePath, FileMode.Open))
            {
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
                {
                    for (int i = 0; i < filePaths.Length; i++)
                    {
                        ZipArchiveEntry zipEntry = archive.GetEntry(filePaths[i].ToLower().Replace(directoryPath.ToLower(), string.Empty));

                        using (StreamReader reader = new StreamReader(zipEntry.Open(), Archive.Encoding))
                        {
                            fileContents[i] = reader.ReadToEnd();
                        }
                    }
                }
            }
        }

        public static void GetFiles(string archivePath,
                                    Action<int, int, string, string> processFile = null)
        {
            int curr = 0;

            using (FileStream zipToOpen = new FileStream(archivePath, FileMode.Open))
            {
                using (ZipArchive archive = new ZipArchive(zipToOpen, ZipArchiveMode.Read))
                {
                    foreach (ZipArchiveEntry zipEntry in archive.Entries)
                    {
                        using (StreamReader reader = new StreamReader(zipEntry.Open(), Archive.Encoding))
                        {
                            processFile(++curr, archive.Entries.Count, zipEntry.FullName, reader.ReadToEnd());
                        }
                    }
                }
            }
        }

        //public static void GetFilesInMem(string archivePath,
        //                                 Action<int, int, string, string> processFile = null)
        //{
        //    int curr = 0;

        //    using (FileStream zipToOpen = new FileStream(archivePath, FileMode.Open))
        //    {
        //        using (MemoryStream ms = new MemoryStream())
        //        {
        //            zipToOpen.CopyTo(ms);

        //            using (ZipArchive archive = new ZipArchive(ms, ZipArchiveMode.Read))
        //            {
        //                foreach (ZipArchiveEntry zipEntry in archive.Entries)
        //                {
        //                    using (StreamReader reader = new StreamReader(zipEntry.Open(), _encoding))
        //                    {
        //                        processFile(++curr, archive.Entries.Count, zipEntry.FullName, reader.ReadToEnd());
        //                    }
        //                }
        //            }
        //        }
        //    }
        //}
    }
}
