using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;
using System.Data;
using System.IO.Compression;

namespace FTBoobenRobot
{
    public class DBHelpers
    {
        private const string _connString = "Server=localhost;Database=Booben;User Id=sa;Password=1;";

        public static bool HasLabel(string url,
                                    string label)
        {
            using (SqlConnection con = new SqlConnection(_connString))
            {
                //
                // Open the SqlConnection.
                //
                con.Open();
                //
                // The following code uses an SqlCommand based on the SqlConnection.
                //
                using (SqlCommand command = new SqlCommand("HasLabel", con))
                {
                    command.CommandType = CommandType.StoredProcedure;

                    command.Parameters.AddWithValue("@URL", url ?? Convert.DBNull);
                    command.Parameters.AddWithValue("@Label", label ?? Convert.DBNull);

                    return (int)command.ExecuteScalar() == 1;
                }
            }
        }

        public static void SaveLabel(string site,
                                     string url,
                                     string label)
        {
            using (SqlConnection con = new SqlConnection(_connString))
            {
                //
                // Open the SqlConnection.
                //
                con.Open();
                //
                // The following code uses an SqlCommand based on the SqlConnection.
                //
                using (SqlCommand command = new SqlCommand("SaveLabel", con))
                {
                    command.CommandType = CommandType.StoredProcedure;

                    command.Parameters.AddWithValue("@Site", site ?? Convert.DBNull);
                    command.Parameters.AddWithValue("@URL", url ?? Convert.DBNull);
                    command.Parameters.AddWithValue("@Label", label ?? Convert.DBNull);

                    command.ExecuteNonQuery();

                    //using (SqlDataReader reader = command.ExecuteReader())
                    //{
                    //    while (reader.Read())
                    //    {
                    //        Console.WriteLine("{0} {1} {2}",
                    //        reader.GetInt32(0), reader.GetString(1), reader.GetString(2));
                    //    }
                    //}
                }

                con.Close();
            }

        }

        public static void GetPages(Action<ZipArchive, int, SqlDataReader> processPage,
                                    ZipArchive archive = null,
                                    DateTime? startDate = null,
                                    DateTime? endDate = null)
        {
            using (SqlConnection con = new SqlConnection(_connString))
            {
                //
                // Open the SqlConnection.
                //
                con.Open();
                //
                // The following code uses an SqlCommand based on the SqlConnection.
                //
                using (SqlCommand command = new SqlCommand("SELECT * FROM Page WHERE (@StartDate IS NULL OR OnDate >= @StartDate) AND (@EndDate IS NULL OR OnDate < @EndDate) ORDER BY OnDate ASC", con))
                {
                    command.CommandType = CommandType.Text;

                    command.Parameters.AddWithValue("@StartDate", startDate ?? Convert.DBNull);
                    command.Parameters.AddWithValue("@EndDate", endDate ?? Convert.DBNull);
                    
                    using (SqlDataReader reader = command.ExecuteReader())
                    {
                        int curr = 0;

                        while (reader.Read())
                        {
                            processPage(archive, ++curr, reader);
                        }
                    }
                }

                con.Close();
            }
        }


        public static void SavePage(string site,
                                    string url)
        {
            using (SqlConnection con = new SqlConnection(_connString))
            {
                //
                // Open the SqlConnection.
                //
                con.Open();
                //
                // The following code uses an SqlCommand based on the SqlConnection.
                //
                using (SqlCommand command = new SqlCommand("SavePage", con))
                {
                    command.CommandType = CommandType.StoredProcedure;

                    command.Parameters.AddWithValue("@Site", site ?? Convert.DBNull);
                    command.Parameters.AddWithValue("@URL", url ?? Convert.DBNull);

                    command.ExecuteNonQuery();
                }

                con.Close();
            }

        }
    }
}
