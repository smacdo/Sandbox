using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
    /// <summary>
    ///  DXSandbox static model data.
    /// </summary>
    public class StaticModelData
    {
        public IList<StaticVertex> Vertices { get; set; }
        public IList<int> Indices { get; set; }

        /// <summary>
        ///  Write the model as text data out to an output stream.
        /// </summary>
        /// <param name="output"></param>
        public void WriteAsTextData(Stream output)
        {
            var writer = new StreamWriter(output);
            writer.Write(GetModelAsTextData());
        }

        /// <summary>
        ///  Write the model as binary data out to an output stream.
        /// </summary>
        /// <param name="output"></param>
        public void WriteAsBinaryData(Stream output)
        {
            var writer = new BinaryWriter(output);

            // Write the binary data header ("SMF\0") that identifies this stream of bytes as a binary simple model
            // format.
            writer.Write(new byte[] {83, 77, 70, 0});

            // Write the current version of the SMF format (1).
            writer.Write((byte)1);

            // Write the number of vertices and then the number of indices in the model.
            writer.Write((UInt32) Vertices.Count);
            writer.Write((UInt32) Indices.Count);

            // Write each vertex out.
            foreach (var v in Vertices)
            {
                writer.Write((float)v.Position.X);
                writer.Write((float)v.Position.Y);
                writer.Write((float)v.Position.Z);

                writer.Write((float)v.TexCoord.X);
                writer.Write((float)v.TexCoord.Y);

                writer.Write((float)v.Normal.X);
                writer.Write((float)v.Normal.Y);
                writer.Write((float)v.Normal.Z);
            }

            // Write each index out.
            for (int i = 0; i < Indices.Count; i += 3)
            {
                writer.Write((UInt32) Indices[0]);
                writer.Write((UInt32) Indices[1]);
                writer.Write((UInt32) Indices[2]);
            }
        }

        /// <summary>
        ///  Get model as text data. (TODO: Remove).
        /// </summary>
        /// <returns></returns>
        public string GetModelAsTextData()
        {
            var output = new StringBuilder();

            // Write model header.
            output.AppendFormat("#SimpleModelv1{0}", Environment.NewLine);

            // Write file header.
            output.AppendFormat("{0} {1}{2}",
                                Vertices.Count,
                                Indices.Count,
                                Environment.NewLine);

            // Write vertices.
            foreach (var v in Vertices)
            {
                output.AppendFormat("{0} {1} {2} {3} {4} {5} {6} {7}{8}",
                                    v.Position.X, v.Position.Y, v.Position.Z,
                                    v.TexCoord.X, v.TexCoord.Y,
                                    v.Normal.X, v.Normal.Y, v.Normal.Z,
                                    Environment.NewLine);
            }

            // Write indices.
            for (int i = 0; i < Indices.Count; i += 3)
            {
                output.AppendFormat("{0} {1} {2}{3}",
                    Indices[i], Indices[i + 1], Indices[i + 2],
                    Environment.NewLine);
            }

            return output.ToString();
        }

        


    }

    /// <summary>
    ///  Vertex data.
    /// </summary>
    public struct StaticVertex
    {
        public Vector3 Position { get; set; }
        public Vector2 TexCoord { get; set; }
        public Vector3 Normal { get; set; }
    }
}
