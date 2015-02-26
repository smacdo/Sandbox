using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
    public class ObjModelLoader : IModelImporter
    {
		public StaticModelData Import(Stream input)
		{
			var streamReader = new StreamReader(input);
			var inputAsText = streamReader.ReadToEnd();

			var staticModel = ConvertToStaticModel(LoadFromText(inputAsText));
			return staticModel;
		}

		/// <summary>
		///  Loads an obj model from a text string.
		/// </summary>
		/// <param name="objTextData"></param>
		/// <returns></returns>
        private ObjData LoadFromText(string objTextData)
        {
            // Convert the obj text data into lines, and ensure each line is "clean" for processing.
            var objLines = new List<string>(objTextData.Split('\n').Select(s => s.Trim()).ToList());

            // Obj vertex data lists.
            var vertices = new List<Vector3>();
            var texCoords = new List<Vector2>();
            var normals = new List<Vector3>();
            var faces = new List<ObjFace>();

		    FaceType? faceType = null;

            // Read obj file components.
            foreach (var rawLine in objLines)
            {
                var line = rawLine.ToLowerInvariant().Trim();

                // Don't bother with empty lines or commented lines.
                if (line.StartsWith("#") || string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }

                // Split line into parts separated by space.
                var parts = line.Split(new char[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                var command = parts[0];

                // Interpret .obj line command.
                if (command.Equals("vt"))
                {
                    double x = Convert.ToDouble(parts[1]);
                    double y = Convert.ToDouble(parts[2]);

                    // Convert to left hand system by inverting V texture.
                    y = 1.0 - y;

                    // Store vertex texture coordinate.
                    texCoords.Add(new Vector2 { X = x, Y = y });
                }
                else if (command.Equals("vn"))
                {
                    double nx = Convert.ToDouble(parts[1]);
                    double ny = Convert.ToDouble(parts[2]);
                    double nz = Convert.ToDouble(parts[3]);

                    // Convert to left hand system by inverting the z vertex.
                    nz *= -1.0;

                    // Store vertex normal.
                    normals.Add(new Vector3 { X = nx, Y = ny, Z = nz });
                }
                else if (command.Equals("v"))
                {
                    double x = Convert.ToDouble(parts[1]);
                    double y = Convert.ToDouble(parts[2]);
                    double z = Convert.ToDouble(parts[3]);

                    // Convert to left hand system by inverting the z vertex.
                    z *= -1.0;

                    // Store vertex position.
                    vertices.Add(new Vector3 { X = x, Y = y, Z = z });
                }
                else if (command.Equals("f"))
                {
                    if (!faceType.HasValue)
                    {
                        faceType = DetectFaceType(parts[1]);
                    }

                    // Expand face values.
                    var faceA = parts[1].Split('/');
                    var faceB = parts[2].Split('/');
                    var faceC = parts[3].Split('/');

                    // Convert to left hand system by reading face entries in reverse.
                    faces.Add(new ObjFace
                    {
                        A = new ObjVertexIndex
                        {
                            V = Convert.ToInt32(faceC[0]),
                            T = Convert.ToInt32(faceC[1]),
                            N = Convert.ToInt32(faceC[2])
                        },
                        B = new ObjVertexIndex
                        {
                            V = Convert.ToInt32(faceB[0]),
                            T = Convert.ToInt32(faceB[1]),
                            N = Convert.ToInt32(faceB[2])
                        },
                        C = new ObjVertexIndex
                        {
                            V = Convert.ToInt32(faceA[0]),
                            T = Convert.ToInt32(faceA[1]),
                            N = Convert.ToInt32(faceA[2])
                        },
                    });
                }
                else if (command.Equals("mtllib"))
                {
                    var materialFile = parts[1];
                }
                else if (command.Equals("s"))
                {
                    // ignore. sets active smoothing group.
                }
                else if (command.Equals("g"))
                {
                    // group.
                }
                else if (command.Equals("usemtl"))
                {
                    // start using material group.
                }
            }

            return new ObjData
			{
				Vertices = vertices,
				TexCoords = texCoords,
				Normals = normals,
				Faces = faces
			};
        }

		public StaticModelData ConvertToStaticModel(ObjData obj)
		{
			// Dumb conversion: Convert each face group into a unique vertex.
			//  TODO: Make this smarter, and have identical face groups point to same vertex.
			var vertices = new List<StaticVertex>();
			var indices = new List<int>();

			foreach (var face in obj.Faces)
			{
				// make sure to subtract one from face index, because it uses one based indexing.
				vertices.Add(new StaticVertex
				{
					Position = obj.Vertices[face.A.V - 1],
					TexCoord = obj.TexCoords[face.A.T - 1],
					Normal = obj.Normals[face.A.N - 1]
				});
				indices.Add(vertices.Count - 1);

				vertices.Add(new StaticVertex
				{
					Position = obj.Vertices[face.B.V - 1],
					TexCoord = obj.TexCoords[face.B.T - 1],
					Normal = obj.Normals[face.B.N - 1]
				});
				indices.Add(vertices.Count - 1);

				vertices.Add(new StaticVertex
				{
					Position = obj.Vertices[face.C.V - 1],
					TexCoord = obj.TexCoords[face.C.T - 1],
					Normal = obj.Normals[face.C.N - 1]
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

        private ObjFace ParseFace(string facePartA, string facePartB, string facePartC, FaceType type)
        {
            var faceA = facePartA.Split('/');
            var faceB = facePartB.Split('/');
            var faceC = facePartC.Split('/');
            var pos = new int[3];
            var tex = new int[3];
            var normal = new int[3];

            // Convert to left hand system by reading face entries in reverse.
            return (new ObjFace
            {
                A = new ObjVertexIndex
                {
                    V = Convert.ToInt32(faceC[0]),
                    T = Convert.ToInt32(faceC[1]),
                    N = Convert.ToInt32(faceC[2])
                },
                B = new ObjVertexIndex
                {
                    V = Convert.ToInt32(faceB[0]),
                    T = Convert.ToInt32(faceB[1]),
                    N = Convert.ToInt32(faceB[2])
                },
                C = new ObjVertexIndex
                {
                    V = Convert.ToInt32(faceA[0]),
                    T = Convert.ToInt32(faceA[1]),
                    N = Convert.ToInt32(faceA[2])
                },
            });
        }

        private FaceType DetectFaceType(string face)
        {
            var bits = face.Split('/');

            switch (bits.Length)
            {
                case 1:
                    return FaceType.Position;
                case 2:
                    return FaceType.PositionTexture;
                case 3:
                    if (string.IsNullOrWhiteSpace(bits[1]))
                    {
                        return FaceType.PositionNormal;
                    }
                    return FaceType.PoisitionTextureNormal;
                default:
                    throw new InvalidOperationException("Unknown obj face type");
            }
        }

        /// <summary>
		///  Obj face.
		/// </summary>
		public struct ObjFace
		{
			public ObjVertexIndex A { get; set; }
			public ObjVertexIndex B { get; set; }
			public ObjVertexIndex C { get; set; }
		}

		/// <summary>
		///  Obj model vertex index offsets.
		/// </summary>
		public struct ObjVertexIndex
		{
			// Vertex position index.
			public int V { get; set; }

			// Vertex texture index.
			public int T { get; set; }

			// Vertex normal inde.x
			public int N { get; set; }
		}

		public class ObjData
		{
			public IList<Vector3> Vertices { get; set; }
			public IList<Vector2> TexCoords { get; set; }
			public IList<Vector3> Normals { get; set; }
			public IList<ObjFace> Faces { get; set; }
		}

        public enum FaceType
        {
            Position,
            PositionTexture,
            PositionNormal,
            PoisitionTextureNormal
        }
	}
}
