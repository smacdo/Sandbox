namespace ObjConverter
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.label1 = new System.Windows.Forms.Label();
            this.mConvertAction = new System.Windows.Forms.Button();
            this.mInputModel = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.mOutputModel = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.label1);
            this.splitContainer1.Panel1.Controls.Add(this.mConvertAction);
            this.splitContainer1.Panel1.Controls.Add(this.mInputModel);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.label2);
            this.splitContainer1.Panel2.Controls.Add(this.mOutputModel);
            this.splitContainer1.Size = new System.Drawing.Size(737, 609);
            this.splitContainer1.SplitterDistance = 360;
            this.splitContainer1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(70, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Input: .obj file";
            // 
            // mConvertAction
            // 
            this.mConvertAction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.mConvertAction.Location = new System.Drawing.Point(183, 574);
            this.mConvertAction.Name = "mConvertAction";
            this.mConvertAction.Size = new System.Drawing.Size(174, 23);
            this.mConvertAction.TabIndex = 1;
            this.mConvertAction.Text = "Convert to app model format";
            this.mConvertAction.UseVisualStyleBackColor = true;
            this.mConvertAction.Click += new System.EventHandler(this.mConvertAction_Click);
            // 
            // mInputModel
            // 
            this.mInputModel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mInputModel.Location = new System.Drawing.Point(12, 25);
            this.mInputModel.Multiline = true;
            this.mInputModel.Name = "mInputModel";
            this.mInputModel.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mInputModel.Size = new System.Drawing.Size(345, 543);
            this.mInputModel.TabIndex = 0;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(106, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Output: .txt model file";
            // 
            // mOutputModel
            // 
            this.mOutputModel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mOutputModel.Location = new System.Drawing.Point(3, 25);
            this.mOutputModel.Multiline = true;
            this.mOutputModel.Name = "mOutputModel";
            this.mOutputModel.ReadOnly = true;
            this.mOutputModel.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.mOutputModel.Size = new System.Drawing.Size(358, 543);
            this.mOutputModel.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(737, 609);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "Obj Converter";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox mInputModel;
        private System.Windows.Forms.TextBox mOutputModel;
        private System.Windows.Forms.Button mConvertAction;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}

