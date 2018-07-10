using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Linq;
using System;

namespace BH.BaseRobot
{
    public interface IRobotFactory
    {
        IEnumerable<BaseRobot> CreateRobots();
    }

    public class RobotFactory : IRobotFactory
    {
        private RobotFactory()
        {
        }

        private static IEnumerable<T> GetAssemblies<T>()
        {
            var path = Path.GetFullPath(Assembly.GetEntryAssembly().Location);

            var files = System.IO.Directory.GetFiles(path, "*.dll", System.IO.SearchOption.AllDirectories)
                                           .Except(new[] { Assembly.GetEntryAssembly().Location });


            foreach (var file in files)
            {
                var assembly = Assembly.LoadFile(file);

                if (assembly != null)
                {
                    var types = assembly.GetTypes()
                                        .Where(t => t.IsAssignableFrom(typeof(T)));

                    foreach (var type in types)
                    {
                        yield return (T)Activator.CreateInstance(type);
                    }
                }
            }
        }

        public static IRobotFactory CreateFactory()
        {
            var factories = GetAssemblies<IRobotFactory>();

            if (factories.Any())
                return factories.First();
            else
                return new RobotFactory();
        }

        public IEnumerable<BaseRobot> CreateRobots()
        {
            return GetAssemblies<BaseRobot>();
        }
    }
}
