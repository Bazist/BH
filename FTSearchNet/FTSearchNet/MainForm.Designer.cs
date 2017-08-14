namespace FTSearchNet
{
    partial class MainForm
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
            this.button1 = new System.Windows.Forms.Button();
            this.gbIOSpeed = new System.Windows.Forms.GroupBox();
            this.lbIOSpeedMax = new System.Windows.Forms.Label();
            this.lbIOSpeedMin = new System.Windows.Forms.Label();
            this.pbIOSpeed = new System.Windows.Forms.ProgressBar();
            this.gbIndexSpeed = new System.Windows.Forms.GroupBox();
            this.lbAmountFiles = new System.Windows.Forms.Label();
            this.lbIndexSpeedMax = new System.Windows.Forms.Label();
            this.lbIndexSpeedMin = new System.Windows.Forms.Label();
            this.pbIndexSpeed = new System.Windows.Forms.ProgressBar();
            this.lbInstanceInfo = new System.Windows.Forms.Label();
            this.gbScan = new System.Windows.Forms.GroupBox();
            this.lbScanSpeedMax = new System.Windows.Forms.Label();
            this.lbScanSpeedMin = new System.Windows.Forms.Label();
            this.pbScanSpeed = new System.Windows.Forms.ProgressBar();
            this.lbIndexFolder = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.tbSearchText = new System.Windows.Forms.TextBox();
            this.gbTotalSpeed = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lbTotalSpeedMax = new System.Windows.Forms.Label();
            this.lbTotalSpeedMin = new System.Windows.Forms.Label();
            this.pbTotalSpeed = new System.Windows.Forms.ProgressBar();
            this.rtbLog = new System.Windows.Forms.RichTextBox();
            this.lbArchive = new System.Windows.Forms.Label();
            this.btnImportIndexes = new System.Windows.Forms.Button();
            this.btnCheckIndexes = new System.Windows.Forms.Button();
            this.btnStartJob = new System.Windows.Forms.Button();
            this.gbIOSpeed.SuspendLayout();
            this.gbIndexSpeed.SuspendLayout();
            this.gbScan.SuspendLayout();
            this.gbTotalSpeed.SuspendLayout();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(195, 329);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 0;
            this.button1.Text = "index";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.btnIndex_Click);
            // 
            // gbIOSpeed
            // 
            this.gbIOSpeed.Controls.Add(this.lbIOSpeedMax);
            this.gbIOSpeed.Controls.Add(this.lbIOSpeedMin);
            this.gbIOSpeed.Controls.Add(this.pbIOSpeed);
            this.gbIOSpeed.Location = new System.Drawing.Point(12, 78);
            this.gbIOSpeed.Name = "gbIOSpeed";
            this.gbIOSpeed.Size = new System.Drawing.Size(708, 65);
            this.gbIOSpeed.TabIndex = 5;
            this.gbIOSpeed.TabStop = false;
            this.gbIOSpeed.Text = "I/O Disc Speed";
            // 
            // lbIOSpeedMax
            // 
            this.lbIOSpeedMax.AutoSize = true;
            this.lbIOSpeedMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbIOSpeedMax.Location = new System.Drawing.Point(431, 32);
            this.lbIOSpeedMax.Name = "lbIOSpeedMax";
            this.lbIOSpeedMax.Size = new System.Drawing.Size(39, 15);
            this.lbIOSpeedMax.TabIndex = 7;
            this.lbIOSpeedMax.Text = "0 mb";
            // 
            // lbIOSpeedMin
            // 
            this.lbIOSpeedMin.AutoSize = true;
            this.lbIOSpeedMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbIOSpeedMin.Location = new System.Drawing.Point(11, 31);
            this.lbIOSpeedMin.Name = "lbIOSpeedMin";
            this.lbIOSpeedMin.Size = new System.Drawing.Size(41, 16);
            this.lbIOSpeedMin.TabIndex = 6;
            this.lbIOSpeedMin.Text = "0 mb";
            // 
            // pbIOSpeed
            // 
            this.pbIOSpeed.Location = new System.Drawing.Point(58, 31);
            this.pbIOSpeed.Maximum = 1;
            this.pbIOSpeed.Name = "pbIOSpeed";
            this.pbIOSpeed.Size = new System.Drawing.Size(367, 23);
            this.pbIOSpeed.Step = 1;
            this.pbIOSpeed.TabIndex = 5;
            // 
            // gbIndexSpeed
            // 
            this.gbIndexSpeed.Controls.Add(this.lbAmountFiles);
            this.gbIndexSpeed.Controls.Add(this.lbIndexSpeedMax);
            this.gbIndexSpeed.Controls.Add(this.lbIndexSpeedMin);
            this.gbIndexSpeed.Controls.Add(this.pbIndexSpeed);
            this.gbIndexSpeed.Location = new System.Drawing.Point(12, 149);
            this.gbIndexSpeed.Name = "gbIndexSpeed";
            this.gbIndexSpeed.Size = new System.Drawing.Size(708, 65);
            this.gbIndexSpeed.TabIndex = 9;
            this.gbIndexSpeed.TabStop = false;
            this.gbIndexSpeed.Text = "Index Speed";
            // 
            // lbAmountFiles
            // 
            this.lbAmountFiles.AutoSize = true;
            this.lbAmountFiles.Location = new System.Drawing.Point(11, 16);
            this.lbAmountFiles.Name = "lbAmountFiles";
            this.lbAmountFiles.Size = new System.Drawing.Size(43, 13);
            this.lbAmountFiles.TabIndex = 9;
            this.lbAmountFiles.Text = "Amount";
            // 
            // lbIndexSpeedMax
            // 
            this.lbIndexSpeedMax.AutoSize = true;
            this.lbIndexSpeedMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbIndexSpeedMax.Location = new System.Drawing.Point(431, 31);
            this.lbIndexSpeedMax.Name = "lbIndexSpeedMax";
            this.lbIndexSpeedMax.Size = new System.Drawing.Size(39, 15);
            this.lbIndexSpeedMax.TabIndex = 7;
            this.lbIndexSpeedMax.Text = "0 mb";
            // 
            // lbIndexSpeedMin
            // 
            this.lbIndexSpeedMin.AutoSize = true;
            this.lbIndexSpeedMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbIndexSpeedMin.Location = new System.Drawing.Point(11, 31);
            this.lbIndexSpeedMin.Name = "lbIndexSpeedMin";
            this.lbIndexSpeedMin.Size = new System.Drawing.Size(41, 16);
            this.lbIndexSpeedMin.TabIndex = 6;
            this.lbIndexSpeedMin.Text = "0 mb";
            // 
            // pbIndexSpeed
            // 
            this.pbIndexSpeed.Location = new System.Drawing.Point(58, 31);
            this.pbIndexSpeed.Maximum = 1;
            this.pbIndexSpeed.Name = "pbIndexSpeed";
            this.pbIndexSpeed.Size = new System.Drawing.Size(367, 23);
            this.pbIndexSpeed.Step = 1;
            this.pbIndexSpeed.TabIndex = 5;
            // 
            // lbInstanceInfo
            // 
            this.lbInstanceInfo.AutoSize = true;
            this.lbInstanceInfo.Location = new System.Drawing.Point(816, 17);
            this.lbInstanceInfo.Name = "lbInstanceInfo";
            this.lbInstanceInfo.Size = new System.Drawing.Size(33, 13);
            this.lbInstanceInfo.TabIndex = 10;
            this.lbInstanceInfo.Text = "lbInfo";
            // 
            // gbScan
            // 
            this.gbScan.Controls.Add(this.lbScanSpeedMax);
            this.gbScan.Controls.Add(this.lbScanSpeedMin);
            this.gbScan.Controls.Add(this.pbScanSpeed);
            this.gbScan.Controls.Add(this.lbIndexFolder);
            this.gbScan.Location = new System.Drawing.Point(12, 1);
            this.gbScan.Name = "gbScan";
            this.gbScan.Size = new System.Drawing.Size(708, 71);
            this.gbScan.TabIndex = 11;
            this.gbScan.TabStop = false;
            this.gbScan.Text = "Scan speed";
            // 
            // lbScanSpeedMax
            // 
            this.lbScanSpeedMax.AutoSize = true;
            this.lbScanSpeedMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbScanSpeedMax.Location = new System.Drawing.Point(431, 38);
            this.lbScanSpeedMax.Name = "lbScanSpeedMax";
            this.lbScanSpeedMax.Size = new System.Drawing.Size(39, 15);
            this.lbScanSpeedMax.TabIndex = 12;
            this.lbScanSpeedMax.Text = "0 mb";
            // 
            // lbScanSpeedMin
            // 
            this.lbScanSpeedMin.AutoSize = true;
            this.lbScanSpeedMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbScanSpeedMin.Location = new System.Drawing.Point(11, 37);
            this.lbScanSpeedMin.Name = "lbScanSpeedMin";
            this.lbScanSpeedMin.Size = new System.Drawing.Size(41, 16);
            this.lbScanSpeedMin.TabIndex = 11;
            this.lbScanSpeedMin.Text = "0 mb";
            // 
            // pbScanSpeed
            // 
            this.pbScanSpeed.Location = new System.Drawing.Point(58, 37);
            this.pbScanSpeed.Maximum = 1;
            this.pbScanSpeed.Name = "pbScanSpeed";
            this.pbScanSpeed.Size = new System.Drawing.Size(367, 23);
            this.pbScanSpeed.Step = 1;
            this.pbScanSpeed.TabIndex = 10;
            // 
            // lbIndexFolder
            // 
            this.lbIndexFolder.AutoSize = true;
            this.lbIndexFolder.Location = new System.Drawing.Point(11, 16);
            this.lbIndexFolder.Name = "lbIndexFolder";
            this.lbIndexFolder.Size = new System.Drawing.Size(36, 13);
            this.lbIndexFolder.TabIndex = 9;
            this.lbIndexFolder.Text = "Folder";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(276, 329);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 12;
            this.button2.Text = "search";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // tbSearchText
            // 
            this.tbSearchText.Location = new System.Drawing.Point(138, 293);
            this.tbSearchText.Name = "tbSearchText";
            this.tbSearchText.Size = new System.Drawing.Size(366, 20);
            this.tbSearchText.TabIndex = 13;
            this.tbSearchText.Text = "start";
            // 
            // gbTotalSpeed
            // 
            this.gbTotalSpeed.Controls.Add(this.label1);
            this.gbTotalSpeed.Controls.Add(this.lbTotalSpeedMax);
            this.gbTotalSpeed.Controls.Add(this.lbTotalSpeedMin);
            this.gbTotalSpeed.Controls.Add(this.pbTotalSpeed);
            this.gbTotalSpeed.Location = new System.Drawing.Point(12, 222);
            this.gbTotalSpeed.Name = "gbTotalSpeed";
            this.gbTotalSpeed.Size = new System.Drawing.Size(708, 65);
            this.gbTotalSpeed.TabIndex = 10;
            this.gbTotalSpeed.TabStop = false;
            this.gbTotalSpeed.Text = "Total Speed";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(43, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Amount";
            // 
            // lbTotalSpeedMax
            // 
            this.lbTotalSpeedMax.AutoSize = true;
            this.lbTotalSpeedMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTotalSpeedMax.Location = new System.Drawing.Point(431, 31);
            this.lbTotalSpeedMax.Name = "lbTotalSpeedMax";
            this.lbTotalSpeedMax.Size = new System.Drawing.Size(39, 15);
            this.lbTotalSpeedMax.TabIndex = 7;
            this.lbTotalSpeedMax.Text = "0 mb";
            // 
            // lbTotalSpeedMin
            // 
            this.lbTotalSpeedMin.AutoSize = true;
            this.lbTotalSpeedMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lbTotalSpeedMin.Location = new System.Drawing.Point(11, 31);
            this.lbTotalSpeedMin.Name = "lbTotalSpeedMin";
            this.lbTotalSpeedMin.Size = new System.Drawing.Size(41, 16);
            this.lbTotalSpeedMin.TabIndex = 6;
            this.lbTotalSpeedMin.Text = "0 mb";
            // 
            // pbTotalSpeed
            // 
            this.pbTotalSpeed.Location = new System.Drawing.Point(58, 31);
            this.pbTotalSpeed.Maximum = 1;
            this.pbTotalSpeed.Name = "pbTotalSpeed";
            this.pbTotalSpeed.Size = new System.Drawing.Size(367, 23);
            this.pbTotalSpeed.Step = 1;
            this.pbTotalSpeed.TabIndex = 5;
            // 
            // rtbLog
            // 
            this.rtbLog.Location = new System.Drawing.Point(12, 368);
            this.rtbLog.Name = "rtbLog";
            this.rtbLog.Size = new System.Drawing.Size(695, 340);
            this.rtbLog.TabIndex = 14;
            this.rtbLog.Text = "";
            // 
            // lbArchive
            // 
            this.lbArchive.AutoSize = true;
            this.lbArchive.Location = new System.Drawing.Point(751, 368);
            this.lbArchive.Name = "lbArchive";
            this.lbArchive.Size = new System.Drawing.Size(81, 13);
            this.lbArchive.TabIndex = 15;
            this.lbArchive.Text = "archive counter";
            // 
            // btnImportIndexes
            // 
            this.btnImportIndexes.Location = new System.Drawing.Point(357, 329);
            this.btnImportIndexes.Name = "btnImportIndexes";
            this.btnImportIndexes.Size = new System.Drawing.Size(105, 23);
            this.btnImportIndexes.TabIndex = 16;
            this.btnImportIndexes.Text = "Import Indexes";
            this.btnImportIndexes.UseVisualStyleBackColor = true;
            this.btnImportIndexes.Click += new System.EventHandler(this.btnImportIndexes_Click);
            // 
            // btnCheckIndexes
            // 
            this.btnCheckIndexes.Location = new System.Drawing.Point(468, 329);
            this.btnCheckIndexes.Name = "btnCheckIndexes";
            this.btnCheckIndexes.Size = new System.Drawing.Size(98, 23);
            this.btnCheckIndexes.TabIndex = 17;
            this.btnCheckIndexes.Text = "Check Indexes";
            this.btnCheckIndexes.UseVisualStyleBackColor = true;
            this.btnCheckIndexes.Click += new System.EventHandler(this.btnCheckIndexes_Click);
            // 
            // btnStartJob
            // 
            this.btnStartJob.Location = new System.Drawing.Point(572, 329);
            this.btnStartJob.Name = "btnStartJob";
            this.btnStartJob.Size = new System.Drawing.Size(75, 23);
            this.btnStartJob.TabIndex = 18;
            this.btnStartJob.Text = "Job";
            this.btnStartJob.UseVisualStyleBackColor = true;
            this.btnStartJob.Click += new System.EventHandler(this.btnStartJob_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(965, 720);
            this.Controls.Add(this.btnStartJob);
            this.Controls.Add(this.btnCheckIndexes);
            this.Controls.Add(this.btnImportIndexes);
            this.Controls.Add(this.lbArchive);
            this.Controls.Add(this.rtbLog);
            this.Controls.Add(this.gbTotalSpeed);
            this.Controls.Add(this.tbSearchText);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.gbScan);
            this.Controls.Add(this.lbInstanceInfo);
            this.Controls.Add(this.gbIndexSpeed);
            this.Controls.Add(this.gbIOSpeed);
            this.Controls.Add(this.button1);
            this.Name = "MainForm";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.gbIOSpeed.ResumeLayout(false);
            this.gbIOSpeed.PerformLayout();
            this.gbIndexSpeed.ResumeLayout(false);
            this.gbIndexSpeed.PerformLayout();
            this.gbScan.ResumeLayout(false);
            this.gbScan.PerformLayout();
            this.gbTotalSpeed.ResumeLayout(false);
            this.gbTotalSpeed.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox gbIOSpeed;
        private System.Windows.Forms.Label lbIOSpeedMax;
        private System.Windows.Forms.Label lbIOSpeedMin;
        private System.Windows.Forms.ProgressBar pbIOSpeed;
        private System.Windows.Forms.GroupBox gbIndexSpeed;
        private System.Windows.Forms.Label lbAmountFiles;
        private System.Windows.Forms.Label lbIndexSpeedMax;
        private System.Windows.Forms.Label lbIndexSpeedMin;
        private System.Windows.Forms.ProgressBar pbIndexSpeed;
        private System.Windows.Forms.Label lbInstanceInfo;
        private System.Windows.Forms.GroupBox gbScan;
        private System.Windows.Forms.Label lbIndexFolder;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox tbSearchText;
        private System.Windows.Forms.Label lbScanSpeedMax;
        private System.Windows.Forms.Label lbScanSpeedMin;
        private System.Windows.Forms.ProgressBar pbScanSpeed;
        private System.Windows.Forms.GroupBox gbTotalSpeed;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lbTotalSpeedMax;
        private System.Windows.Forms.Label lbTotalSpeedMin;
        private System.Windows.Forms.ProgressBar pbTotalSpeed;
        private System.Windows.Forms.RichTextBox rtbLog;
        private System.Windows.Forms.Label lbArchive;
        private System.Windows.Forms.Button btnImportIndexes;
        private System.Windows.Forms.Button btnCheckIndexes;
        private System.Windows.Forms.Button btnStartJob;
    }
}

