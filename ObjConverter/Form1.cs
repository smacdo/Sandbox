using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ObjConverter
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private IEnumerable<string> GetInputAsStringList()
        {
            return new List<string>(mInputModel.Text.Split('\n').Select(s => s.Trim()).ToList());
        }

        private void mConvertAction_Click(object sender, EventArgs e)
        {
            var inputLines = GetInputAsStringList();
            var modelData = ParseObjFile(inputLines);
            mOutputModel.Text = WriteModelFile(modelData);
        }

        private class ModelData
        {
            public IList<Vertex> Vertices { get; set; }
            public IList<int> Indices { get; set;}
        }

        private struct Vertex
        {
            public Vector3 Position { get; set; }
            public Vector2 TexCoord { get; set; }
            public Vector3 Normal { get; set; }
        }

        private struct Face
        {
            public VertexIndex A { get; set; }
            public VertexIndex B { get; set; }
            public VertexIndex C { get; set; }
        }

        private struct VertexIndex
        {
            // Vertex position index.
            public int V { get; set; }

            // Vertex texture index.
            public int T { get; set; }

            // Vertex normal inde.x
            public int N { get; set; }
        }

        private ModelData ParseObjFile(IEnumerable<string> objLines)
        {
            var vertices = new List<Vector3>();
            var texCoords = new List<Vector2>();
            var normals = new List<Vector3>();
            var faces = new List<Face>();

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
                    texCoords.Add(new Vector2 {X = x, Y = y});
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
                    // Expand face values.
                    var faceA = parts[1].Split('/');
                    var faceB = parts[2].Split('/');
                    var faceC = parts[3].Split('/');

                    // Convert to left hand system by reading face entries in reverse.
                    faces.Add(new Face
                    {
                        A = new VertexIndex
                        {
                            V = Convert.ToInt32(faceC[0]),
                            T = Convert.ToInt32(faceC[1]),
                            N = Convert.ToInt32(faceC[2])
                        },
                        B = new VertexIndex
                        {
                            V = Convert.ToInt32(faceB[0]),
                            T = Convert.ToInt32(faceB[1]),
                            N = Convert.ToInt32(faceB[2])
                        },
                        C = new VertexIndex
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

            // Dumb conversion: Convert each face group into a unique vertex.
            //  TODO: Make this smarter, and have identical face groups point to same vertex.
            var modelData = new ModelData
            {
                Vertices = new List<Vertex>(),
                Indices = new List<int>()
            };

            foreach (var face in faces)
            {
                // make sure to subtract one from face index, because it uses one based indexing.
                modelData.Vertices.Add(new Vertex
                {
                    Position = vertices[face.A.V - 1],
                    TexCoord = texCoords[face.A.T - 1],
                    Normal = normals[face.A.N - 1]
                });
                modelData.Indices.Add(modelData.Vertices.Count - 1);

                modelData.Vertices.Add(new Vertex
                {
                    Position = vertices[face.B.V - 1],
                    TexCoord = texCoords[face.B.T - 1],
                    Normal = normals[face.B.N - 1]
                });
                modelData.Indices.Add(modelData.Vertices.Count - 1);

                modelData.Vertices.Add(new Vertex
                {
                    Position = vertices[face.C.V - 1],
                    TexCoord = texCoords[face.C.T - 1],
                    Normal = normals[face.C.N - 1]
                });
                modelData.Indices.Add(modelData.Vertices.Count - 1);
            }

            return modelData;
        }

        private string WriteModelFile(ModelData modelData)
        {
            var output = new StringBuilder();

            // Write model header.
            output.AppendFormat("#SimpleModelv1{0}", Environment.NewLine);

            // Write file header.
            output.AppendFormat("{0} {1}{2}",
                                modelData.Vertices.Count,
                                modelData.Indices.Count,
                                Environment.NewLine);

            // Write vertices.
            foreach (var v in modelData.Vertices)
            {
                output.AppendFormat("{0} {1} {2} {3} {4} {5} {6} {7}{8}",
                                    v.Position.X, v.Position.Y, v.Position.Z,
                                    v.TexCoord.X, v.TexCoord.Y,
                                    v.Normal.X, v.Normal.Y, v.Normal.Z,
                                    Environment.NewLine);
            }

            // Write indices.
            for(int i = 0; i < modelData.Indices.Count; i += 3)
            {
                output.AppendFormat("{0} {1} {2}{3}",
                    modelData.Indices[i], modelData.Indices[i + 1], modelData.Indices[i + 2],
                    Environment.NewLine);
            }

            return output.ToString();
        }

        private void GetObjectCounts(
            IEnumerable<string> objFileLines,
            out int vertices,
            out int textures,
            out int normals,
            out int faces)
        {
            vertices = 0;
            textures = 0;
            normals = 0;
            faces = 0;

            foreach (var rawLine in objFileLines)
            {
                var line = rawLine.ToLowerInvariant();

                if (line.StartsWith("vt"))
                {
                    textures++;
                }
                else if (line.StartsWith("tn"))
                {
                    normals++;
                }
            }
        }
    }
}
