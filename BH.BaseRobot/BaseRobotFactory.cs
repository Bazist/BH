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
