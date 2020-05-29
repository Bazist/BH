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

using System.Collections.Concurrent;
using System.Collections.Generic;

namespace BH.BaseRobot
{
    public class QueueIndexing
    {
        public QueueIndexing()
        {
            _folders = new ConcurrentQueue<Folder>();
            _documents = new ConcurrentQueue<Document>();
            _documentsWithContent = new ConcurrentQueue<Document>();
        }

        private ConcurrentQueue<Folder> _folders;
        private ConcurrentQueue<Document> _documents;
        private ConcurrentQueue<Document> _documentsWithContent;

        public void EnqueFolder(Folder directory)
        {
            _folders.Enqueue(directory);
        }

        public void EnqueFolders(IEnumerable<Folder> folders)
        {
            foreach (var folder in folders)
                EnqueFolder(folder);
        }

        public Folder DequeFolder()
        {
            Folder folder;

            if (_folders.TryDequeue(out folder))
                return folder;
            else
                return null;
        }

        public void EnqueDocument(Document document)
        {
            _documents.Enqueue(document);
        }

        public void EnqueDocuments(IEnumerable<Document> documents)
        {
            foreach (var document in documents)
                EnqueDocument(document);
        }

        public Document DequeDocument()
        {
            Document document;

            if (_documents.TryDequeue(out document))
                return document;
            else
                return null;
        }

        public void EnqueDocumentWithContent(Document document)
        {
            _documentsWithContent.Enqueue(document);
        }

        public Document DequeDocumentWithContent()
        {
            Document document;

            if (_documentsWithContent.TryDequeue(out document))
                return document;
            else
                return null;
        }
    }
}
