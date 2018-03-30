namespace WindowsFormsApplication1
{
    partial class Form1
    {
        /// <summary>
        /// Требуется переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Обязательный метод для поддержки конструктора - не изменяйте
        /// содержимое данного метода при помощи редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this._tbR = new System.Windows.Forms.TextBox();
            this._tbG = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this._tbB = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this._lv_kcs = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.lab_color = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this._tbARGB = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(514, 5);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(15, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "R";
            // 
            // _tbR
            // 
            this._tbR.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._tbR.Location = new System.Drawing.Point(535, 2);
            this._tbR.Name = "_tbR";
            this._tbR.Size = new System.Drawing.Size(143, 20);
            this._tbR.TabIndex = 3;
            this._tbR.Text = "0";
            this._tbR.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this._tbR.TextChanged += new System.EventHandler(this._tb_TextChanged);
            // 
            // _tbG
            // 
            this._tbG.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._tbG.Location = new System.Drawing.Point(535, 25);
            this._tbG.Name = "_tbG";
            this._tbG.Size = new System.Drawing.Size(143, 20);
            this._tbG.TabIndex = 5;
            this._tbG.Text = "0";
            this._tbG.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this._tbG.TextChanged += new System.EventHandler(this._tb_TextChanged);
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(514, 28);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(15, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "G";
            // 
            // _tbB
            // 
            this._tbB.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._tbB.Location = new System.Drawing.Point(535, 48);
            this._tbB.Name = "_tbB";
            this._tbB.Size = new System.Drawing.Size(143, 20);
            this._tbB.TabIndex = 7;
            this._tbB.Text = "0";
            this._tbB.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this._tbB.TextChanged += new System.EventHandler(this._tb_TextChanged);
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(514, 51);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(14, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "B";
            // 
            // _lv_kcs
            // 
            this._lv_kcs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._lv_kcs.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader4,
            this.columnHeader5,
            this.columnHeader6,
            this.columnHeader7,
            this.columnHeader8});
            this._lv_kcs.FullRowSelect = true;
            this._lv_kcs.Location = new System.Drawing.Point(1, 2);
            this._lv_kcs.Name = "_lv_kcs";
            this._lv_kcs.Size = new System.Drawing.Size(507, 482);
            this._lv_kcs.TabIndex = 8;
            this._lv_kcs.UseCompatibleStateImageBehavior = false;
            this._lv_kcs.View = System.Windows.Forms.View.Details;
            this._lv_kcs.SelectedIndexChanged += new System.EventHandler(this._lv_kcs_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Known name";
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Red";
            this.columnHeader2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Green";
            this.columnHeader3.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Blue";
            this.columnHeader4.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "Brightness";
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "Hue";
            // 
            // columnHeader7
            // 
            this.columnHeader7.Text = "Saturation";
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "";
            // 
            // lab_color
            // 
            this.lab_color.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lab_color.BackColor = System.Drawing.Color.Black;
            this.lab_color.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lab_color.Location = new System.Drawing.Point(511, 129);
            this.lab_color.Name = "lab_color";
            this.lab_color.Size = new System.Drawing.Size(167, 329);
            this.lab_color.TabIndex = 9;
            this.lab_color.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(511, 103);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(167, 23);
            this.button1.TabIndex = 10;
            this.button1.Text = "Find Nearest Color";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // _tbARGB
            // 
            this._tbARGB.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._tbARGB.Location = new System.Drawing.Point(558, 74);
            this._tbARGB.Name = "_tbARGB";
            this._tbARGB.Size = new System.Drawing.Size(120, 20);
            this._tbARGB.TabIndex = 12;
            this._tbARGB.Text = "0";
            this._tbARGB.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(514, 77);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(37, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "ARGB";
            // 
            // button2
            // 
            this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button2.Location = new System.Drawing.Point(511, 461);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(167, 23);
            this.button2.TabIndex = 13;
            this.button2.Text = "Sort / Unsort";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(679, 485);
            this.Controls.Add(this.button2);
            this.Controls.Add(this._tbARGB);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.lab_color);
            this.Controls.Add(this._lv_kcs);
            this.Controls.Add(this._tbB);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._tbG);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._tbR);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _tbR;
        private System.Windows.Forms.TextBox _tbG;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox _tbB;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ListView _lv_kcs;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.Label lab_color;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox _tbARGB;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button2;
    }
}

