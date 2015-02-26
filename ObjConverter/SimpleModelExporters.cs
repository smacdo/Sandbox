using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ObjConverter
{
	public class SimpleModelTextFormatExporter : IModelExporter
	{
		public void Export(StaticModelData model, Stream output)
		{
			var writer = new StreamWriter(output);

			// Write model header.
			writer.Write(
				"#SimpleModelv1{0}",
				Environment.NewLine);

			// Write file header.
			writer.Write(
				"{0} {1}{2}",
				model.Vertices.Count,
				model.Indices.Count,
				Environment.NewLine);

			// Write vertices.
			foreach (var v in model.Vertices)
			{
				writer.Write(
					"{0} {1} {2} {3} {4} {5} {6} {7}{8}",
					v.Position.X, v.Position.Y, v.Position.Z,
					v.TexCoord.X, v.TexCoord.Y,
					v.Normal.X, v.Normal.Y, v.Normal.Z,
					Environment.NewLine);
			}

			// Write indices.
			for (int i = 0; i < model.Indices.Count; i += 3)
			{
				writer.Write(
					"{0} {1} {2}{3}",
					model.Indices[i], model.Indices[i + 1], model.Indices[i + 2],
					Environment.NewLine);
			}
		}
	}

	public class SimpleModelBinaryFormatExporter : IModelExporter
	{
		public void Export(StaticModelData model, Stream output)
		{
			var writer = new BinaryWriter(output);

			// Write the binary data header ("SMF\0") that identifies this stream of bytes as a binary simple model
			// format.
			writer.Write(new byte[] { 83, 77, 70, 0 });

			// Write the current version of the SMF format (1).
			writer.Write((byte) 1);

			// Write the number of vertices and then the number of indices in the model.
			writer.Write((UInt32) model.Vertices.Count);
			writer.Write((UInt32) model.Indices.Count);

			// Write each vertex out.
			foreach (var v in model.Vertices)
			{
				writer.Write((float) v.Position.X);
				writer.Write((float) v.Position.Y);
				writer.Write((float) v.Position.Z);

				writer.Write((float) v.TexCoord.X);
				writer.Write((float) v.TexCoord.Y);

				writer.Write((float) v.Normal.X);
				writer.Write((float) v.Normal.Y);
				writer.Write((float) v.Normal.Z);
			}

			// Write each index out.
			for (int i = 0; i < model.Indices.Count; i += 3)
			{
				writer.Write((UInt32) model.Indices[0]);
				writer.Write((UInt32) model.Indices[1]);
				writer.Write((UInt32) model.Indices[2]);
			}
		}
	}
}
