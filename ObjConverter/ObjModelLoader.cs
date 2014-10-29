using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
    public class ObjModelLoader
    {
        public ObjData LoadFromText(string objTextData)
        {
            // Convert the obj text data into lines, and ensure each line is "clean" for processing.
            var objLines = new List<string>(objTextData.Split('\n').Select(s => s.Trim()).ToList());

            // Obj vertex data lists.
            var vertices = new List<Vector3>();
            var texCoords = new List<Vector2>();
            var normals = new List<Vector3>();
            var faces = new List<ObjFace>();

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

            return new ObjData(vertices, texCoords, normals, faces);
        }


    }
}
