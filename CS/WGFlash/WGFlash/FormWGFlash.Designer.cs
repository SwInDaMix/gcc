namespace WindowsFormsApplication1
{
    partial class FormWGFlash
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
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this._cb_port = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this._tb_firmware = new System.Windows.Forms.TextBox();
            this._tb_version = new System.Windows.Forms.TextBox();
            this._btn_browse = new System.Windows.Forms.Button();
            this._pb_program = new System.Windows.Forms.ProgressBar();
            this._btn_program = new System.Windows.Forms.Button();
            this._comport = new System.IO.Ports.SerialPort(this.components);
            this._ofd_firmware = new System.Windows.Forms.OpenFileDialog();
            this._fsw_firmware = new System.IO.FileSystemWatcher();
            this._cb_wgmode = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this._fsw_firmware)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Port:";
            // 
            // _cb_port
            // 
            this._cb_port.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cb_port.Location = new System.Drawing.Point(44, 12);
            this._cb_port.MaxDropDownItems = 40;
            this._cb_port.Name = "_cb_port";
            this._cb_port.Size = new System.Drawing.Size(121, 21);
            this._cb_port.Sorted = true;
            this._cb_port.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 42);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(52, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Firmware:";
            // 
            // _tb_firmware
            // 
            this._tb_firmware.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this._tb_firmware.Location = new System.Drawing.Point(67, 39);
            this._tb_firmware.Name = "_tb_firmware";
            this._tb_firmware.ReadOnly = true;
            this._tb_firmware.Size = new System.Drawing.Size(384, 20);
            this._tb_firmware.TabIndex = 3;
            // 
            // _tb_version
            // 
            this._tb_version.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this._tb_version.Location = new System.Drawing.Point(12, 65);
            this._tb_version.Name = "_tb_version";
            this._tb_version.ReadOnly = true;
            this._tb_version.Size = new System.Drawing.Size(520, 20);
            this._tb_version.TabIndex = 5;
            this._tb_version.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // _btn_browse
            // 
            this._btn_browse.Location = new System.Drawing.Point(457, 37);
            this._btn_browse.Name = "_btn_browse";
            this._btn_browse.Size = new System.Drawing.Size(75, 23);
            this._btn_browse.TabIndex = 6;
            this._btn_browse.Text = "Browse...";
            this._btn_browse.UseVisualStyleBackColor = true;
            this._btn_browse.Click += new System.EventHandler(this._btn_browse_Click);
            // 
            // _pb_program
            // 
            this._pb_program.Location = new System.Drawing.Point(12, 91);
            this._pb_program.Name = "_pb_program";
            this._pb_program.Size = new System.Drawing.Size(520, 23);
            this._pb_program.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this._pb_program.TabIndex = 7;
            // 
            // _btn_program
            // 
            this._btn_program.Enabled = false;
            this._btn_program.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btn_program.Location = new System.Drawing.Point(190, 120);
            this._btn_program.Name = "_btn_program";
            this._btn_program.Size = new System.Drawing.Size(165, 41);
            this._btn_program.TabIndex = 8;
            this._btn_program.Text = "Program";
            this._btn_program.UseVisualStyleBackColor = true;
            this._btn_program.Click += new System.EventHandler(this._btn_program_Click);
            // 
            // _comport
            // 
            this._comport.BaudRate = 115200;
            this._comport.ReadTimeout = 500;
            this._comport.WriteTimeout = 500;
            // 
            // _ofd_firmware
            // 
            this._ofd_firmware.Filter = "Hex (*.hex)|*.hex|All files (*.*)|*.*";
            this._ofd_firmware.Title = "Select firmware file";
            // 
            // _fsw_firmware
            // 
            this._fsw_firmware.EnableRaisingEvents = true;
            this._fsw_firmware.NotifyFilter = System.IO.NotifyFilters.LastWrite;
            this._fsw_firmware.SynchronizingObject = this;
            this._fsw_firmware.Changed += new System.IO.FileSystemEventHandler(this._fsw_firmware_event);
            this._fsw_firmware.Created += new System.IO.FileSystemEventHandler(this._fsw_firmware_event);
            this._fsw_firmware.Deleted += new System.IO.FileSystemEventHandler(this._fsw_firmware_event);
            // 
            // _cb_wgmode
            // 
            this._cb_wgmode.AutoSize = true;
            this._cb_wgmode.Checked = true;
            this._cb_wgmode.CheckState = System.Windows.Forms.CheckState.Checked;
            this._cb_wgmode.Location = new System.Drawing.Point(193, 14);
            this._cb_wgmode.Name = "_cb_wgmode";
            this._cb_wgmode.Size = new System.Drawing.Size(339, 17);
            this._cb_wgmode.TabIndex = 9;
            this._cb_wgmode.Text = "Automatically go to operational mode after successful programming";
            this._cb_wgmode.UseVisualStyleBackColor = true;
            // 
            // FormWGFlash
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 166);
            this.Controls.Add(this._cb_wgmode);
            this.Controls.Add(this._btn_program);
            this.Controls.Add(this._pb_program);
            this.Controls.Add(this._btn_browse);
            this.Controls.Add(this._tb_version);
            this.Controls.Add(this._tb_firmware);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._cb_port);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "FormWGFlash";
            this.Text = "WG Flash Utility 1.0";
            ((System.ComponentModel.ISupportInitialize)(this._fsw_firmware)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox _cb_port;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox _tb_firmware;
        private System.Windows.Forms.TextBox _tb_version;
        private System.Windows.Forms.Button _btn_browse;
        private System.Windows.Forms.ProgressBar _pb_program;
        private System.Windows.Forms.Button _btn_program;
        private System.IO.Ports.SerialPort _comport;
        private System.Windows.Forms.OpenFileDialog _ofd_firmware;
        private System.IO.FileSystemWatcher _fsw_firmware;
        private System.Windows.Forms.CheckBox _cb_wgmode;
    }
}

