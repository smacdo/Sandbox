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
    public partial class SimpleModelConverter : Form
    {
        public SimpleModelConverter()
        {
            InitializeComponent();
        }

        private void mConvertAction_Click(object sender, EventArgs e)
        {
			byte[] byteArray = Encoding.UTF8.GetBytes(mInputModel.Text);
			var inputStream = new MemoryStream(byteArray);

			var outputStream = new MemoryStream();

            ConvertFile(inputStream, outputStream, ModelFileFormat.Obj);

			var outputReader = new StreamReader(outputStream);
			string outputAsText = outputReader.ReadToEnd();

			mOutputModel.Text = outputAsText;
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void importModelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            var result = importModelFilePicker.ShowDialog();

            if (result == System.Windows.Forms.DialogResult.OK)
            {
                
            }
        }

        private void ConvertFile(Stream input, Stream output, ModelFileFormat format)
        {
            var modelLoader = new ObjModelLoader();
            var modelExporter = new SimpleModelTextFormatExporter();

            // Import model.
            var modelData = modelLoader.Import(input);
            modelExporter.Export(modelData, output);
        }
    }
}
