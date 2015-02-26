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
