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

using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Linq;
using System;

namespace BH.BaseRobot
{
    public interface IRobotFactory
    {
        IEnumerable<IBaseRobot> CreateRobots();
    }

    public class RobotFactory : IRobotFactory
    {
        private RobotFactory()
        {
        }

        private static IEnumerable<T> FindTypes<T>()
        {
            var path = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

            var files = System.IO.Directory.GetFiles(path, "*.dll", System.IO.SearchOption.AllDirectories)
                                           .Where(x => !x.Contains(Assembly.GetExecutingAssembly().Location) && 
                                                       !x.Contains("FTSearch.dll"));


            foreach (var file in files)
            {
                Assembly assembly;

                try
                {
                    assembly = Assembly.LoadFile(file);
                }
                catch (System.BadImageFormatException)
                {
                    continue;
                }

                if (assembly != null)
                {
                    var types = assembly.GetTypes()
                                        .Where(t => t.IsAssignableFrom(typeof(T)) ||
                                                    t.IsSubclassOf(typeof(T)));

                    foreach (var type in types)
                    {
                        yield return (T)Activator.CreateInstance(type);
                    }
                }

            }
        }

        public static IRobotFactory CreateFactory()
        {
            var factories = FindTypes<IRobotFactory>();

            if (factories.Any())
                return factories.First();
            else
                return new RobotFactory();
        }

        public IEnumerable<IBaseRobot> CreateRobots()
        {
            return FindTypes<BaseRobot>().Where(x => x.IsEnabled);
        }
    }
}
