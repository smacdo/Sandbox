using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
    public struct ObjFace
    {
        public ObjVertexIndex A { get; set; }
        public ObjVertexIndex B { get; set; }
        public ObjVertexIndex C { get; set; }
    }

    public struct ObjVertexIndex
    {
        // Vertex position index.
        public int V { get; set; }

        // Vertex texture index.
        public int T { get; set; }

        // Vertex normal inde.x
        public int N { get; set; }
    }

    // TODO: Make all properties IReadonlyList since their values should not be modified after initialization.
    public class ObjData
    {
        public IList<Vector3> Vertices { get; private set; }
        public IList<Vector2> TexCoords { get; private set; }
        public IList<Vector3> Normals { get; private set; }
        public IList<ObjFace> Faces { get; private set; }

        /// <summary>
        ///  Constructor.
        /// </summary>
        /// <param name="vertices">List of vertex positions.</param>
        /// <param name="texCoords">List of vertex texture coordinates.</param>
        /// <param name="normals">List of vertex normals.</param>
        /// <param name="faces">List of faces to draw.</param>
        public ObjData(
            IEnumerable<Vector3> vertices,
            IEnumerable<Vector2> texCoords,
            IEnumerable<Vector3> normals,
            IEnumerable<ObjFace> faces)
        {
            Vertices = new List<Vector3>(vertices);
            TexCoords = new List<Vector2>(texCoords);
            Normals = new List<Vector3>(normals);
            Faces = new List<ObjFace>(faces);

            // TODO: Error check.
        }

        public StaticModelData ConvertToStaticModel()
        {
            // Dumb conversion: Convert each face group into a unique vertex.
            //  TODO: Make this smarter, and have identical face groups point to same vertex.
            var vertices = new List<StaticVertex>();
            var indices = new List<int>();

            foreach (var face in Faces)
            {
                // make sure to subtract one from face index, because it uses one based indexing.
                vertices.Add(new StaticVertex
                {
                    Position = Vertices[face.A.V - 1],
                    TexCoord = TexCoords[face.A.T - 1],
                    Normal = Normals[face.A.N - 1]
                });
                indices.Add(vertices.Count - 1);

                vertices.Add(new StaticVertex
                {
                    Position = Vertices[face.B.V - 1],
                    TexCoord = TexCoords[face.B.T - 1],
                    Normal = Normals[face.B.N - 1]
                });
                indices.Add(vertices.Count - 1);

                vertices.Add(new StaticVertex
                {
                    Position = Vertices[face.C.V - 1],
                    TexCoord = TexCoords[face.C.T - 1],
                    Normal = Normals[face.C.N - 1]
                });
                indices.Add(vertices.Count - 1);
            }

            // Create and return new static model.
            return new StaticModelData
            {
                Vertices = vertices,
                Indices = indices
            };
        }
    }
}
