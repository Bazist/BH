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

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Text;

namespace BH.BoobenRobot
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
